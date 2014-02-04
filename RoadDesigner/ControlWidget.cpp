#include "ControlWidget.h"
#include "MainWindow.h"
#include "GLWidget.h"
#include "GraphUtil.h"
#include "GridFeature.h"
#include "RoadSegmentationUtil.h"
#include "RoadGenerator.h"

ControlWidget::ControlWidget(MainWindow* mainWin) : QDockWidget("Control Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	// set up the UI
	ui.setupUi(this);
	ui.checkBoxRoadTypeAvenue->setChecked(true);
	ui.checkBoxRoadTypeLocalStreet->setChecked(true);

	// register the event handlers
	connect(ui.checkBoxRoadTypeLocalStreet, SIGNAL(stateChanged(int)), this, SLOT(showLocalStreet(int)));
	connect(ui.pushButtonGenerateGrid, SIGNAL(clicked()), this, SLOT(generateGrid()));
	connect(ui.pushButtonGenerateRadial, SIGNAL(clicked()), this, SLOT(generateRadial()));

	hide();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

void ControlWidget::showLocalStreet(int flag) {
	mainWin->glWidget->roads.showLocalStreets = ui.checkBoxRoadTypeLocalStreet->isChecked();
	mainWin->glWidget->roads.setModified();

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Generate Grid]
 */
void ControlWidget::generateGrid() {
	if (mainWin->glWidget->selectedArea == -1) return;

	GridFeature gf(0);
	gf.load("grid_feature.xml");
	RoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->roads, mainWin->glWidget->areas[mainWin->glWidget->selectedArea], gf);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Generate Radial]
 */
void ControlWidget::generateRadial() {
	mainWin->glWidget->updateGL();
}


