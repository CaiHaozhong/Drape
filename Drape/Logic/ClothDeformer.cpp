#include "MeshContainer.h"
#include "ClothDeformer.h"
#include "SparseLinearEquationSolver.h"
#include "KNNSHelper.h"
#include <vector>
void ClothDeformer::deformPose( const Skeleton& skeleton )
{
	Mesh& cloth = globalMeshContatiner.getMeshRef(1);

	/* 构造LaplacianMeshEditor的输入面数组 */
	std::vector<LaplacianMeshEditorFace> faceList;
	typedef OpenMesh::PolyConnectivity::FaceHandle FaceHandle;
	typedef OpenMesh::PolyConnectivity::FaceVertexIter FaceVertexIter;
	std::for_each(cloth.faces_begin(), cloth.faces_end(), [&](const FaceHandle f_it){
		LaplacianMeshEditorFace f;
		for (FaceVertexIter face_v_it = cloth.cfv_iter(f_it); face_v_it.is_valid(); ++face_v_it)
		{
			f.v.push_back(face_v_it->idx());
		}
		faceList.push_back(f);
	});

	/* 构造LaplacianMeshEditor的输入顶点数组 */
	std::vector<LaplacianMeshEditorVertex> vertexList;
	typedef OpenMesh::PolyConnectivity::VertexHandle VertexHandle;
	std::for_each(cloth.vertices_begin(), cloth.vertices_end(), [&](VertexHandle vh){
		Mesh::Point p = cloth.point(vh);
		vertexList.push_back(LaplacianMeshEditorVertex(p.values_[0], p.values_[1], p.values_[2]));
	});

	/* 构造新顶点 */
	std::vector<LaplacianMeshEditorVertex> newVertexList = vertexList;
	computeDeltaVertices(skeleton, newVertexList);

	/* 设置数据 */
	LaplacianMeshEditor meshEditor(&faceList,&vertexList);
	meshEditor.setDeltaVertices(&newVertexList);

	/* 计算结果 */
	std::vector<LaplacianMeshEditorVertex> *result = meshEditor.compute();

	/* 更新Mesh的顶点 */
	int index = 0;
	for(auto v_it = cloth.vertices_begin(); v_it != cloth.vertices_end(); v_it++)
	{
		Mesh::Point& p = cloth.point(*v_it);
		LaplacianMeshEditorVertex newVertex = result->at(index++);
		p.values_[0] = newVertex.x;
		p.values_[1] = newVertex.y;
		p.values_[2] = newVertex.z;
	}
}

void ClothDeformer::computeDeltaVertices( const Skeleton& skeleton, std::vector<LaplacianMeshEditorVertex>& newVertexList )
{
	int meshVertiecsCount = newVertexList.size();
	typedef Skeleton::vertex_descriptor SkeletonNodeIndex;
	typedef Skeleton::vertex_property_type SkeletonNode;
	std::vector< std::vector<SkeletonNodeIndex> > vertexHasNodes;
	vertexHasNodes.resize(meshVertiecsCount);
	BOOST_FOREACH(SkeletonNodeIndex vd, boost::vertices(skeleton))
	{
		SkeletonNode node = skeleton[vd];
		std::vector<int> correspondences = node.correspondanceIndices;
		for (int i = 0; i < correspondences.size(); i++)
		{
			vertexHasNodes[correspondences.at(i)].push_back(vd);
		}
	}

	for (int i = 0; i < meshVertiecsCount; i++)
	{
		OpenMesh::Vec3d meshPoint = newVertexList.at(i).toOpenMeshVector();
		std::vector<SkeletonNodeIndex>& nodes = vertexHasNodes[i];		
		std::vector<double> invertDisList;/** 距离的倒数 **/
		double total = 0;
		for (int j = 0; j < nodes.size(); j++)
		{
			auto nodePoint = skeleton[nodes.at(j)].point;			
			OpenMesh::Vec3d v(nodePoint.x(), nodePoint.y(), nodePoint.z());			
			double invertDis = 1.0/(v - meshPoint).length();
// 			double dx = nodePoint.x() - meshPoint.x;
// 			double dy = nodePoint.y() - meshPoint.y;
// 			double dz = nodePoint.z() - meshPoint.z;
// 			double dis = dx*dx + dy*dy + dz*dz;
			total += invertDis;
			invertDisList.push_back(invertDis);
		}		
		for (int j = 0; j < nodes.size(); j++)
		{
			auto _delta = skeleton[nodes.at(j)].delta;
			OpenMesh::Vec3d delta(_delta.x(), _delta.y(), _delta.z());
			meshPoint += (invertDisList.at(j)/total * delta);
		}
		newVertexList[i] = LaplacianMeshEditorVertex(meshPoint.values_[0],meshPoint.values_[1],meshPoint.values_[2]);
	}
}
/*
x0
.
.
.
xn
y0
.
.
.
yn
z0
.
.
.
zn
*/
bool ClothDeformer::resolvePenetration( const Mesh& humanMesh, double eps)
{
	Mesh& clothMesh = globalMeshContatiner.getMeshRef(1);
	int clothVerticesCount = clothMesh.n_vertices();
	std::vector<double> rightB;
	std::vector< std::vector< std::pair<int, double> > > leftMatrix;//稀疏矩阵，pair为下标和值
	std::vector<bool> penetrationTest;
	std::vector<int> nnsHumanVertex;
	nnsHumanVertex.resize(clothVerticesCount,-1);
	computeNNSHumanVertex(clothMesh, humanMesh, nnsHumanVertex);
	computePenetrationVertices(clothMesh, humanMesh,nnsHumanVertex,penetrationTest);
	/** 构造左矩阵和右向量 **/
	for(int i = 0; i < clothVerticesCount; i++)
	{
		auto curVertex = clothMesh.point(OpenMesh::VertexHandle(i)).values_;
		/** 一行 **/
		std::vector< std::pair<int, double> > row;	
		if(penetrationTest.at(i) == false)
		{						
			for (int j = 0; j < 3; j++)
			{
				row.clear();
				row.push_back(std::make_pair(i+j*clothVerticesCount,1));
				leftMatrix.push_back(row);
				rightB.push_back(curVertex[j]);
			}
		}
		else
		{
			auto curNNSHumanVertexIndex = nnsHumanVertex.at(i);
			auto curNNSHumanVertex = humanMesh.point(OpenMesh::VertexHandle(curNNSHumanVertexIndex)).values_;
			auto curNNSHumanNormal = humanMesh.normal(OpenMesh::VertexHandle(curNNSHumanVertexIndex)).values_;
			row.clear();
			row.push_back(std::make_pair(i,curNNSHumanNormal[0]));
			row.push_back(std::make_pair(i+clothVerticesCount,curNNSHumanNormal[1]));
			row.push_back(std::make_pair(i+2*clothVerticesCount,curNNSHumanNormal[2]));
			leftMatrix.push_back(row);
			rightB.push_back(curNNSHumanNormal[0]*curNNSHumanVertex[0]
			+curNNSHumanNormal[1]*curNNSHumanVertex[1]
			+curNNSHumanNormal[2]*curNNSHumanVertex[2]
			-eps);
		}
	}
	std::vector< std::vector<Mesh::VertexHandle> > adjList;
	computeAdjList(clothMesh,adjList);
	for (int i = 0; i < clothVerticesCount; i++)
	{

	}
	std::vector<double> ret;
	bool isSuccess = SparseLinearEquationSolver().solve(leftMatrix,rightB, 3*clothVerticesCount, ret);
	if(isSuccess)
	{
		auto it_end = clothMesh.vertices_end();
		int index = 0;
		for(Mesh::VertexIter v_it = clothMesh.vertices_begin(); v_it != it_end; v_it++)
		{
			Mesh::Point& p = clothMesh.point(*v_it);
			p.values_[0] = ret[index];
			p.values_[1] = ret[index+clothVerticesCount];
			p.values_[2] = ret[index+clothVerticesCount*2];
		}
		return true;
	}
	else
		return false;
}

void ClothDeformer::computeNNSHumanVertex(const Mesh& clothMesh,  const Mesh& humanMesh, std::vector<int>& nnsHumanVertex )
{
	nnsHumanVertex.resize(clothMesh.n_vertices());
	std::vector<Point_3> points;
	for(Mesh::VertexIter it = humanMesh.vertices_begin(); it != humanMesh.vertices_end(); it++)
	{
		Mesh::Point openMeshPoint = humanMesh.point(*it);
		Point_3 cgalPoint(openMeshPoint.values_[0],openMeshPoint.values_[1],openMeshPoint.values_[2]);
		points.push_back(cgalPoint);
	}
	KNNSHelper knnsHelper(points);	
	int curClothVertexIndex = 0;
	for(Mesh::VertexIter it = clothMesh.vertices_begin(); it != clothMesh.vertices_end(); it++)
	{
		Mesh::Point openMeshPoint = clothMesh.point(*it);
		Point_3 cgalPoint(openMeshPoint.values_[0],openMeshPoint.values_[1],openMeshPoint.values_[2]);
		std::pair<int, double> ret = knnsHelper.singleNeighborSearch(cgalPoint);
		nnsHumanVertex[curClothVertexIndex++] = ret.first;
	}
}

void ClothDeformer::computePenetrationVertices( const Mesh& clothMesh, const Mesh& humanMesh, const std::vector<int>& nnsHumanVertex, std::vector<bool>& penetrationTest )
{
	penetrationTest.resize(clothMesh.n_vertices(),false);
	int curClothVertexIndex = 0;
	for(Mesh::VertexIter it = clothMesh.vertices_begin(); it != clothMesh.vertices_end(); it++)
	{
		Mesh::Point clothPoint = clothMesh.point(*it);
		auto nnsPointHandle = Mesh::VertexHandle(nnsHumanVertex.at(curClothVertexIndex));
		Mesh::Point nnsPoint = humanMesh.point(nnsPointHandle);
		Mesh::Normal nnsNormal = humanMesh.normal(nnsPointHandle);
		double dotProduct = (clothPoint-nnsPoint)|nnsNormal;
		if(dotProduct < 0)
			penetrationTest[curClothVertexIndex] = true;
		curClothVertexIndex += 1;
	}
}

void ClothDeformer::computeAdjList( const Mesh& clothMesh, std::vector< std::vector<Mesh::VertexHandle> >& adjList )
{
	int curClothVertexIndex = 0;
	adjList.resize(clothMesh.n_vertices(), std::vector<Mesh::VertexHandle>());
	for(Mesh::VertexIter it = clothMesh.vertices_begin(); it != clothMesh.vertices_end(); it++)
	{
		for(Mesh::ConstVertexVertexIter vvIt = clothMesh.cvv_begin(*it); vvIt.is_valid(); vvIt++)
		{
			adjList[curClothVertexIndex].push_back(*vvIt);
		}
		curClothVertexIndex += 1;
	}
}
