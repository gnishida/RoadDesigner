#include "ControlWidget.h"
#include "MainWindow.h"
#include "GLWidget.h"
#include <road/GraphUtil.h>
#include <road/feature/GridFeature.h>
#include <road/feature/RadialFeature.h>
#include <road/feature/GenericFeature.h>
#include <road/generator/RoadGenerator.h>
#include <road/generator/KDERoadGenerator.h>
#include <road/generator/KDERoadGenerator2.h>

ControlWidget::ControlWidget(MainWindow* mainWin) : QDockWidget("Control Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	// set up the UI
	ui.setupUi(this);
	ui.checkBoxRoadTypeHighway->setChecked(true);
	ui.checkBoxRoadTypeBoulevard->setChecked(true);
	ui.checkBoxRoadTypeAvenue->setChecked(true);
	ui.checkBoxRoadTypeLocalStreet->setChecked(true);
	ui.lineEditIteration->setText("1000");
	ui.checkBoxLocalStreets->setChecked(false);

	// register the event handlers
	connect(ui.checkBoxRoadTypeHighway, SIGNAL(stateChanged(int)), this, SLOT(showRoad(int)));
	connect(ui.checkBoxRoadTypeBoulevard, SIGNAL(stateChanged(int)), this, SLOT(showRoad(int)));
	connect(ui.checkBoxRoadTypeAvenue, SIGNAL(stateChanged(int)), this, SLOT(showRoad(int)));
	connect(ui.checkBoxRoadTypeLocalStreet, SIGNAL(stateChanged(int)), this, SLOT(showRoad(int)));
	connect(ui.pushButtonGenerateKDE, SIGNAL(clicked()), this, SLOT(generateKDE()));
	connect(ui.pushButtonClear, SIGNAL(clicked()), this, SLOT(clear()));
	connect(ui.pushButtonConnect, SIGNAL(clicked()), this, SLOT(connectRoads()));

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

	int iteration = ui.lineEditIteration->text().toInt();
	bool addAvenuesOnBoundary = ui.checkBoxAddAvenuesOnBoundary->isChecked();
	bool localStreets = ui.checkBoxLocalStreets->isChecked();
	int orientation = ui.dialOrientation->value() - 180;

	RoadFeature rf;
	rf.load(filename);

	if (orientation != 0) {
		rf.rotate(orientation);
	}

	RoadGenerator rg;
	rg.generateRoadNetwork(mainWin->glWidget->areas[mainWin->glWidget->selectedArea].roads, mainWin->glWidget->areas[mainWin->glWidget->selectedArea].area, rf, addAvenuesOnBoundary, iteration, localStreets);
	//rg.generateRoadNetwork(mainWin->glWidget->areas.roads, mainWin->glWidget->areas[mainWin->glWidget->selectedArea].area, rf, addAvenuesOnBoundary, iteration, localStreets);
	mainWin->glWidget->updateGL();
}

void ControlWidget::clear() {
	mainWin->glWidget->areas[mainWin->glWidget->selectedArea].roads.clear();

	mainWin->glWidget->updateGL();
}

/**
 * エリア間の境界上で、エッジができる限りつながるように、微調整する。
 */
void ControlWidget::connectRoads() {
	mainWin->glWidget->areas.mergeRoads();
	KDERoadGenerator::connectRoads(mainWin->glWidget->areas.roads, 200.0f, 0.15f);
	//KDERoadGenerator::connectRoads2(mainWin->glWidget->areas, 300.0f, 0.2f);

	mainWin->glWidget->updateGL();
}
