#include "MeshContainer.h"

MeshContainer::MeshContainer(void)
{
}


MeshContainer::~MeshContainer(void)
{
}

bool MeshContainer::addMeshFromFile( const char* fileName )
{
	mMeshList.push_back(Mesh());
	Mesh& _mesh = mMeshList[mMeshList.size()-1];
	_mesh.request_vertex_normals();
	OpenMesh::IO::Options opt;
	if(!OpenMesh::IO::read_mesh(_mesh,fileName,opt))
	{
		mMeshList.pop_back();
		return false;
	}
	if ( !opt.check( OpenMesh::IO::Options::VertexNormal ) )
	{
		// we need face normals to update the vertex normals
		_mesh.request_face_normals();
		// let the mesh update the normals
		_mesh.update_normals();
		// dispose the face normals, as we don't need them anymore
		_mesh.release_face_normals();
	}
	return true;
}

Mesh& MeshContainer::getMeshRef( int i )
{
	assert(i < mMeshList.size());
	return mMeshList[i];
}

int MeshContainer::size()
{
	return mMeshList.size();
}

void MeshContainer::clearAllMesh()
{
	mMeshList.clear();
}

MeshContainer globalMeshContatiner;