#include "SkeletonViewer.h"
#include <climits>

SkeletonViewer::SkeletonViewer(void)
{
}

SkeletonViewer::SkeletonViewer( QWidget* parent ) : QGLViewerWidget(parent)
{

}


SkeletonViewer::~SkeletonViewer(void)
{
}

void SkeletonViewer::draw_scene( const std::string& _draw_mode )
{
	for(int i = 0; i < globalSkeletonContainer.size(); i++)
	{
		drawSkeleton(globalSkeletonContainer.getSkeletonRef(i));
	}
}

void SkeletonViewer::updateSkeleton()
{
	OpenMesh::Vec3f bbMin, bbMax;
	float maxFloat = std::numeric_limits<float>::max();
	float minFloat = std::numeric_limits<float>::min();
	bbMin = OpenMesh::Vec3f(maxFloat,maxFloat,maxFloat);
	bbMax = OpenMesh::Vec3f(minFloat,minFloat,minFloat);

	for(int i = 0; i < globalSkeletonContainer.size(); i++)
	{
		Skeleton& skeleton = globalSkeletonContainer.getSkeletonRef(i);

		BOOST_FOREACH(Skeleton_vertex v, boost::vertices(skeleton))
		{
			auto x = skeleton[v].point.x();
			auto y = skeleton[v].point.y();
			auto z = skeleton[v].point.z();
			bbMin.minimize( OpenMesh::Vec3f(x,y,z) );
			bbMax.maximize( OpenMesh::Vec3f(x,y,z) );
		}
	}

	// set center and radius
	/* 设置模型包围盒的中心， radius就是对角线长度的一半 */
	set_scene_pos( (bbMin+bbMax)*0.5, (bbMin-bbMax).norm()*0.5 );

	updateGL();
}

void SkeletonViewer::drawSkeleton( const Skeleton& skeleton )
{
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_LINES);
	BOOST_FOREACH(Skeleton_edge e, boost::edges(skeleton))
	{
		auto s = skeleton[boost::source(e, skeleton)].point;
		auto t = skeleton[boost::target(e, skeleton)].point;
		glVertex3f(s.x(), s.y(), s.z());
		glVertex3f(t.x(), t.y(), t.z());
	}
	glEnd();


	glPointSize(3);
	glBegin(GL_POINTS);
	BOOST_FOREACH(Skeleton_vertex v, boost::vertices(skeleton))
	{
		auto s = skeleton[v].point;
		glVertex3f(s.x(), s.y(), s.z());
	}
	glEnd();
	glPointSize(5);
	glPointSize(10);
	Kernel::Point_3 point = skeleton[skeleton.mNeckIndex].point;
	glBegin(GL_POINTS);
	glVertex3f(point.x(), point.y(), point.z());
	glEnd();
	
// 	glPointSize(8.0);
// 	glColor3f(1.0f, 0, 0);
// 	glBegin(GL_POINTS);
// 	BOOST_FOREACH(Skeleton_vertex v, skeleton.mLeftHandVertices)
// 	{
// 		auto s = skeleton[v].point;
// 		glVertex3f(s.x(), s.y(), s.z());
// 	}
// 	glEnd();
// 
// 	glPointSize(5.0);
// 	glColor3f(0, 1.0f, 0);
// 	glBegin(GL_POINTS);
// 	BOOST_FOREACH(Skeleton_vertex v, skeleton.mRightHandVertices)
// 	{
// 		auto s = skeleton[v].point;
// 		glVertex3f(s.x(), s.y(), s.z());
// 	}
// 	glEnd();

	glPointSize(2);
	glBegin(GL_POINTS);
	BOOST_FOREACH(Skeleton_vertex v, boost::vertices(skeleton))
	{
		auto s = skeleton[v].point;
		auto d = skeleton[v].delta;
		glVertex3f(s.x() + d.x(), s.y() + d.y(), s.z() + d.z());
	}
	glEnd();

	glPointSize(1.0);
}
