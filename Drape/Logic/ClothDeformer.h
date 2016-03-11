#pragma once
#include <vector>
#include "LaplacianMeshEditor.h"
#include "SkeletonDef.h"
#include "MeshDef.h"
class ClothDeformer
{
public:
	/************************************************************************/
	/* 其中，输入的参数为衣服的骨骼，已经和网格对齐，并且计算了节点的delta值
	/* 该函数改变globalMeshContainer中的衣服网格
	/************************************************************************/
	void deformPose(const Skeleton& skeleton);

	void resolvePenetration(const Mesh& humanMesh){/** To do **/}

	void physicalSimulate(){/** To do **/}

private:
	/** 传入衣服网格未移动的顶点的集合，即原始顶点 **/
	void computeDeltaVertices( const Skeleton& skeleton, std::vector<LaplacianMeshEditorVertex>& newVertexList);
	
};

