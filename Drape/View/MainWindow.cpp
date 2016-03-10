#include "MainWindow.h"
#include <QAction>
#include <QMenu>
#include "CompositionWidget.h"
MainWindow::MainWindow()
{
	mCompositionWidget = new CompositionWidget();
	setCentralWidget(mCompositionWidget);
	createAction();
	createMenu();
}

void MainWindow::createMenu()
{
	mFileMenu = menuBar()->addMenu(tr("&File"));
	mFileMenu->addAction(mOpenAct);
	mFileMenu->addAction(mClearAct);
	mFileMenu->addAction(mSaveAct);
	menuBar()->addSeparator();
	mFileMenu->addAction(mExitAct);

	mHelpMenu = menuBar()->addMenu(tr("&Help"));
	mHelpMenu->addAction(mAbout);
}

void MainWindow::createAction()
{
	/* FileMenu Items */
	mOpenAct = new QAction(tr("Open Mesh..."),this);
	connect(mOpenAct, SIGNAL(triggered()), mCompositionWidget->meshViewer(), SLOT(open_mesh_gui()));
	mClearAct = new QAction(tr("Clear Mesh"),this);
	connect(mClearAct, SIGNAL(triggered()), mCompositionWidget->meshViewer(), SLOT(clear_current_mesh()));
	mSaveAct = new QAction(tr("Save as a Mesh..."),this);
	mExitAct = new QAction(tr("Exit"),this);
	connect(mExitAct, SIGNAL(triggered()), this, SLOT(close()));

	/* HelpMenu Items */
	mAbout = new QAction(tr("About"),this);
	connect(mAbout, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::about()
{
	QMessageBox::about(this,tr("About"),
		tr("This <b>Drape</b> Application is an implementation of CaiHaozhong's Master thesis."));
}
