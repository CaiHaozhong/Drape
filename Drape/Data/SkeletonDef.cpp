#include "SkeletonDef.h"

void Skeleton::findNeck()
{
	/* find topest */
	Skeleton::vertex_iterator v_begin, v_end;
	std::tie(v_begin,v_end) = boost::vertices(*this);
	float maxY = (*this)[*v_begin].point.y();
	vertex_descriptor maxVertex = *v_begin;
	BOOST_FOREACH(vertex_descriptor v, std::make_pair(v_begin,v_end))
	{			
		auto y = (*this)[v].point.y();
		if(maxY < y)
		{
			maxY = y;
			maxVertex = v;
		}
	}

	vertex_descriptor tempVertex = maxVertex;
	vertex_descriptor lastVertex = maxVertex;
	/* find neck */
	while (true)
	{
		Skeleton::adjacency_iterator adjV_begin, adjV_end;
		
		std::tie(adjV_begin, adjV_end) = boost::adjacent_vertices(tempVertex, *this);
		printf("%d ",adjV_end-adjV_begin);
		if(adjV_end - adjV_begin > 2)
		{
			mNeckIndex = tempVertex;
			return;
		}
		auto it = adjV_begin;
		for (; it != adjV_end; it++)
		{
			if(*it != lastVertex)
				break;				
		}

		lastVertex = tempVertex;
// 		float minY = (*this)[*adjV_begin].point.y();
// 		Skeleton::adjacency_iterator nextV = adjV_begin;
// 		for (auto it = adjV_begin; it != adjV_end; it++)
// 		{
// 			float py = (*this)[*it].point.y();
// 			if(minY > py)
// 			{
// 				minY = py;
// 				nextV = it;
// 			}
// 		}
		if(it == adjV_end)
		{
			lastVertex = tempVertex;
			tempVertex = *adjV_begin;
		}
		else
		{
			tempVertex = *it;
		}
		
	}
}

void Skeleton::findHand()
{
	Skeleton& skeleton = *this;
	Skeleton::adjacency_iterator begin, end;
	std::tie(begin, end) = boost::adjacent_vertices(mNeckIndex,*this);
	vertex_descriptor left = *begin, right = *begin;
	float leftX = skeleton[left].point.x();
	float rightX = skeleton[right].point.x();
	for (auto it = begin; it != end; it++)
	{
		float curX = skeleton[*it].point.x();			
		if(curX < leftX)
		{
			left = *it;
			leftX = curX;
		}
		if(curX > rightX)
		{
			right = *it;
			rightX = curX;
		}
	}		
	vertex_descriptor test[2] = {left, right};
	for (int i = 0; i < 2; i++)
	{
		vertex_descriptor cur = test[i];
		vertex_descriptor lastVisit = mNeckIndex;
		while (true)
		{
			if(i == 0)
				mLeftHandVertices.push_back(cur);
			else
				mRightHandVertices.push_back(cur);
			vertex_descriptor next = cur;
			BOOST_FOREACH(vertex_descriptor vd, boost::adjacent_vertices(cur, skeleton))
			{
				if(vd != lastVisit)
					next = vd;
			}
			if(next == cur)
				break;
			lastVisit = cur;
			cur = next;
		}		
	}
}
