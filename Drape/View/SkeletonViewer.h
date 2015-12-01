#pragma once
#include "QGLViewerWidget.h"
#include "SkeletonContainer.h"
#include <OpenMesh/Core/Utils/vector_cast.hh>

typedef Skeleton::vertex_descriptor Skeleton_vertex;
typedef Skeleton::edge_descriptor Skeleton_edge;

class SkeletonViewer : public QGLViewerWidget
{
	Q_OBJECT
public:
	SkeletonViewer(void);
	~SkeletonViewer(void);

	SkeletonViewer(QWidget* parent);
	void draw_scene(const std::string& _draw_mode);

	void drawSkeleton(const Skeleton& skeleton);
public slots:
	void updateSkeleton();
};

