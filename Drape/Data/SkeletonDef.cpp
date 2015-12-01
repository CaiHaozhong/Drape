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
			neckIndex = tempVertex;
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
