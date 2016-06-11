#pragma once
#include "SkeletonDef.h"
#include "MeshDef.h"

class SkeletonExtractor
{
public:
	SkeletonExtractor(void);
	~SkeletonExtractor(void);

	void extract(const Mesh& mesh, Skeletonization::Skeleton& skeleton);

private:
	void convertToSurfaceMesh(const Mesh& mesh, Triangle_mesh& triangle_mesh);
};

