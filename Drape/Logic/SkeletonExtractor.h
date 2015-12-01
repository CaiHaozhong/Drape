#pragma once
#include "SkeletonDef.h"
#include "MeshDef.h"

typedef boost::graph_traits<Triangle_mesh>::vertex_descriptor vertex_descriptor;
typedef Skeleton::vertex_descriptor                           Skeleton_vertex;
typedef Skeleton::edge_descriptor                             Skeleton_edge;

class SkeletonExtractor
{
public:
	SkeletonExtractor(void);
	~SkeletonExtractor(void);

	void extract(const Mesh& mesh, Skeleton& skeleton);

private:
	void convertToSurfaceMesh(const Mesh& mesh, Triangle_mesh& triangle_mesh);
};

