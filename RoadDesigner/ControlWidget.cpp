#include "ControlWidget.h"
#include "MainWindow.h"
#include "GLWidget.h"
#include <common/GraphUtil.h>
#include <road/feature/GridFeature.h>
#include <road/feature/RadialFeature.h>
#include <road/feature/GenericFeature.h>
#include "RoadGenerator.h"

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
	connect(ui.pushButtonGenerateKDE, SIGNAL(clicked()), this, SLOT(generateKDE()));
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

	//GridFeature gf(mainWin->glWidget->selectedArea);
	RoadFeature rf;

	if (ui.radioButtonGridPattern1->isChecked()) {
		rf.load("grid_feature1.xml");
	} else {
		rf.load("grid_feature2.xml");
	}

	RoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->areas[mainWin->glWidget->selectedArea], rf);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Generate Radial]
 */
void ControlWidget::generateRadial() {
	if (mainWin->glWidget->selectedArea == -1) return;

	RoadFeature rf;
	rf.load("radial_feature1.xml");

	RoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->areas[mainWin->glWidget->selectedArea], rf);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Generate KDE-base]
 */
void ControlWidget::generateKDE() {
	if (mainWin->glWidget->selectedArea == -1) return;

	RoadFeature rf;
	rf.load("kde_feature1.xml");

	RoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->areas[mainWin->glWidget->selectedArea], rf);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Generate Generic]
 */
void ControlWidget::generateGeneric() {
	if (mainWin->glWidget->selectedArea == -1) return;

	RoadFeature rf;
	rf.load("generic_feature1.xml");

	RoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->areas[mainWin->glWidget->selectedArea], rf);

	mainWin->glWidget->updateGL();
}

