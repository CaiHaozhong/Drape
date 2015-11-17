#pragma once

#include "QGLViewerWidget.h"
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/vector_cast.hh>
#include <QMessageBox>
#include <QFileDialog>
#include <string>
#include <algorithm>
#include <tuple>
#include <fstream>
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + (bytes))
#define PRINT_ERROR(line) printf("%d:%s",line,gluErrorString(glGetError()));}
#define OUTPUT_MESHPOS(out,pos) out << p.values_[0] << ", " << p.values_[1] << ", " << p.values_[2];

class MeshViewer : public QGLViewerWidget
{
	Q_OBJECT
public://self

	typedef OpenMesh::TriMesh_ArrayKernelT<> Mesh;

	MeshViewer(void);
	~MeshViewer(void);

	MeshViewer(QWidget* parent);


public://derive
	void draw_scene(const std::string& _draw_mode);


	void readMesh(const QString& fileName);


	//************************************
	// FullName:        MeshViewer::prepareDrawMesh
	// Access:          public 
	// Returns:         void
	// Description:     初始化缓冲区对象和顶点数组
	//************************************
	void prepareDrawMesh();

	void drawMesh();

	void drawPoint();

	void adjustScene();
	

public slots:
	void open_mesh_gui();

	void clear_current_mesh();



private:
	Mesh _mesh;


	enum{
		VBO_VERTEX,
		VBO_NORMAL,
		VBO_INDEX,
		VBO_SIZE
	};
	GLuint _vbo[VBO_SIZE];
	GLuint* indices_array;


};

/*#undef BUFFER_OFFSET(bytes)*/