﻿#include "ControlWidget.h"
#include "MainWindow.h"
#include "GLWidget.h"
#include <common/global.h>
#include <road/GraphUtil.h>
#include <road/feature/GridFeature.h>
#include <road/feature/RadialFeature.h>
#include <road/feature/GenericFeature.h>
#include <road/generator/RoadGenerator.h>
#include <road/generator/KDERoadGenerator.h>

ControlWidget::ControlWidget(MainWindow* mainWin) : QDockWidget("Control Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	// set up the UI
	ui.setupUi(this);
	ui.checkBoxRoadTypeHighway->setChecked(true);
	ui.checkBoxRoadTypeBoulevard->setChecked(true);
	ui.checkBoxRoadTypeAvenue->setChecked(true);
	ui.checkBoxRoadTypeLocalStreet->setChecked(true);
	ui.lineEditNumIterations->setText("1000");
	ui.checkBoxLocalStreets->setChecked(false);
	ui.checkBoxInvadingCheck->setChecked(false);
	ui.radioButtonMultiSeeds->setChecked(true);
	ui.checkBoxConnectAvenues->setChecked(true);
	ui.checkBoxCropping->setChecked(true);
	ui.radioButtonCartesianCoordinate->setChecked(true);
	ui.lineEditWeightEdge->setText("1");
	ui.lineEditWeightLocation->setText("1");
	ui.lineEditWeightRepetition->setText("2000000");

	// register the event handlers
	connect(ui.checkBoxRoadTypeHighway, SIGNAL(stateChanged(int)), this, SLOT(showRoad(int)));
	connect(ui.checkBoxRoadTypeBoulevard, SIGNAL(stateChanged(int)), this, SLOT(showRoad(int)));
	connect(ui.checkBoxRoadTypeAvenue, SIGNAL(stateChanged(int)), this, SLOT(showRoad(int)));
	connect(ui.checkBoxRoadTypeLocalStreet, SIGNAL(stateChanged(int)), this, SLOT(showRoad(int)));
	connect(ui.pushButtonGenerateKDE, SIGNAL(clicked()), this, SLOT(generateKDE()));
	connect(ui.pushButtonClear, SIGNAL(clicked()), this, SLOT(clear()));
	connect(ui.pushButtonConnect, SIGNAL(clicked()), this, SLOT(connectRoads()));
	connect(ui.pushButtonRemoveDeadend, SIGNAL(clicked()), this, SLOT(removeDeadends()));

	hide();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

void ControlWidget::showRoad(int flag) {
	mainWin->glWidget->areas.roads.showHighways = ui.checkBoxRoadTypeHighway->isChecked();
	mainWin->glWidget->areas.roads.showBoulevard = ui.checkBoxRoadTypeBoulevard->isChecked();
	mainWin->glWidget->areas.roads.showAvenues = ui.checkBoxRoadTypeAvenue->isChecked();
	mainWin->glWidget->areas.roads.showLocalStreets = ui.checkBoxRoadTypeLocalStreet->isChecked();
	mainWin->glWidget->areas.roads.setModified();
	for (int i = 0; i < mainWin->glWidget->areas.size(); ++i) {
		mainWin->glWidget->areas[i].roads.showHighways = ui.checkBoxRoadTypeHighway->isChecked();
		mainWin->glWidget->areas[i].roads.showBoulevard = ui.checkBoxRoadTypeBoulevard->isChecked();
		mainWin->glWidget->areas[i].roads.showAvenues = ui.checkBoxRoadTypeAvenue->isChecked();
		mainWin->glWidget->areas[i].roads.showLocalStreets = ui.checkBoxRoadTypeLocalStreet->isChecked();
		mainWin->glWidget->areas[i].roads.setModified();
	}

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Generate KDE-base]
 */
void ControlWidget::generateKDE() {
	if (mainWin->glWidget->selectedArea == -1) return;

	QString filename = QFileDialog::getOpenFileName(this, tr("Open Feature file..."), "", tr("StreetMap Files (*.xml)"));

	if (filename.isEmpty()) {
		printf("Unable to open file\n");
		return;
	}

	G::global()["invadingCheck"] = ui.checkBoxInvadingCheck->isChecked();
	G::global()["numIterations"] = ui.lineEditNumIterations->text().toInt();
	G::global()["addAvenuesOnBoundary"] = ui.checkBoxAddAvenuesOnBoundary->isChecked();
	G::global()["generateLocalStreets"] = ui.checkBoxLocalStreets->isChecked();
	G::global()["saveSnappingImages"] = ui.checkBoxSaveSnappingImages->isChecked();
	G::global()["weightEdge"] = ui.lineEditWeightEdge->text().toFloat();
	G::global()["weightLocation"] = ui.lineEditWeightLocation->text().toFloat();
	G::global()["weightRepetition"] = ui.lineEditWeightRepetition->text().toFloat();

	G::global()["multiSeeds"] = ui.radioButtonMultiSeeds->isChecked();
	G::global()["connectAvenues"] = ui.checkBoxConnectAvenues->isChecked();
	G::global()["saveConnectingImages"] = ui.checkBoxSaveConnectingImages->isChecked();
	G::global()["cropping"] = ui.checkBoxCropping->isChecked();
	G::global()["areaScaling"] = ui.checkBoxAreaScaling->isChecked();

	G::global()["coordiniates"] = ui.radioButtonCartesianCoordinate->isChecked() ? "cartesian" : "polar";

	int orientation = ui.dialOrientation->value() - 180;
	bool areaScaling = ui.checkBoxAreaScaling->isChecked();

	RoadFeature rf;
	rf.load(filename);

	if (orientation != 0) {
		rf.rotate(orientation);
	}

	if (areaScaling) {
		rf.scale(mainWin->glWidget->areas[mainWin->glWidget->selectedArea].area);
	}


	RoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->areas[mainWin->glWidget->selectedArea].roads, mainWin->glWidget->areas[mainWin->glWidget->selectedArea].area, rf);
	//rg.generateRoadNetwork(mainWin->glWidget->areas.roads, mainWin->glWidget->areas[mainWin->glWidget->selectedArea].area, rf, addAvenuesOnBoundary, iteration, localStreets);
	mainWin->glWidget->updateGL();
}

void ControlWidget::clear() {
	if (mainWin->glWidget->selectedArea >= 0) {
		mainWin->glWidget->areas[mainWin->glWidget->selectedArea].roads.clear();
		mainWin->glWidget->selectedVertex = NULL;
		mainWin->glWidget->selectedEdge = NULL;
	}

	mainWin->glWidget->updateGL();
}

/**
 * エリア間の境界上で、エッジができる限りつながるように、微調整する。
 */
void ControlWidget::connectRoads() {
	mainWin->glWidget->areas.mergeRoads();
	KDERoadGenerator::connectRoads(mainWin->glWidget->areas.roads, mainWin->glWidget->areas, 200.0f, 0.15f);

	mainWin->glWidget->updateGL();
}

void ControlWidget::removeDeadends() {
	GraphUtil::removeDeadEnd(mainWin->glWidget->areas.roads);

	mainWin->glWidget->updateGL();
}
