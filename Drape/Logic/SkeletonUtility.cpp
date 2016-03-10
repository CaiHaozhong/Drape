#include "SkeletonUtility.h"
#include <cmath>


SkeletonUtility::SkeletonUtility()
{
}

SkeletonUtility::~SkeletonUtility()
{
}

void SkeletonUtility::write( const Skeleton& skeleton, const char* file )
{
	std::ofstream out(file);
	out << boost::num_vertices(skeleton) << std::endl;
	BOOST_FOREACH(Skeleton::vertex_descriptor vd, boost::vertices(skeleton))
	{
		auto point = skeleton[vd].point;
		auto delta = skeleton[vd].delta;
		out << point.x() << ' ' << point.y() << ' ' << point.z() 
			<< " delta: "
			<< delta.x() << ' ' << delta.y() << ' ' << delta.z() 
			<< std::endl;		
	}

	out << boost::num_edges(skeleton) << std::endl;
	BOOST_FOREACH(Skeleton::edge_descriptor ed, boost::edges(skeleton))
	{
		out << boost::source(ed,skeleton) << ' ' << boost::target(ed,skeleton) << std::endl;
	}

	out << "correspondance" << std::endl;

	BOOST_FOREACH(Skeleton::vertex_descriptor vd, boost::vertices(skeleton))
	{
		auto verticesList = skeleton[vd].vertices;
		out << verticesList.size() << ' ';
		for (int i = 0; i < verticesList.size(); i++)
		{
			out << verticesList.at(i) << ' ';
		}
		out << std::endl;
	}

	out.close();
}

void SkeletonUtility::read( Skeleton& skeleton, const char* file )
{
	std::ifstream input(file);
	int numVertices, numEdges;
	input >> numVertices;

	typedef Skeleton::vertex_property_type Node;
	std::vector<Node> nodeList;
	for (int i = 0; i < numVertices; i++)
	{			
		float x,y,z;
		input >> x >> y >> z;
		Node node;
		typedef Kernel::Point_3 SurfacePoint;
		SurfacePoint p(x,y,z);
		node.point = p;
		nodeList.push_back(node);
	}

	input >> numEdges;
	typedef std::pair<Skeleton::vertex_descriptor, Skeleton::vertex_descriptor> Edge;
	std::vector<Edge> edgeList;		
	for (int i = 0; i < numEdges; i++)
	{
		Edge edge;
		input >> edge.first >> edge.second;
		edgeList.push_back(edge);
	}

	std::string ignore;
	input >> ignore;

	for (int i = 0; i < numVertices; i++)
	{
		int size;
		int correspondance;
		std::string str;
		input >> size;
		for (int j = 0; j < size; j++)
		{
			input >> str;
			std::string numstring = str.substr(1);
			correspondance = atoi(numstring.c_str());
			nodeList[i].correspondanceIndices.push_back(correspondance);
		}
	}
	input.close();
	for (int i = 0; i < numVertices; i++)
	{
		boost::add_vertex(nodeList.at(i),skeleton);
	}

	for (int i = 0; i < numEdges; i++)
	{
		Edge edge = edgeList.at(i);
		boost::add_edge(edge.first, edge.second, skeleton);
	}
}

void SkeletonUtility::readIntoContainer( const char* file )
{
	Skeleton skeleton;
	read(skeleton,file);
	globalSkeletonContainer.pushSkeleton(skeleton);
}

void SkeletonUtility::skeletonMatch( Skeleton& human, Skeleton& cloth )
{
	std::vector<Skeleton::vertex_descriptor>& humanLeftHandVertices = human.getLeftHandVerticesRef();
	std::vector<Skeleton::vertex_descriptor>& clothLeftHandVertices = cloth.getLeftHandVerticesRef();
	skeletonMatch(human, cloth, humanLeftHandVertices, clothLeftHandVertices);

	std::vector<Skeleton::vertex_descriptor>& humanRightHandVertices = human.getRightHandVerticesRef();
	std::vector<Skeleton::vertex_descriptor>& clothRightHandVertices = cloth.getRightHandVerticesRef();
	skeletonMatch(human, cloth, humanRightHandVertices, clothRightHandVertices);
}

void SkeletonUtility::skeletonMatch( Skeleton& human, Skeleton& cloth, std::vector<Skeleton::vertex_descriptor>& humanHandVertices, std::vector<Skeleton::vertex_descriptor>& clothHandVertices )
{
	float handLen = 0;
	for(int i = 0; i < clothHandVertices.size()-1; i++)
	{
		handLen += dis(cloth, clothHandVertices.at(i), clothHandVertices.at(i+1));
	}
	int slice = 20;
	float interval = handLen / slice;
	std::vector<Point> interpolatePointList;
	float curDis = 0;
	float intervalAdd = interval;
	for (int i = 0; i < humanHandVertices.size()-1 && interpolatePointList.size() <= slice; i++)
	{
		Skeleton::vertex_descriptor from = humanHandVertices.at(i);
		Skeleton::vertex_descriptor to = humanHandVertices.at(i+1);
		float fromValue = curDis;
		curDis += dis(human, from, to);		
		while(curDis > intervalAdd)
		{
			Point fromPoint = human[from].point;
			Point toPoint = human[to].point;
			interpolatePointList.push_back(interpolate(fromValue, curDis, fromPoint, toPoint,intervalAdd));
			if(interpolatePointList.size() > slice)
				break;
			intervalAdd += interval;
		}
	}

	Point clothNeck = cloth[cloth.mNeckIndex].point;
	Point humanNeck = human[human.mNeckIndex].point;
	float dx = clothNeck.x() - humanNeck.x();
	float dy = clothNeck.y() - humanNeck.y();
	float dz = clothNeck.z() - humanNeck.z();
	for (int i = 0; i < interpolatePointList.size(); i++)
	{
		Point p = interpolatePointList[i];
		interpolatePointList[i] = Point(p.x() + dx, p.y() + dy, p.z() + dz);
	}

	//clothHandVertices.insert(clothHandVertices.begin(), cloth.mNeckIndex);
	interpolatePointList.insert(interpolatePointList.begin(), cloth[cloth.mNeckIndex].point);
	float fromValue = 0;
	intervalAdd = interval + dis(cloth, cloth.mNeckIndex, clothHandVertices.at(0));
	int curClothNode = 0;
	curDis = dis(cloth, cloth.mNeckIndex, clothHandVertices.at(curClothNode));
	for (int i = 1; i < interpolatePointList.size() && curClothNode < clothHandVertices.size(); i++)
	{
		while (curDis <= intervalAdd)
		{
			Point fromPoint = interpolatePointList.at(i - 1);
			Point toPoint = interpolatePointList.at(i);
			Point targetPoint = interpolate(fromValue, intervalAdd, fromPoint, toPoint, curDis);
			cloth[clothHandVertices.at(curClothNode)].delta = pointSub(targetPoint,cloth[clothHandVertices.at(curClothNode)].point);
			curClothNode++;
			if(curClothNode < clothHandVertices.size())
				curDis += dis(cloth, clothHandVertices.at(curClothNode-1), clothHandVertices.at(curClothNode));
			else
			{
				break;
			}
		}
		fromValue = intervalAdd;
		intervalAdd += interval;
	}
}

float SkeletonUtility::dis( Skeleton& skeleton, Skeleton::vertex_descriptor from, Skeleton::vertex_descriptor to )
{
	Point fromPoint = skeleton[from].point;
	Point toPoint = skeleton[to].point;
	float dx = fromPoint.x() - toPoint.x();
	float dy = fromPoint.y() - toPoint.y();
	float dz = fromPoint.z() - toPoint.z();
	return sqrt(dx * dx + dy * dy + dz * dz);
}

SkeletonUtility::Point SkeletonUtility::interpolate( float from, float to, Point& fromPoint, Point& toPoint, float target )
{
	float dx = toPoint.x() - fromPoint.x();
	float dy = toPoint.y() - fromPoint.y();
	float dz = toPoint.z() - fromPoint.z();

	float scale = (target - from)/ (to - from);

	return Point(fromPoint.x() + scale * dx, fromPoint.y() + scale * dy, fromPoint.z() + scale * dz);
}

SkeletonUtility::Point SkeletonUtility::pointSub( const Point& a, const Point& b )
{
	return Point(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}

