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
	size_t nodeCount = skeleton.nodeCount();
	size_t edgeCount = skeleton.edgeCount();
	typedef SkeletonNode::Point Point;
	std::ofstream out(file);
	out << nodeCount << "\n";
	for(size_t i = 0; i < nodeCount; i++){
		SkeletonNode* node = skeleton.nodeAt(i);
		float* point = node->point.values_;
		float* delta = node->delta.values_;
		Skeleton::IndexList& correspondanceList = node->correspondanceIndices;
		size_t correspondanceCount = correspondanceList.size();
		out << "point "
			<< point[0] << ' ' << point[1] << ' ' << point[2] 
			<< " delta "
			<< delta[0] << ' ' << delta[1] << ' ' << delta[2]
			<< " correspondance "	
		    << correspondanceCount << ' ';
		for (size_t j = 0; j < correspondanceCount; j++)
		{
			out << correspondanceList.at(j) << ' ';
		}
		out << "\n";	
	}

	out << edgeCount << "\n";

	for(size_t i = 0; i < edgeCount; i++){
		SkeletonEdge* edge = skeleton.edgeAt(i);
		out << edge->sourceVertex << ' ' << edge->targetVertex << "\n";
	}

	out.close();
}

void SkeletonUtility::read( Skeleton& skeleton, const char* file )
{
	skeleton.clear();
	std::ifstream input(file);
	int numVertices, numEdges, correspondance, correspondanceCount;
	std::string ignore;
	input >> numVertices;
	while(numVertices--){
		SkeletonNode* node = new SkeletonNode;
		input >> ignore
			  >> node->point.values_[0] >> node->point.values_[1] >> node->point.values_[2]
		      >> ignore
			  >> node->delta.values_[0] >> node->delta.values_[1] >> node->delta.values_[2]
			  >> ignore
		      >> correspondanceCount;
		while (correspondanceCount--){
			input >> correspondance;
			node->correspondanceIndices.push_back(correspondance);
		}
		skeleton.addNode(node);
	}
	input >> numEdges;
	while(numEdges--){
		SkeletonEdge* edge = new SkeletonEdge;
		input >> edge->sourceVertex >> edge->targetVertex;
		skeleton.addEdge(edge);
	}
	input.close();
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
	double dx = clothNeck.x() - humanNeck.x();
	double dy = clothNeck.y() - humanNeck.y();
	double dz = clothNeck.z() - humanNeck.z();
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
	double dx = fromPoint.x() - toPoint.x();
	double dy = fromPoint.y() - toPoint.y();
	double dz = fromPoint.z() - toPoint.z();
	return sqrt(dx * dx + dy * dy + dz * dz);
}

SkeletonUtility::Point SkeletonUtility::interpolate( float from, float to, Point& fromPoint, Point& toPoint, float target )
{
	double dx = toPoint.x() - fromPoint.x();
	double dy = toPoint.y() - fromPoint.y();
	double dz = toPoint.z() - fromPoint.z();

	float scale = (target - from)/ (to - from);

	return Point(fromPoint.x() + scale * dx, fromPoint.y() + scale * dy, fromPoint.z() + scale * dz);
}

SkeletonUtility::Point SkeletonUtility::pointSub( const Point& a, const Point& b )
{
	return Point(a.x() - b.x(), a.y() - b.y(), a.z() - b.z());
}

void SkeletonUtility::recomputeCorrepspondence( Skeleton& skeleton, Mesh& mesh, int numberOfCorrepondencePoint )
{
	std::vector<Point_3> points;
	for(Mesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); it++)
	{
		Mesh::Point& openMeshPoint = mesh.point(*it);
		Point_3 cgalPoint(openMeshPoint.values_[0],openMeshPoint.values_[1],openMeshPoint.values_[2]);
		points.push_back(cgalPoint);
	}
	KNNSHelper knnsHelper(points);		
	typedef Skeleton::vertex_property_type SkeletonNode;
	BOOST_FOREACH(Skeleton::vertex_descriptor vd, boost::vertices(skeleton))
	{
		SkeletonNode& skeletonNode = skeleton[vd];
		std::vector<int> lastCors = skeletonNode.correspondanceIndices;
		skeletonNode.correspondanceIndices.clear();
		auto skeletonPoint = skeletonNode.point;
		knnsHelper.kNeighborSearch(Point_3(skeletonPoint.x(),skeletonPoint.y(),skeletonPoint.z()),numberOfCorrepondencePoint);
		for(KNNSHelper::KNNSHelperIterator it = knnsHelper.begin(); it != knnsHelper.end(); it++)
		{
			int neighbor = knnsHelper.getIndex(it);
			bool correct = false;
			for (int i = 0; i < lastCors.size(); i++)
			{
				if(neighbor == lastCors.at(i))
				{
					correct = true;
					break;
				}
			}
			skeletonNode.correspondanceIndices.push_back(neighbor);
		}
	}
}