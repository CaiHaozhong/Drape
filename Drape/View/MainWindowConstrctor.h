#pragma once
#include <QMainWindow>
#include <QMenuBar>
#include "MeshViewer.h"
#include "SkeletonViewer.h"
#include <QtGui>
#include "CompositionWidget.h"
class MainWindowConstructor
{
public:	
	MainWindowConstructor(QMainWindow& _mainWin);

	~MainWindowConstructor();
	void create_menu();

	void create_mainView();

	void createFileMenu();

	void createFunctionMenu();

	//************************************
	// FullName:        MainWindowConstructor::construct
	// Access:          public 
	// Returns:         void
	// Description:     构造主窗口，包括创建菜单，主界面，连接UI与事件
	//************************************
	void construct();


	//************************************
	// FullName:        MainWindowConstructor::connectEvent
	// Access:          public 
	// Returns:         void
	// Description:     连接界面上的部件
	//************************************
	void connectEvent();
private:
	QMainWindow& mMainWindow;

	QAction* _openAct;
	QAction* _clearAct;
	QAction* _showGeodesicAct;
	QAction* _showReebGraphAct;
	MeshViewer* mMeshViewer;
	SkeletonViewer* mSkeletonViewer;

	CompositionWidget* mMainWidget;
};