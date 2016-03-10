#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <list>
#include <cmath>
#include <time.h>
#include <tuple>
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point_3;
typedef CGAL::Search_traits_3<Kernel> TreeTrait;
typedef CGAL::Orthogonal_k_neighbor_search<TreeTrait> Neighbor_search;
typedef Neighbor_search::Tree Kd_Tree;
class NNSHelper
{
	NNSHelper(std::list<Point_3>& pointList);

	NNSHelper();

	void insert(std::list<Point_3>& pointList);

	/** Return the nearest neighbor point and the distance from point q to the nnp **/
	std::pair<Point_3, double> query(Point_3& q);

private:
	Kd_Tree mKdTree;
};