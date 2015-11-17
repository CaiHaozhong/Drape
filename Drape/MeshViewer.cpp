#include "MeshViewer.h"
#include <GL/glu.h>
#include <GL/gl.h>
MeshViewer::MeshViewer(void)
{
	indices_array = nullptr;
}

MeshViewer::MeshViewer( QWidget* parent ) :QGLViewerWidget(parent)
{

	indices_array = nullptr;
}


MeshViewer::~MeshViewer(void)
{
	if(indices_array)
		delete [] indices_array;
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
	_mesh.request_vertex_normals();
	OpenMesh::IO::Options opt;
	if(!OpenMesh::IO::read_mesh(_mesh,fileName.toStdString(),opt))
	{

		QString msg = "Cannot read mesh from file:\n '";
		msg += fileName;
		msg += "'";
		QMessageBox::critical( NULL, windowTitle(), msg);
	}
	if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) )
	{
		// we need face normals to update the vertex normals
		_mesh.request_face_normals();
		// let the mesh update the normals
		_mesh.update_normals();
		// dispose the face normals, as we don't need them anymore
		_mesh.release_face_normals();
	}

	prepareDrawMesh();

	adjustScene();

	updateGL();
	setWindowTitle(QFileInfo(fileName).fileName());
}

void MeshViewer::prepareDrawMesh()
{
	indices_array = new GLuint[_mesh.n_faces() * 3];

	//初始化两个数组,typedef的作用域？
	typedef OpenMesh::PolyConnectivity::VertexHandle VertexHandle;
	typedef OpenMesh::PolyConnectivity::FaceHandle FaceHandle;
	typedef OpenMesh::PolyConnectivity::FaceVertexIter FaceVertexIter;

	int vi = 0;
	std::for_each(_mesh.faces_begin(), _mesh.faces_end(), [&](const FaceHandle f_it){
		int t = 0;
		for (FaceVertexIter face_v_it = _mesh.fv_iter(f_it); face_v_it.is_valid(); ++face_v_it)
		{
			t++;
			indices_array[vi++] = face_v_it->idx();
		}
		assert(t == 3, "not triangle face");
	});		


	makeCurrent();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	glGenBuffers(VBO_SIZE,_vbo);
	glBindBuffer(GL_ARRAY_BUFFER,_vbo[VBO_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _mesh.n_vertices() * 1 , _mesh.points(), GL_STATIC_DRAW);
	glEnableClientState(GL_VERTEX_ARRAY);		
	glVertexPointer(3,GL_FLOAT,0,0);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo[VBO_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _mesh.n_vertices() * 1 , _mesh.vertex_normals(), GL_STATIC_DRAW);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT,0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[VBO_INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint) * 3 * _mesh.n_faces(),indices_array,GL_STATIC_DRAW);

	//delete [] indices_array;
}

void MeshViewer::drawMesh()
{
	if(_mesh.n_vertices() == 0)
		return;

	glClear(GL_COLOR_BUFFER_BIT);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[VBO_INDEX]);
	glDrawElements(GL_TRIANGLES, _mesh.n_faces() * 3, GL_UNSIGNED_INT, 0);
}

void MeshViewer::drawPoint()
{
	if(_mesh.n_vertices() == 0)
		return;

	glClear(GL_COLOR_BUFFER_BIT);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[VBO_INDEX]);
	glPointSize(2);

	glDrawElements(GL_POINTS, _mesh.n_faces() * 3, GL_UNSIGNED_INT, 0);
}

void MeshViewer::adjustScene()
{
	// bounding box
	Mesh::ConstVertexIter vIt(_mesh.vertices_begin());
	Mesh::ConstVertexIter vEnd(_mesh.vertices_end());      

	OpenMesh::Vec3f bbMin, bbMax;

	bbMin = bbMax = OpenMesh::vector_cast<OpenMesh::Vec3f>(_mesh.point(*vIt));

	for (size_t count=0; vIt!=vEnd; ++vIt, ++count)
	{
		bbMin.minimize( OpenMesh::vector_cast<OpenMesh::Vec3f>(_mesh.point(*vIt)));
		bbMax.maximize( OpenMesh::vector_cast<OpenMesh::Vec3f>(_mesh.point(*vIt)));
	}


	// set center and radius
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
	_mesh.clear();
	glDeleteBuffers(VBO_SIZE,_vbo);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisableClientState(GL_VERTEX_ARRAY);
}

