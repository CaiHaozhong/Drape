#include <fstream>
#include "SkeletonContainer.h"
#include "SkeletonDef.h"
#include <iostream>
#include "MeshDef.h"
#include "KNNSHelper.h"
class SkeletonUtility
{
public:

	SkeletonUtility();
	~SkeletonUtility();

	void write(const Skeleton& skeleton, const char* file);

	void read(Skeleton& skeleton, const char* file);

	void readIntoContainer(const char* file);

	void skeletonMatch(Skeleton& human, Skeleton& cloth);

	void recomputeCorrepspondence(Skeleton& skeleton, Mesh& mesh, int numberOfCorrepondencePoint);
private:

	float dis(Skeleton& skeleton, Skeleton::vertex_descriptor from, Skeleton::vertex_descriptor to);

	Point interpolate(float from, float to, Point& fromPoint, Point& toPoint, float target);

	Point pointSub(const Point& a, const Point& b);

	void skeletonMatch(Skeleton& human, Skeleton& cloth, std::vector<Skeleton::vertex_descriptor>& humanHandVertices, std::vector<Skeleton::vertex_descriptor>& clothHandVertices);
};
