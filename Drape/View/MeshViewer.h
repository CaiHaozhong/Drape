#pragma once

#include "QGLViewerWidget.h"

#include <OpenMesh/Core/Utils/vector_cast.hh>
#include <QMessageBox>
#include <QFileDialog>
#include <string>
#include <algorithm>
#include <tuple>
#include <fstream>
#include "MeshContainer.h"

#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + (bytes))
#define PRINT_ERROR(line) printf("%d:%s",line,gluErrorString(glGetError()));}
#define OUTPUT_MESHPOS(out,pos) out << p.values_[0] << ", " << p.values_[1] << ", " << p.values_[2];
/* Vertex, Normal, Index */
typedef std::tuple<GLuint, GLuint, GLuint> VBOBufferName;

class MeshViewer : public QGLViewerWidget
{
	Q_OBJECT
public:
	MeshViewer(void);
	~MeshViewer(void);

	MeshViewer(QWidget* parent);

	void draw_scene(const std::string& _draw_mode);

	void readMesh(const QString& fileName);

	void drawMesh();

	void drawPoint();

	void adjustScene();
	
public slots:
	void open_mesh_gui();

	void clear_current_mesh();

	void updateScene();
private:



	std::vector<VBOBufferName> mVBOBufferNameList;

	//************************************
	// FullName:        MeshViewer::prepareDrawMesh
	// Access:          public 
	// Returns:         void
	// Description:     初始化缓冲区对象和顶点数组
	//************************************
	VBOBufferName prepareDrawMesh(const Mesh& _mesh);

	void initGlew();

};

/*#undef BUFFER_OFFSET(bytes)*/