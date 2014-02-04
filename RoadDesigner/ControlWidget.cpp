#include "ControlWidget.h"
#include "MainWindow.h"
#include "GLWidget.h"
#include "GraphUtil.h"
#include "RoadSegmentationUtil.h"

ControlWidget::ControlWidget(MainWindow* mainWin) : QDockWidget("Control Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	// set up the UI
	ui.setupUi(this);
	ui.checkBoxRoadTypeAvenue->setChecked(true);
	ui.checkBoxRoadTypeLocalStreet->setChecked(true);
	ui.lineEditGridMaxIteration->setText("2");
	ui.lineEditNumBins->setText("9");
	ui.lineEditMinTotalLength->setText("3000");
	ui.lineEditMinMaxBinRatio->setText("0.5");
	ui.lineEditGridAngleThreshold->setText("0.1");
	ui.lineEditGridVotingThreshold->setText("0.7");
	ui.lineEditGridExtendingDistanceThreshold->setText("20");
	ui.lineEditGridMinOBBLength->setText("300");

	ui.lineEditRadialMaxIteration->setText("3");
	ui.lineEditScale1->setText("0.05");
	ui.lineEditScale2->setText("0.1");
	ui.lineEditCenterErrorTol2->setText("80");
	ui.lineEditAngleThreshold2->setText("0.4");
	ui.lineEditScale3->setText("0.2");
	ui.lineEditCenterErrorTol3->setText("80");
	ui.lineEditAngleThreshold3->setText("0.2");
	ui.lineEditRadialVotingThreshold->setText("0.7");
	ui.lineEditRadialSeedDistance->setText("80");
	ui.lineEditRadialMinSeedDirections->setText("6");
	ui.lineEditRadialExtendingAngleThreshold->setText("0.1");

	// register the event handlers
	connect(ui.checkBoxRoadTypeLocalStreet, SIGNAL(stateChanged(int)), this, SLOT(showLocalStreet(int)));
	connect(ui.pushButtonDetectGrid, SIGNAL(clicked()), this, SLOT(detectGrid()));
	connect(ui.pushButtonDetectPlaza, SIGNAL(clicked()), this, SLOT(detectPlaza()));
	connect(ui.pushButtonDetectRadial, SIGNAL(clicked()), this, SLOT(detectRadial()));
	connect(ui.pushButtonDetectGridRadial, SIGNAL(clicked()), this, SLOT(detectGridRadial()));

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
 * Event handler for button [Detect Grid]
 */
void ControlWidget::detectGrid() {
	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Detect Plaza]
 */
void ControlWidget::detectPlaza() {
	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Detect Radial]
 */
void ControlWidget::detectRadial() {
	mainWin->glWidget->updateGL();
}

void ControlWidget::detectGridRadial() {
	mainWin->glWidget->updateGL();
}

