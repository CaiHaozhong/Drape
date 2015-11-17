#pragma once
#include <QMainWindow>
#include <QMenuBar>
#include "MeshViewer.h"
#include <QGLWidget>
class MainWindowConstructor
{
public:	
	MainWindowConstructor(QMainWindow& _mainWin):_mainWindow(_mainWin){}

	~MainWindowConstructor()
	{
// 		if(_meshViewer != nullptr)
// 			delete _meshViewer;
	}
	void create_menu()
	{
		createFileMenu();
		createFunctionMenu();
	}

	void create_mainView()
	{
		_meshViewer = new MeshViewer;
		_mainWindow.setCentralWidget(_meshViewer);
	}

	void createFileMenu()
	{
		QMenuBar* menuBar = _mainWindow.menuBar();
		QMenu* fileMenu = menuBar->addMenu(_mainWindow.tr("&File"));

		_openAct = new QAction(_mainWindow.tr("&Open mesh..."), &_mainWindow);
		_openAct->setShortcut(_mainWindow.tr("Ctrl+O"));
		_openAct->setStatusTip(_mainWindow.tr("Open a mesh file"));
		fileMenu->addAction(_openAct);
		

		_clearAct = new QAction(_mainWindow.tr("&clear mesh..."), &_mainWindow);
		_clearAct->setStatusTip(_mainWindow.tr("Clear current mesh"));
		fileMenu->addAction(_clearAct);

	}

	void createFunctionMenu()
	{
		QMenuBar* menuBar = _mainWindow.menuBar();
		QMenu* funcMenu = menuBar->addMenu(_mainWindow.tr("&Function"));

		_showGeodesicAct = new QAction(_mainWindow.tr("&Show Geodesic..."), &_mainWindow);		
		_showGeodesicAct->setStatusTip(_mainWindow.tr("Show Geodesic"));

		_showReebGraphAct = new QAction(_mainWindow.tr("&Show ReebGraph..."), &_mainWindow);		
		_showReebGraphAct->setStatusTip(_mainWindow.tr("Show ReebGraph"));

		funcMenu->addAction(_showGeodesicAct);
		funcMenu->addAction(_showReebGraphAct);
	}

	//************************************
	// FullName:        MainWindowConstructor::construct
	// Access:          public 
	// Returns:         void
	// Description:     构造主窗口，包括创建菜单，主界面，连接UI与事件
	//************************************
	void construct()
	{
		create_menu();
		create_mainView();
		connectEvent();
	}


	//************************************
	// FullName:        MainWindowConstructor::connectEvent
	// Access:          public 
	// Returns:         void
	// Description:     连接界面上的部件
	//************************************
	void connectEvent()
	{
		QObject::connect(_openAct,SIGNAL(triggered()),_meshViewer,SLOT(open_mesh_gui()));
		QObject::connect(_clearAct,SIGNAL(triggered()),_meshViewer,SLOT(clear_current_mesh()));
	}

	void setMainView(QWidget* view)
	{
		_mainWindow.setCentralWidget(view);
	}
private:
	QMainWindow& _mainWindow;

	QAction* _openAct;
	QAction* _clearAct;
	QAction* _showGeodesicAct;
	QAction* _showReebGraphAct;
	MeshViewer* _meshViewer;
};