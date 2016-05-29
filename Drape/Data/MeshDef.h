#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <climits>
typedef OpenMesh::TriMesh_ArrayKernelT<> _Mesh;

class Mesh : public _Mesh
{
public:
	Mesh():_Mesh()
	{
		mHasRequestAABB = false;
		float maxFloat = std::numeric_limits<float>::max();
		float minFloat = std::numeric_limits<float>::min();
		mMinPoint = OpenMesh::Vec3f(maxFloat,maxFloat,maxFloat);
		mMaxPoint = OpenMesh::Vec3f(minFloat,minFloat,minFloat);
	}

	/** ��ģ�ͱ���֮����Ҫ���¼����Χ�У��������� **/
	void requestRecomputeAABB()
	{
		mHasRequestAABB = false;
		requestAABB();
	}

	/** �����Χ�� **/
	void requestAABB()
	{
		if(mHasRequestAABB)
			return;
		for(Mesh::ConstVertexIter vIt = this->vertices_begin(); vIt != this->vertices_end(); vIt++)
		{
			const Mesh::Point& p = this->point(*vIt);
			mMinPoint.minimize(OpenMesh::vector_cast<OpenMesh::Vec3f>(p));
			mMaxPoint.maximize(OpenMesh::vector_cast<OpenMesh::Vec3f>(p));
		}
		mHasRequestAABB = true;
	}	
	OpenMesh::Vec3f getMin()
	{
		return mMinPoint;
	}
	OpenMesh::Vec3f getMax()
	{
		return mMaxPoint;
	}

	/** ��ȡ��Χ�жԽ��ߵĳ��� **/
	float getDigonalLen()
	{
		return (mMaxPoint-mMinPoint).length();
	}

	float getHeight()
	{
		return mMaxPoint.values_[1] - mMinPoint.values_[1];
	}
private:
	bool mHasRequestAABB;
	OpenMesh::Vec3f mMinPoint, mMaxPoint;
};
