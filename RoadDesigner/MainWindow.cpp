#include <road/GraphUtil.h>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	// setup the docking widgets
	controlWidget = new ControlWidget(this);

	// setup the toolbar
	ui.fileToolBar->addAction(ui.actionNew);
	ui.fileToolBar->addAction(ui.actionOpen);
	ui.areaToolBar->addAction(ui.actionAreaSelect);
	ui.areaToolBar->addAction(ui.actionAreaCreate);
	ui.areaToolBar->addAction(ui.actionHighwaySketch);
	ui.areaToolBar->addAction(ui.actionBoulevardSketch);

	ui.actionAreaSelect->setChecked(true);

	// register the menu's action handlers
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
	connect(ui.actionSaveRoads, SIGNAL(triggered()), this, SLOT(onSaveRoads()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.menuArea, SIGNAL(aboutToShow()), this, SLOT(onAreaMenu()) );
	connect(ui.actionAreaSelect, SIGNAL(triggered()), this, SLOT(onAreaSelect()));
	connect(ui.actionAreaCreate, SIGNAL(triggered()), this, SLOT(onAreaCreate()));
	connect(ui.actionHighwaySketch, SIGNAL(triggered()), this, SLOT(onHighwaySketch()));
	connect(ui.actionBoulevardSketch, SIGNAL(triggered()), this, SLOT(onBoulevardSketch()));
	connect(ui.actionControlWidget, SIGNAL(triggered()), this, SLOT(onShowControlWidget()));

	// setup the GL widget
	glWidget = new GLWidget(this);
	setCentralWidget(glWidget);

	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);

	mode = MODE_AREA_SELECT;
}

MainWindow::~MainWindow() {
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

void MainWindow::keyPressEvent(QKeyEvent* e) {
	glWidget->keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	glWidget->keyReleaseEvent(e);
}

void MainWindow::onNew() {
	// clear the areas
	glWidget->areas.clear();

	glWidget->updateGL();
}

void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Area file..."), "", tr("StreetMap Files (*.area)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	glWidget->areas.load(filename);
	glWidget->areas.setZ(glWidget->camera->dz);
	glWidget->updateGL();
	QApplication::restoreOverrideCursor();
}

void MainWindow::onSave() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Area file..."), "", tr("StreetMap Files (*.area)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	glWidget->areas.save(filename);
	QApplication::restoreOverrideCursor();
}

void MainWindow::onSaveRoads() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save StreetMap file..."), "", tr("StreetMap Files (*.gsm)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	RoadGraph roads;
	GraphUtil::mergeRoads(roads, glWidget->areas.roads);
	for (int i = 0; i < glWidget->areas.size(); ++i) {
		GraphUtil::mergeRoads(roads, glWidget->areas[i].roads);
	}
	GraphUtil::saveRoads(roads, filename);
	QApplication::restoreOverrideCursor();
}

void MainWindow::onAreaMenu() {
	ui.actionAreaSelect->setChecked(mode == MODE_AREA_SELECT);
	ui.actionAreaCreate->setChecked(mode == MODE_AREA_CREATE);
	ui.actionHighwaySketch->setChecked(mode == MODE_HIGHWAY_SKETCH);
	ui.actionBoulevardSketch->setChecked(mode == MODE_BOULEVARD_SKETCH);
}

void MainWindow::onAreaSelect() {
	mode = MODE_AREA_SELECT;
	ui.actionAreaCreate->setChecked(false);
	ui.actionHighwaySketch->setChecked(false);
	ui.actionBoulevardSketch->setChecked(false);
}

void MainWindow::onAreaCreate() {
	mode = MODE_AREA_CREATE;
	ui.actionAreaSelect->setChecked(false);
	ui.actionHighwaySketch->setChecked(false);
	ui.actionBoulevardSketch->setChecked(false);
}

void MainWindow::onHighwaySketch() {
	mode = MODE_HIGHWAY_SKETCH;
	ui.actionAreaCreate->setChecked(false);
	ui.actionAreaSelect->setChecked(false);
	ui.actionBoulevardSketch->setChecked(false);
}

void MainWindow::onBoulevardSketch() {
	mode = MODE_BOULEVARD_SKETCH;
	ui.actionAreaCreate->setChecked(false);
	ui.actionAreaSelect->setChecked(false);
	ui.actionHighwaySketch->setChecked(false);
}

void MainWindow::onShowControlWidget() {
	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}
