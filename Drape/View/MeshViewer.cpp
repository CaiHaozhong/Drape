#include "MeshViewer.h"
#include <climits>
#include <QMouseEvent>
#include <GL/glu.h>
#include <GL/gl.h>
#include <QDebug>
#include "GLError.h"
#include "SkeletonContainer.h"
MeshViewer::MeshViewer(void)
{
	initGlew();
	mAccumulateWheelTranslation = 0;
}

MeshViewer::MeshViewer( QWidget* parent ) :QGLViewerWidget(parent)
{
	initGlew();	
	mAccumulateWheelTranslation = 0;
}


MeshViewer::~MeshViewer(void)
{
}


void MeshViewer::draw_scene( const std::string& _draw_mode )
{
	if(mVBOBufferNameList.size() == 0)
		return;
	if (_draw_mode == "Wireframe")
	{
		glDisable(GL_LIGHTING);		
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);		
		drawMesh();
		//debugDraw();
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
//	curNodeIter = boost::vertices(globalSkeletonContainer.getSkeletonRef(1)).first;
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
	//setWindowTitle(QFileInfo(fileName).fileName());
	adjustScene();
	updateGL();
	check_gl_error();
}

VBOBufferName MeshViewer::prepareDrawMesh(const Mesh& _mesh)
{
	//初始化两个数组,typedef的作用域？
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
	check_gl_error();
	/* 生成三个VBO对象 */
	glGenBuffers(VBO_SIZE,_vbo);
	check_gl_error();
	/* 将顶点数据拷贝到顶点VBO中 */
	glBindBuffer(GL_ARRAY_BUFFER,_vbo[VBO_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _mesh.n_vertices() * 1 , _mesh.points(), GL_DYNAMIC_DRAW);
	glEnableClientState(GL_VERTEX_ARRAY);		
//	glVertexPointer(3,GL_FLOAT,0,0);
	check_gl_error();
	/* 将法线数据拷贝到法线VBO中 */
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[VBO_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _mesh.n_vertices() * 1 , _mesh.vertex_normals(), GL_STATIC_DRAW);
	glEnableClientState(GL_NORMAL_ARRAY);
//	glNormalPointer(GL_FLOAT,0, 0);
	check_gl_error();
	/* 将索引数据拷贝到索引的VBO中，而索引如何与顶点和法线关联起来，
	   靠的是glVertexPointer和glNormalPointer，在绘制的时候指定就好 */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[VBO_INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint) * 3 * _mesh.n_faces(),indices_array,GL_STATIC_DRAW);
	delete [] indices_array;
	check_gl_error();
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
	int meshCount = mVBOBufferNameList.size();
	
	for (int i = 0; i < meshCount; i++)
	{
		if(i == 0)
			glColor3f(0.4f,0.4f,0.4f);
		else
			glColor3f(1,1,1);
		enum{
			VBO_VERTEX,
			VBO_NORMAL,
			VBO_INDEX,
			VBO_SIZE
		};
		GLuint _vbo[VBO_SIZE];
		std::tie(_vbo[VBO_VERTEX], _vbo[VBO_NORMAL], _vbo[VBO_INDEX]) = mVBOBufferNameList.at(i);

		/* 指定顶点数据的offset */
		glBindBuffer(GL_ARRAY_BUFFER,_vbo[VBO_VERTEX]);
		glVertexPointer(3,GL_FLOAT,0,0);

		/* 指定法向量的offset */
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[VBO_NORMAL]);
		glNormalPointer(GL_FLOAT,0, 0);

		/* 绘制 */
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

	//OpenMesh::Vec3f bbMin, bbMax;
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
	/* 设置模型包围盒的中心， radius就是对角线长度的一半 */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	set_scene_pos( (bbMin+bbMax)*0.5, (bbMin-bbMax).norm()*0.5 );
	translate(OpenMesh::Vec3f(0,0,mAccumulateWheelTranslation));
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
	{
		readMesh(fileName);		
	}
}

void MeshViewer::clear_current_mesh()
{
	globalMeshContatiner.clearAllMesh();
	mVBOBufferNameList.clear();
	//glDeleteBuffers(VBO_SIZE,_vbo);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisableClientState(GL_VERTEX_ARRAY);
}

/* 网格模型发生变化时，更新界面 */
void MeshViewer::updateScene()
{
	if(globalMeshContatiner.size() > 1)
	{
		Mesh& mesh = globalMeshContatiner.getMeshRef(1);
		GLuint vertexVbo;
		std::tie(vertexVbo, std::ignore, std::ignore ) = mVBOBufferNameList.at(1);
		/* 指定顶点数据的offset */
		glBindBuffer(GL_ARRAY_BUFFER,vertexVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh.n_vertices() * 1 , mesh.points(), GL_DYNAMIC_DRAW);
		adjustScene();
	}	
	updateGL();
}

void MeshViewer::debugDraw()
{
	Skeleton clothSkeleton = globalSkeletonContainer.getSkeletonRef(1);
	Skeleton::vertex_property_type node = clothSkeleton[*curNodeIter];
	std::vector<int> cors = node.correspondanceIndices;

	/** Draw Skeleton **/ 
	glColor3f(0.0f,1.0f,0);
	glPointSize(3.0f);
	glBegin(GL_LINES);
	BOOST_FOREACH(Skeleton::edge_descriptor e, boost::edges(clothSkeleton))
	{
		auto s = clothSkeleton[boost::source(e, clothSkeleton)].point;
		auto t = clothSkeleton[boost::target(e, clothSkeleton)].point;
		glVertex3f(s.x(), s.y(), s.z());
		glVertex3f(t.x(), t.y(), t.z());
	}
	glEnd();

	/** Draw Skeleton Nodes **/ 
	glPointSize(5.0f);
	glColor3f(0.0f, 1.0f, 0);
	glBegin(GL_POINTS);	
	BOOST_FOREACH(Skeleton::vertex_descriptor e, boost::vertices(clothSkeleton))
	{
		auto p = clothSkeleton[e].point;
		glVertex3f(p.x(), p.y(), p.z());
	}
	glEnd();


	/** Draw target point **/
	glColor3f(1.0f, 0, 0);
	glPointSize(10.0f);
	auto highlightPoint = node.point;
	glBegin(GL_POINTS);
	glVertex3f(highlightPoint.x(), highlightPoint.y(), highlightPoint.z());
	glEnd();

	/** Draw cor points **/

	Mesh& clothMesh = globalMeshContatiner.getMeshRef(1);
	glPointSize(6.0f);
	glColor3f(1.0f,0,1.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < cors.size(); i++)
	{
		auto p = clothMesh.point(OpenMesh::ArrayKernel::VertexHandle(cors.at(i)));
		glVertex3f(p.values_[0],p.values_[1],p.values_[2]);
	}	
	glEnd();


	glPointSize(3);
	glColor3f(0,0.5,1);
	glBegin(GL_POINTS);
	BOOST_FOREACH(Skeleton_vertex v, boost::vertices(clothSkeleton))
	{
		auto s = clothSkeleton[v].point;
		auto d = clothSkeleton[v].delta;
		glVertex3f(s.x() + d.x(), s.y() + d.y(), s.z() + d.z());
	}
	glEnd();

	glPointSize(1.0f);
	glColor3f(1.0f,1.0f,1.0f);

}

void MeshViewer::debugOne()
{
	Skeleton clothSkeleton = globalSkeletonContainer.getSkeletonRef(1);
	if (curNodeIter < boost::vertices(clothSkeleton).second)
	{
		curNodeIter++;
	}
	else
	{
		curNodeIter = boost::vertices(clothSkeleton).first;
	}
	updateGL();
}

void MeshViewer::wheelEvent( QWheelEvent* events)
{
	QGLViewerWidget::wheelEvent(events);
	float d = -(float)events->delta() / 120.0 * 0.2 * radius();
	mAccumulateWheelTranslation += d;
}

