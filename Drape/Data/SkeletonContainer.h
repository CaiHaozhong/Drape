#pragma once
#include <vector>
#include "SkeletonDef.h"
#include "MeshDef.h"
#include "SkeletonExtractor.h"
class SkeletonContainer
{
public:
	SkeletonContainer(void);
	~SkeletonContainer(void);

	bool addSkeletonFromMesh(const Mesh& mesh);

	int size();

	Skeleton& getSkeletonRef(int i);
private:
	std::vector<Skeleton> mSkeletonList;
};

extern SkeletonContainer globalSkeletonContainer;