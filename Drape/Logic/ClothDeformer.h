#pragma once
#include <vector>
#include "LaplacianMeshEditor.h"
#include "SkeletonDef.h"
#include "MeshDef.h"
class ClothDeformer
{
public:
	/************************************************************************/
	/* ���У�����Ĳ���Ϊ�·��Ĺ������Ѿ���������룬���Ҽ����˽ڵ��deltaֵ
	/* �ú����ı�globalMeshContainer�е��·�����
	/************************************************************************/
	void deformPose(const Skeleton& skeleton);

	void resolvePenetration(const Mesh& humanMesh){/** To do **/}

	void physicalSimulate(){/** To do **/}

private:
	/** �����·�����δ�ƶ��Ķ���ļ��ϣ���ԭʼ���� **/
	void computeDeltaVertices( const Skeleton& skeleton, std::vector<LaplacianMeshEditorVertex>& newVertexList);
	
};

