#include "ControlWidget.h"
#include "MainWindow.h"
#include "GLWidget.h"
#include "GraphUtil.h"
#include "GridFeature.h"
#include "RadialFeature.h"
#include "GenericFeature.h"
#include "RoadSegmentationUtil.h"
#include "GridRoadGenerator.h"
#include "RadialRoadGenerator.h"
#include "GenericRoadGenerator.h"

ControlWidget::ControlWidget(MainWindow* mainWin) : QDockWidget("Control Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	// set up the UI
	ui.setupUi(this);
	ui.checkBoxRoadTypeAvenue->setChecked(true);
	ui.checkBoxRoadTypeLocalStreet->setChecked(true);
	ui.radioButtonGridPattern1->setChecked(true);

	// register the event handlers
	connect(ui.checkBoxRoadTypeLocalStreet, SIGNAL(stateChanged(int)), this, SLOT(showLocalStreet(int)));
	connect(ui.pushButtonGenerateGrid, SIGNAL(clicked()), this, SLOT(generateGrid()));
	connect(ui.pushButtonGenerateRadial, SIGNAL(clicked()), this, SLOT(generateRadial()));
	connect(ui.pushButtonGenerateGeneric, SIGNAL(clicked()), this, SLOT(generateGeneric()));

	hide();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

void ControlWidget::showLocalStreet(int flag) {
	//mainWin->glWidget->roads.showLocalStreets = ui.checkBoxRoadTypeLocalStreet->isChecked();
	//mainWin->glWidget->roads.setModified();

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Generate Grid]
 */
void ControlWidget::generateGrid() {
	if (mainWin->glWidget->selectedArea == -1) return;

	GridFeature gf(mainWin->glWidget->selectedArea);

	if (ui.radioButtonGridPattern1->isChecked()) {
		gf.load("grid_feature1.xml");
	} else {
		gf.load("grid_feature2.xml");
	}

	GridRoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->areas[mainWin->glWidget->selectedArea], gf);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Generate Radial]
 */
void ControlWidget::generateRadial() {
	if (mainWin->glWidget->selectedArea == -1) return;

	RadialFeature rf(mainWin->glWidget->selectedArea);
	rf.load("radial_feature1.xml");
	RadialRoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->areas[mainWin->glWidget->selectedArea], rf);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Generate Generic]
 */
void ControlWidget::generateGeneric() {
	if (mainWin->glWidget->selectedArea == -1) return;

	GenericFeature gf(mainWin->glWidget->selectedArea);
	gf.load("generic_feature1.xml");
	GenericRoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->areas[mainWin->glWidget->selectedArea], gf);

	mainWin->glWidget->updateGL();
}

