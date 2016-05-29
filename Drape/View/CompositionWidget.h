#pragma once
#include <QtGUI>
#include "MeshViewer.h"
#include "SkeletonViewer.h"
#include "SkeletonContainer.h"
#include "MeshContainer.h"
#include "ClothDeformer.h"

class CompositionWidget : public QWidget
{
	Q_OBJECT
public:
	CompositionWidget(void);
	~CompositionWidget(void);

private:
	MeshViewer* mMeshViewer;
	SkeletonViewer* mSkeletonViewer;
	ClothDeformer mClothDeformer;
public:
	MeshViewer* meshViewer();
	SkeletonViewer* skeletonViewer();

private slots:

	void extractSkeleton();

	void moveCloth();

	void deformCloth();

	void resolvePenetration();

	void startPhysicalSimulation();

	void stopPhysicalSimulation();

protected:
	void timerEvent(QTimerEvent *event);
	int mTimerId;
	int mInterval;
};

