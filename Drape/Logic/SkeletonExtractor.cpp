#include "SkeletonExtractor.h"


SkeletonExtractor::SkeletonExtractor(void)
{
}


SkeletonExtractor::~SkeletonExtractor(void)
{
}

void SkeletonExtractor::extract( const Mesh& mesh, Skeleton& skeleton )
{
	Triangle_mesh triangle_mesh;
	convertToSurfaceMesh(mesh, triangle_mesh);
	Skeletonization mcs(triangle_mesh);

	// 1. Contract the mesh by mean curvature flow.
	mcs.contract_geometry();

	// 2. Collapse short edges and split bad triangles.
	mcs.collapse_edges();
	mcs.split_faces();

	// 3. Fix degenerate vertices.
	mcs.detect_degeneracies();

	// Perform the above three steps in one iteration.
	mcs.contract();

	// Iteratively apply step 1 to 3 until convergence.
	mcs.contract_until_convergence();

	// Convert the contracted mesh into a curve skeleton and
	// get the correspondent surface points
	mcs.convert_to_skeleton(skeleton);

	std::cout << "Number of vertices of the skeleton: " << boost::num_vertices(skeleton) << "\n";
	std::cout << "Number of edges of the skeleton: " << boost::num_edges(skeleton) << "\n";

	// Output skeleton points and the corresponding surface points
// 	std::ofstream output;
// 	output.open("correspondance.cgal");
// 	BOOST_FOREACH(Skeleton_vertex v, vertices(skeleton))
// 		BOOST_FOREACH(vertex_descriptor vd, skeleton[v].vertices)
// 		output << "2 " << skeleton[v].point << "  " << boost::get(CGAL::vertex_point, triangle_mesh, vd)  << "\n";
// 	output.close();
}

void SkeletonExtractor::convertToSurfaceMesh( const Mesh& mesh, Triangle_mesh& triangle_mesh )
{
	typedef OpenMesh::PolyConnectivity::VertexIter VertexIterator;
	typedef OpenMesh::PolyConnectivity::FaceIter FaceIterator;
	typedef OpenMesh::PolyConnectivity::FaceVertexIter FaceVertexIterator;
	typedef OpenMesh::PolyConnectivity::VertexHandle VertexHandle;
	typedef OpenMesh::PolyConnectivity::FaceHandle FaceHandle;
	typedef Kernel::Point_3 SurfacePoint;

	std::for_each(mesh.vertices_begin(), mesh.vertices_end(), [&](const VertexHandle vIndex){
		Mesh::Point meshPoint = mesh.point(vIndex);
		SurfacePoint p(meshPoint.values_[0],meshPoint.values_[1],meshPoint.values_[2]);
		triangle_mesh.add_vertex(p);
	});

	typedef Triangle_mesh::size_type size_type;
	std::for_each(mesh.faces_begin(), mesh.faces_end(), [&](const FaceHandle fIndex){
		std::vector<size_type> face;
		for(FaceVertexIterator faceVertexIter = mesh.cfv_begin(fIndex); faceVertexIter.is_valid(); faceVertexIter++)
		{
			face.push_back(faceVertexIter->idx());
		}
		assert(face.size() == 3, "not triangle face");
		triangle_mesh.add_face(face);
	});
}
