#include "MainWindowConstrctor.h"
#include <QString>
#include "arthurwidgets.h"
MainWindowConstructor::MainWindowConstructor( QMainWindow& _mainWin ) :mMainWindow(_mainWin)
{

}

MainWindowConstructor::~MainWindowConstructor()
{
	// 		if(_meshViewer != nullptr)
	// 			delete _meshViewer;
}

void MainWindowConstructor::create_menu()
{
	createFileMenu();
	createFunctionMenu();
}

void MainWindowConstructor::create_mainView()
{
	mMainWidget = new CompositionWidget;
	mMainWindow.setCentralWidget(mMainWidget);

	/* …Ë÷√∑Á∏Ò */
	QStyle* style = new ArthurStyle;
	mMainWindow.setStyle(style);	
	QList<QWidget *> widgets = mMainWindow.findChildren<QWidget *>();
	foreach (QWidget *w, widgets)
		w->setStyle(style);
}

void MainWindowConstructor::createFileMenu()
{
	QMenuBar* menuBar = mMainWindow.menuBar();
	QMenu* fileMenu = menuBar->addMenu(mMainWindow.tr("&File"));

	_openAct = new QAction(mMainWindow.tr("&Open mesh..."), &mMainWindow);
	_openAct->setShortcut(mMainWindow.tr("Ctrl+O"));
	_openAct->setStatusTip(mMainWindow.tr("Open a mesh file"));
	fileMenu->addAction(_openAct);


	_clearAct = new QAction(mMainWindow.tr("&clear mesh..."), &mMainWindow);
	_clearAct->setStatusTip(mMainWindow.tr("Clear current mesh"));
	fileMenu->addAction(_clearAct);
}

void MainWindowConstructor::createFunctionMenu()
{
	QMenuBar* menuBar = mMainWindow.menuBar();
	QMenu* funcMenu = menuBar->addMenu(mMainWindow.tr("&Function"));

	_showGeodesicAct = new QAction(mMainWindow.tr("&Show Geodesic..."), &mMainWindow);		
	_showGeodesicAct->setStatusTip(mMainWindow.tr("Show Geodesic"));

	_showReebGraphAct = new QAction(mMainWindow.tr("&Show ReebGraph..."), &mMainWindow);		
	_showReebGraphAct->setStatusTip(mMainWindow.tr("Show ReebGraph"));

	funcMenu->addAction(_showGeodesicAct);
	funcMenu->addAction(_showReebGraphAct);
}

void MainWindowConstructor::construct()
{
	create_menu();
	create_mainView();
	connectEvent();
}

void MainWindowConstructor::connectEvent()
{
	QObject::connect(_openAct,SIGNAL(triggered()),mMainWidget,SLOT(open_mesh_gui()));
	QObject::connect(_clearAct,SIGNAL(triggered()),mMainWidget,SLOT(clear_current_mesh()));
}