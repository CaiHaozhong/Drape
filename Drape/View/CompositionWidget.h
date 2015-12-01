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



private slots:
	void open_mesh_gui();

	void clear_current_mesh();

	void extractSkeleton();

	void moveCloth();
};

