#include "CompositionWidget.h"
#include "SkeletonUtility.h"
#include <fstream>
#include "LaplacianMeshEditor.h"
#include <cmath>
#include "GLError.h"

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
	findNeckButton->setText(QObject::tr("Deform Cloth"));
	QPushButton* moveClothButton = new QPushButton(buttonGroup);
	moveClothButton->setText(QObject::tr("Move Cloth"));
	QPushButton* resolvePenetrationButton = new QPushButton(buttonGroup);
	resolvePenetrationButton->setText(QObject::tr("Resolve Penetration"));
	QPushButton* physicalSimuationButton = new QPushButton(buttonGroup);
	physicalSimuationButton->setText(QObject::tr("PhysicalSimuate"));
	QPushButton* physicalSimuationStopButton = new QPushButton(buttonGroup);
	physicalSimuationStopButton->setText(QObject::tr("Stop PhysicalSimuation"));
	 
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
	buttonGroupLayout->addWidget(resolvePenetrationButton);
	buttonGroupLayout->addWidget(physicalSimuationButton);
	buttonGroupLayout->addWidget(physicalSimuationStopButton);

	connect(extractSkeletonButton, SIGNAL(clicked()), this, SLOT(extractSkeleton()));
	connect(moveClothButton, SIGNAL(clicked()), this, SLOT(moveCloth()));
	connect(findNeckButton, SIGNAL(clicked()), this, SLOT(deformCloth()));
	connect(resolvePenetrationButton, SIGNAL(clicked()), this, SLOT(resolvePenetration()));
	connect(physicalSimuationButton, SIGNAL(clicked()), this, SLOT(startPhysicalSimulation()));
	connect(physicalSimuationStopButton, SIGNAL(clicked()), this, SLOT(stopPhysicalSimulation()));

//	SkeletonUtility utility;
//	utility.readIntoContainer("humanskeleton.txt");
//	utility.readIntoContainer("clothskeleton.txt");
//
// 	for (int i = 0; i < globalSkeletonContainer.size(); i++)
// 	{
// 		Skeleton& skeleton = globalSkeletonContainer.getSkeletonRef(i);
// 		skeleton.findNeck();
// 		skeleton.findHand();
// 	}
//  	utility.skeletonMatch(globalSkeletonContainer.getSkeletonRef(0), globalSkeletonContainer.getSkeletonRef(1));
// 
//  	mSkeletonViewer->updateSkeleton();		

	mInterval = 1000/25;
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
	/*SkeletonUtility u;
	u.write(globalSkeletonContainer.getSkeletonRef(0),"human03skeleton");
	u.write(globalSkeletonContainer.getSkeletonRef(1),"shirt01skeleton");*/
	mSkeletonViewer->updateSkeleton();
}

void CompositionWidget::moveCloth()
{
	SkeletonUtility u;
	u.recomputeCorrepspondence(globalSkeletonContainer.getSkeletonRef(1),globalMeshContatiner.getMeshRef(1),300);

	/* 计算移动向量 */
	SkeletonNode::Point humanNeck = globalSkeletonContainer.getSkeletonRef(0).nodeAt(globalSkeletonContainer.getSkeletonRef(0).getNeckIndex())->point;
	SkeletonNode::Point clothNeck = globalSkeletonContainer.getSkeletonRef(1).nodeAt(globalSkeletonContainer.getSkeletonRef(1).getNeckIndex())->point;
	float deltaX = humanNeck.values_[0] - clothNeck.values_[0];
	float deltaY = humanNeck.values_[1] - clothNeck.values_[1];
	float deltaZ = humanNeck.values_[2] - clothNeck.values_[2];
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
	mClothDeformer.deformPose(globalSkeletonContainer.getSkeletonRef(1));
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

void CompositionWidget::resolvePenetration()
{
	mClothDeformer.resolvePenetration(globalMeshContatiner.getMeshRef(0),-0.3);
	mMeshViewer->updateScene();
	mMeshViewer->updateScene();
}

void CompositionWidget::timerEvent( QTimerEvent *event )
{
	printf("Tick\n");
	mClothDeformer.physicalSimulate(mInterval);
	mMeshViewer->updateScene();
	mMeshViewer->updateScene();
}

void CompositionWidget::startPhysicalSimulation()
{
	mClothDeformer.setPhysicalConstrainer(new PhysicalConstrainer);
	mTimerId = this->startTimer(mInterval);
}

void CompositionWidget::stopPhysicalSimulation()
{
	if(mTimerId != 0)
	{
		this->killTimer(mTimerId);
		mTimerId = 0;
	}
}
