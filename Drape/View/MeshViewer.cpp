#include "MeshViewer.h"
#include <climits>
#include <GL/glu.h>
#include <GL/gl.h>
#include <QDebug>
MeshViewer::MeshViewer(void)
{
	initGlew();
}

MeshViewer::MeshViewer( QWidget* parent ) :QGLViewerWidget(parent)
{
	initGlew();
}


MeshViewer::~MeshViewer(void)
{
}


void MeshViewer::draw_scene( const std::string& _draw_mode )
{
	if (_draw_mode == "Wireframe")
	{
		glDisable(GL_LIGHTING);		
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);		
		drawMesh();
	}

	else if (_draw_mode == "Solid Flat")
	{
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glShadeModel(GL_FLAT);
		drawMesh();
	}

	else if (_draw_mode == "Solid Smooth")
	{
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glShadeModel(GL_SMOOTH);
		drawMesh();
	}
	
	else if(_draw_mode == "Geodesic")
	{
		glDisable(GL_LIGHTING);
		
		//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		drawPoint();
	}
}

void MeshViewer::readMesh( const QString& fileName )
{
	
/*	qDebug() << (const char*)fileName.toAscii();*/
	bool isSuccess = globalMeshContatiner.addMeshFromFile((const char*)fileName.toAscii());

	if(isSuccess == false)
	{
		QString msg = "Cannot read mesh from file:\n '";
		msg += fileName;
		msg += "'";
		QMessageBox::critical( NULL, tr("title"), msg);
		return;
	}

	Mesh& mesh = globalMeshContatiner.getMeshRef(globalMeshContatiner.size()-1);
	VBOBufferName vbo = prepareDrawMesh(mesh);
	mVBOBufferNameList.push_back(vbo);

	adjustScene();

	updateGL();
	//setWindowTitle(QFileInfo(fileName).fileName());
}

VBOBufferName MeshViewer::prepareDrawMesh(const Mesh& _mesh)
{
	//��ʼ����������,typedef��������
	typedef OpenMesh::PolyConnectivity::VertexHandle VertexHandle;
	typedef OpenMesh::PolyConnectivity::FaceHandle FaceHandle;
	typedef OpenMesh::PolyConnectivity::FaceVertexIter FaceVertexIter;

	int vi = 0;
	GLuint* indices_array  = new GLuint[_mesh.n_faces() * 3];
	std::for_each(_mesh.faces_begin(), _mesh.faces_end(), [&](const FaceHandle f_it){
		int t = 0;
		for (FaceVertexIter face_v_it = _mesh.cfv_iter(f_it); face_v_it.is_valid(); ++face_v_it)
		{
			t++;
			indices_array[vi++] = face_v_it->idx();
		}
		assert(t == 3, "not triangle face");
	});		

	enum{
		VBO_VERTEX,
		VBO_NORMAL,
		VBO_INDEX,
		VBO_SIZE
	};
	GLuint _vbo[VBO_SIZE];

	/* ��������VBO���� */
	glGenBuffers(VBO_SIZE,_vbo);

	/* ���������ݿ���������VBO�� */
	glBindBuffer(GL_ARRAY_BUFFER,_vbo[VBO_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _mesh.n_vertices() * 1 , _mesh.points(), GL_DYNAMIC_DRAW);
	glEnableClientState(GL_VERTEX_ARRAY);		
//	glVertexPointer(3,GL_FLOAT,0,0);

	/* ���������ݿ���������VBO�� */
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[VBO_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _mesh.n_vertices() * 1 , _mesh.vertex_normals(), GL_STATIC_DRAW);
	glEnableClientState(GL_NORMAL_ARRAY);
//	glNormalPointer(GL_FLOAT,0, 0);

	/* ���������ݿ�����������VBO�У�����������붥��ͷ��߹���������
	   ������glVertexPointer��glNormalPointer���ڻ��Ƶ�ʱ��ָ���ͺ� */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[VBO_INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint) * 3 * _mesh.n_faces(),indices_array,GL_STATIC_DRAW);
	delete [] indices_array;

	return std::make_tuple(_vbo[VBO_VERTEX], _vbo[VBO_NORMAL], _vbo[VBO_INDEX]);
}

void MeshViewer::initGlew()
{
	makeCurrent();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}

void MeshViewer::drawMesh()
{
	for (int i = 0; i < mVBOBufferNameList.size(); i++)
	{
		enum{
			VBO_VERTEX,
			VBO_NORMAL,
			VBO_INDEX,
			VBO_SIZE
		};
		GLuint _vbo[VBO_SIZE];
		std::tie(_vbo[VBO_VERTEX], _vbo[VBO_NORMAL], _vbo[VBO_INDEX]) = mVBOBufferNameList.at(i);

		/* ָ���������ݵ�offset */
		glBindBuffer(GL_ARRAY_BUFFER,_vbo[VBO_VERTEX]);
		glVertexPointer(3,GL_FLOAT,0,0);

		/* ָ����������offset */
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[VBO_NORMAL]);
		glNormalPointer(GL_FLOAT,0, 0);

		/* ���� */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[VBO_INDEX]);
		glDrawElements(GL_TRIANGLES, globalMeshContatiner.getMeshRef(i).n_faces() * 3, GL_UNSIGNED_INT, 0);
	}
	
}

void MeshViewer::drawPoint()
{
// 	if(_mesh.n_vertices() == 0)
// 		return;
// 
// 	glClear(GL_COLOR_BUFFER_BIT);
// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[VBO_INDEX]);
// 	glPointSize(2);
// 
// 	glDrawElements(GL_POINTS, _mesh.n_faces() * 3, GL_UNSIGNED_INT, 0);
}

void MeshViewer::adjustScene()
{

	OpenMesh::Vec3f bbMin, bbMax;
	float maxFloat = std::numeric_limits<float>::max();
	float minFloat = std::numeric_limits<float>::min();
	bbMin = OpenMesh::Vec3f(maxFloat,maxFloat,maxFloat);
	bbMax = OpenMesh::Vec3f(minFloat,minFloat,minFloat);
	for(int i = 0; i < globalMeshContatiner.size(); i++)
	{
		Mesh& _mesh = globalMeshContatiner.getMeshRef(i);
		// bounding box
		Mesh::ConstVertexIter vIt(_mesh.vertices_begin());
		Mesh::ConstVertexIter vEnd(_mesh.vertices_end());

		for (; vIt!=vEnd; ++vIt)
		{
			bbMin.minimize( OpenMesh::vector_cast<OpenMesh::Vec3f>(_mesh.point(*vIt)));
			bbMax.maximize( OpenMesh::vector_cast<OpenMesh::Vec3f>(_mesh.point(*vIt)));
		}

	}

	// set center and radius
	/* ����ģ�Ͱ�Χ�е����ģ� radius���ǶԽ��߳��ȵ�һ�� */
	set_scene_pos( (bbMin+bbMax)*0.5, (bbMin-bbMax).norm()*0.5 );
}

void MeshViewer::open_mesh_gui()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open mesh file"),
		tr(""),
		tr("OBJ Files (*.obj);;"
		"OFF Files (*.off);;"
		"STL Files (*.stl);;"
		"All Files (*)"));
	if (!fileName.isEmpty())
		readMesh(fileName);
}

void MeshViewer::clear_current_mesh()
{
	globalMeshContatiner.clearAllMesh();
	mVBOBufferNameList.clear();
	//glDeleteBuffers(VBO_SIZE,_vbo);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void MeshViewer::updateScene()
{
	if(globalMeshContatiner.size() > 1)
	{
		Mesh& mesh = globalMeshContatiner.getMeshRef(1);
		GLuint vertexVbo;
		std::tie(vertexVbo, std::ignore, std::ignore ) = mVBOBufferNameList.at(1);
		/* ָ���������ݵ�offset */
		glBindBuffer(GL_ARRAY_BUFFER,vertexVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh.n_vertices() * 1 , mesh.points(), GL_DYNAMIC_DRAW);
		adjustScene();
	}

	updateGL();
}
