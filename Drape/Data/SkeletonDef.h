#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/graph_traits_Surface_mesh.h>
#include <CGAL/boost/graph/properties_Surface_mesh.h>
#include <CGAL/Mean_curvature_flow_skeletonization.h>
#include <climits>


typedef CGAL::Simple_cartesian<double>                        Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3>                   Triangle_mesh;
typedef CGAL::Mean_curvature_flow_skeletonization<Triangle_mesh> Skeletonization;
//typedef Skeletonization::Skeleton                             Skeleton;

class Skeleton : public Skeletonization::Skeleton
{
public:
	enum SkeletonType
	{
		HUMAN,
		CLOTH
	};	

	void findNeck();

	void findHand();

	std::vector<vertex_descriptor>& getLeftHandVerticesRef()
	{
		return mLeftHandVertices;
	}


	std::vector<vertex_descriptor>& getRightHandVerticesRef()
	{
		return mRightHandVertices;
	}

public:
	vertex_descriptor mNeckIndex;
	SkeletonType mType;
	std::vector<vertex_descriptor> mLeftHandVertices;
	std::vector<vertex_descriptor> mRightHandVertices;
};