#include "NNSHelper.h"

NNSHelper::NNSHelper( std::list<Point_3>& pointList )
{
	this->insert(pointList);
}

NNSHelper::NNSHelper()
{

}

void NNSHelper::insert( std::list<Point_3>& pointList )
{
	mKdTree.insert(pointList.begin(), pointList.end());
}

std::pair<Point_3, double> NNSHelper::query( Point_3& q )
{
	Neighbor_search search(mKdTree, q, 1);
	Neighbor_search::iterator it = search.begin();		
	return std::make_pair(it->first, std::sqrt(it->second));
}

