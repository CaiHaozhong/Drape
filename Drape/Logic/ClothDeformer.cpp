#include "MeshContainer.h"
#include "ClothDeformer.h"

void ClothDeformer::deformPose( const Skeleton& skeleton )
{
	Mesh& cloth = globalMeshContatiner.getMeshRef(1);

	/* ����LaplacianMeshEditor������������ */
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

	/* ����LaplacianMeshEditor�����붥������ */
	std::vector<LaplacianMeshEditorVertex> vertexList;
	typedef OpenMesh::PolyConnectivity::VertexHandle VertexHandle;
	std::for_each(cloth.vertices_begin(), cloth.vertices_end(), [&](VertexHandle vh){
		Mesh::Point p = cloth.point(vh);
		vertexList.push_back(LaplacianMeshEditorVertex(p.values_[0], p.values_[1], p.values_[2]));
	});

	/* �����¶��� */
	std::vector<LaplacianMeshEditorVertex> newVertexList = vertexList;
	computeDeltaVertices(skeleton, newVertexList);

	/* �������� */
	LaplacianMeshEditor meshEditor(&faceList,&vertexList);
	meshEditor.setDeltaVertices(&newVertexList);

	/* ������ */
	std::vector<LaplacianMeshEditorVertex> *result = meshEditor.compute();

	/* ����Mesh�Ķ��� */
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
		std::vector<double> invertDisList;/** ����ĵ��� **/
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
