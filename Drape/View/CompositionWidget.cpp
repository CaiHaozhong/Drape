#include "CompositionWidget.h"
#include <fstream>

CompositionWidget::CompositionWidget(void)
{
	QGroupBox* viewGroup = new QGroupBox(this);

	QGroupBox* meshViewGroup = new QGroupBox(viewGroup);
	meshViewGroup->setTitle(tr("Mesh View"));
	//QDockWidget* dockMainView = new QDockWidget(QString("Mesh View"),viewGroup);
	//dockMainView->setAllowedAreas(Qt::TopDockWidgetArea| Qt::BottomDockWidgetArea);
	mMeshViewer = new MeshViewer(meshViewGroup);
	//dockMainView->setWidget(mMeshViewer);
	//mMainWindow.addDockWidget(Qt::TopDockWidgetArea, dockMainView);	
	//QDockWidget* dockSkeletonView = new QDockWidget(QString("Skeleton View"), viewGroup);
	//dockSkeletonView->setAllowedAreas(Qt::TopDockWidgetArea| Qt::BottomDockWidgetArea);
	QGroupBox* skeletonViewGroup = new QGroupBox(viewGroup);
	skeletonViewGroup->setTitle(tr("Skeleton View"));
	mSkeletonViewer = new SkeletonViewer(skeletonViewGroup);
	//dockSkeletonView->setWidget(mSkeletonViewer);
	//mMainWindow.addDockWidget(Qt::BottomDockWidgetArea, dockSkeletonView);
	 
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
}


CompositionWidget::~CompositionWidget(void)
{
}

void CompositionWidget::open_mesh_gui()
{
	mMeshViewer->open_mesh_gui();
}

void CompositionWidget::clear_current_mesh()
{
	mMeshViewer->clear_current_mesh();
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
	Kernel::Point_3 humanNeck = globalSkeletonContainer.getSkeletonRef(0)[globalSkeletonContainer.getSkeletonRef(0).neckIndex].point;
	Kernel::Point_3 clothNeck = globalSkeletonContainer.getSkeletonRef(1)[globalSkeletonContainer.getSkeletonRef(1).neckIndex].point;
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
	std::ofstream out("cloth.obj");
	bool d = OpenMesh::IO::write_mesh(cloth,out,std::string(".obj"));
	printf("Success:%d\n",d);
	mMeshViewer->updateScene();
	mMeshViewer->updateScene();
}
