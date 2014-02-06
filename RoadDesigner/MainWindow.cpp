#include "MainWindow.h"
#include "GraphUtil.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags) : QMainWindow(parent, flags) {
	ui.setupUi(this);

	// setup the docking widgets
	controlWidget = new ControlWidget(this);

	// setup the toolbar
	ui.fileToolBar->addAction(ui.actionNew);
	ui.fileToolBar->addAction(ui.actionOpen);
	ui.areaToolBar->addAction(ui.actionAreaSelect);
	ui.areaToolBar->addAction(ui.actionAreaCreate);

	ui.actionAreaSelect->setChecked(true);

	// register the menu's action handlers
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.menuArea, SIGNAL(aboutToShow()), this, SLOT(onAreaMenu()) );
	connect(ui.actionAreaSelect, SIGNAL(triggered()), this, SLOT(onAreaSelect()));
	connect(ui.actionAreaCreate, SIGNAL(triggered()), this, SLOT(onAreaCreate()));
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
	//glWidget->roads.clear();

	// clear the areas
	glWidget->areas.clear();

	glWidget->updateGL();
}

void MainWindow::onOpen() {
	/*
	QString filename = QFileDialog::getOpenFileName(this, tr("Open StreetMap file..."), "", tr("StreetMap Files (*.gsm)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	GraphUtil::loadRoads(glWidget->roads, filename);
	GraphUtil::copyRoads(glWidget->roads, glWidget->origRoads);
	glWidget->updateGL();
	QApplication::restoreOverrideCursor();
	*/
}

void MainWindow::onAreaMenu() {
	ui.actionAreaSelect->setChecked(mode == MODE_AREA_SELECT);
	ui.actionAreaCreate->setChecked(mode == MODE_AREA_CREATE);
	ui.actionSketch->setChecked(mode == MODE_SKETCH);
}

void MainWindow::onAreaSelect() {
	mode = MODE_AREA_SELECT;
}

void MainWindow::onAreaCreate() {
	mode = MODE_AREA_CREATE;
}

void MainWindow::onShowControlWidget() {
	controlWidget->show();
	addDockWidget(Qt::LeftDockWidgetArea, controlWidget);
}
