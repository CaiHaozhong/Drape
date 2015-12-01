#pragma once
#include <vector>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include "MeshDef.h"

class MeshContainer
{	
public:
	
	~MeshContainer(void);
	MeshContainer(void);

	bool addMeshFromFile(const char* fileName);

	Mesh& getMeshRef(int i);

	int size();

	void clearAllMesh();
private:

	std::vector<Mesh> mMeshList;

};

/* ����ȫ�ֱ��� */
extern MeshContainer globalMeshContatiner;
