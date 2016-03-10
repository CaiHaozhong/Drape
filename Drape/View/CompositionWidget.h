#pragma once
#include <QtGUI>
#include "MeshViewer.h"
#include "SkeletonViewer.h"
#include "SkeletonContainer.h"
#include "MeshContainer.h"

class CompositionWidget : public QWidget
{
	Q_OBJECT
public:
	CompositionWidget(void);
	~CompositionWidget(void);

private:
	MeshViewer* mMeshViewer;
	SkeletonViewer* mSkeletonViewer;

public:
	MeshViewer* meshViewer();
	SkeletonViewer* skeletonViewer();

private slots:

	void extractSkeleton();

	void moveCloth();

	void deformCloth();
};

