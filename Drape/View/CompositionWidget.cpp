#include "CompositionWidget.h"
#include "SkeletonUtility.h"
#include <fstream>
#include "LaplacianMeshEditor.h"
#include <cmath>

CompositionWidget::CompositionWidget(void)
{
	/* ViewGroup */
	QGroupBox* viewGroup = new QGroupBox(this);
	QGroupBox* meshViewGroup = new QGroupBox(viewGroup);
	meshViewGroup->setTitle(tr("Mesh View"));
	mMeshViewer = new MeshViewer(meshViewGroup);
	QGroupBox* skeletonViewGroup = new QGroupBox(viewGroup);
	skeletonViewGroup->setTitle(tr("Skeleton View"));
	mSkeletonViewer = new SkeletonViewer(skeletonViewGroup);
	 
	/* ControlGroup */
	QGroupBox* buttonGroup = new QGroupBox(this);
	buttonGroup->setTitle(QObject::tr("Drape"));
	QPushButton* extractSkeletonButton = new QPushButton(buttonGroup);
	extractSkeletonButton->setText(QObject::tr("Extract Skeleton"));	 
	QPushButton* findNeckButton = new QPushButton(buttonGroup);
	findNeckButton->setText(QObject::tr("Find Neck"));
	QPushButton* moveClothButton = new QPushButton(buttonGroup);
	moveClothButton->setText(QObject::tr("Move Cloth"));
	 
	/* Layout */
	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	QSizePolicy sizePolicy = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(4);
	viewGroup->setSizePolicy(sizePolicy);
	sizePolicy.setHorizontalStretch(1);
	buttonGroup->setSizePolicy(sizePolicy);
	mainLayout->addWidget(viewGroup);
	mainLayout->addWidget(buttonGroup);


	QVBoxLayout* viewGroupLayout = new QVBoxLayout(viewGroup);
	viewGroupLayout->addWidget(meshViewGroup);
	viewGroupLayout->addWidget(skeletonViewGroup);
	QVBoxLayout* meshViewGroupLayout = new QVBoxLayout(meshViewGroup);
	meshViewGroupLayout->addWidget(mMeshViewer);
	QVBoxLayout* skeletonViewGroupLayout = new QVBoxLayout(skeletonViewGroup);
	skeletonViewGroupLayout->addWidget(mSkeletonViewer);
	
	QVBoxLayout* buttonGroupLayout = new QVBoxLayout(buttonGroup);
	buttonGroupLayout->addWidget(extractSkeletonButton);
	buttonGroupLayout->addWidget(findNeckButton);
	buttonGroupLayout->addWidget(moveClothButton);

	connect(extractSkeletonButton, SIGNAL(clicked()), this, SLOT(extractSkeleton()));
	connect(moveClothButton, SIGNAL(clicked()), this, SLOT(moveCloth()));
	connect(findNeckButton, SIGNAL(clicked()), this, SLOT(deformCloth()));

	//return;

	SkeletonUtility utility;
	utility.readIntoContainer("humanskeleton.txt");
	utility.readIntoContainer("clothskeleton.txt");

	for (int i = 0; i < globalSkeletonContainer.size(); i++)
	{
		Skeleton& skeleton = globalSkeletonContainer.getSkeletonRef(i);
		skeleton.findNeck();
		skeleton.findHand();
	}
// 	utility.skeletonMatch(globalSkeletonContainer.getSkeletonRef(0), globalSkeletonContainer.getSkeletonRef(1));
// 
 	mSkeletonViewer->updateSkeleton();
	mMeshViewer->readMesh(QString("dapangziFromOff.obj"));
	mMeshViewer->readMesh(QString("2.obj"));
}


CompositionWidget::~CompositionWidget(void)
{
}


void CompositionWidget::extractSkeleton()
{
	for (int i = globalSkeletonContainer.size(); i < globalMeshContatiner.size(); i++)
	{
		Mesh& mesh = globalMeshContatiner.getMeshRef(i);
		globalSkeletonContainer.addSkeletonFromMesh(mesh);
	}
	mSkeletonViewer->updateSkeleton();
}

void CompositionWidget::moveCloth()
{
	/* 计算移动向量 */
	Kernel::Point_3 humanNeck = globalSkeletonContainer.getSkeletonRef(0)[globalSkeletonContainer.getSkeletonRef(0).mNeckIndex].point;
	Kernel::Point_3 clothNeck = globalSkeletonContainer.getSkeletonRef(1)[globalSkeletonContainer.getSkeletonRef(1).mNeckIndex].point;
	float deltaX = humanNeck.x() - clothNeck.x();
	float deltaY = humanNeck.y() - clothNeck.y();
	float deltaZ = humanNeck.z() - clothNeck.z();
	printf("%f,%f,%f\n",deltaX,deltaY,deltaZ);
	Mesh& cloth = globalMeshContatiner.getMeshRef(1);
	for(auto v_it = cloth.vertices_begin(); v_it != cloth.vertices_end(); v_it++)
	{
		Mesh::Point& p = cloth.point(*v_it);
		p.values_[0] += deltaX;
		p.values_[1] += deltaY;
		p.values_[2] += deltaZ;
	}
// 	std::ofstream out("cloth.obj");
// 	bool d = OpenMesh::IO::write_mesh(cloth,out,std::string(".obj"));
// 	printf("Success:%d\n",d);
	mMeshViewer->updateScene();
	mMeshViewer->updateScene();
}

void CompositionWidget::deformCloth()
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
	Skeleton& clothSkeleton = globalSkeletonContainer.getSkeletonRef(1);
	typedef Skeleton::vertex_property_type SkeletonNode;
	SkeletonUtility utility;
	utility.write(clothSkeleton,"debugSkeleton.txt");
	BOOST_FOREACH(Skeleton::vertex_descriptor vd, boost::vertices(clothSkeleton))
	{
		SkeletonNode node = clothSkeleton[vd];
		auto vertexDelta = node.delta;
		if(fabs(vertexDelta.x()) <= 0.000001f &&
			fabs(vertexDelta.y()) <= 0.000001f &&
			fabs(vertexDelta.z()) <= 0.000001f)
			continue;
		for(int i = 0 ; i < node.correspondanceIndices.size(); i++)
		{
			int index = clothSkeleton[vd].correspondanceIndices.at(i);
			newVertexList[index].x += vertexDelta.x();
			newVertexList[index].y += vertexDelta.y();
			newVertexList[index].z += vertexDelta.z();
		}
	}

	/* 设置数据 */
	LaplacianMeshEditor meshEditor(&faceList,&vertexList);
	meshEditor.setDeltaVertices(&newVertexList);

	/* Cholesky分解 */
	meshEditor.factorizeAprimeTxAprime();
	/* 解方程组 */
	std::vector<LaplacianMeshEditorVertex> *result = meshEditor.compute();

	/* 更改Mesh的顶点 */
	int index = 0;
	for(auto v_it = cloth.vertices_begin(); v_it != cloth.vertices_end(); v_it++)
	{
		Mesh::Point& p = cloth.point(*v_it);
		LaplacianMeshEditorVertex newVertex = result->at(index++);
		p.values_[0] = newVertex.x;
		p.values_[1] = newVertex.y;
		p.values_[2] = newVertex.z;
	}
	mMeshViewer->updateScene();
	mMeshViewer->updateScene();
}

MeshViewer* CompositionWidget::meshViewer()
{
	return mMeshViewer;
}

SkeletonViewer* CompositionWidget::skeletonViewer()
{
	return mSkeletonViewer;
}
