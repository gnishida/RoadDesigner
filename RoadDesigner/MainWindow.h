#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_MainWindow.h"
#include "ControlWidget.h"
#include "PropertyWidget.h"
#include "GLWidget.h"
#include <QToolBar>

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	static enum { MODE_AREA_SELECT = 0, MODE_AREA_CREATE, MODE_HIGHWAY_SKETCH, MODE_BOULEVARD_SKETCH, MODE_3DVIEW, MODE_TERRAIN, MODE_DEBUG };

public:
	Ui::MainWindow ui;
	ControlWidget* controlWidget;
	PropertyWidget* propertyWidget;
	GLWidget* glWidget;

	int mode;

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

protected:
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

public slots:
	void onNew();
	void onOpen();
	void onSave();
	void onSaveRoads();
	void onAreaMenu();
	void onAreaSelect();
	void onAreaCreate();
	void onHighwaySketch();
	void onBoulevardSketch();
	void on3DView();
	void onTerrain();
	void onDebug();
	void onShowControlWidget();
	void onShowPropertyWidget();
};

#endif // MAINWINDOW_H
