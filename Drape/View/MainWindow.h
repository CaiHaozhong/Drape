#pragma once
#include <QMainWindow>

class QAction;
class QMenu;
class CompositionWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();	

	void createMenu();
	void createAction();

private slots:
	void about();

private:
	CompositionWidget* mCompositionWidget;

	QMenu *mFileMenu;
	QMenu *mHelpMenu;

	/* FileMenu Items */
	QAction* mOpenAct;
	QAction* mClearAct;
	QAction* mSaveAct;
	QAction* mExitAct;

	/* HelpMenu Items */
	QAction* mAbout;

};