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
#include "SkeletonDef.h"


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

	void debugOne();
protected:
	void wheelEvent( QWheelEvent* event);
private:

	void debugDraw();	
//	Skeleton::vertex_iterator curNodeIter;

	std::vector<VBOBufferName> mVBOBufferNameList;

	//************************************
	// FullName:        MeshViewer::prepareDrawMesh
	// Access:          public 
	// Returns:         void
	// Description:     ��ʼ������������Ͷ�������
	//************************************
	VBOBufferName prepareDrawMesh(const Mesh& _mesh);

	void initGlew();

	OpenMesh::Vec3f bbMin, bbMax;

	double mAccumulateWheelTranslation;
};

/*#undef BUFFER_OFFSET(bytes)*/