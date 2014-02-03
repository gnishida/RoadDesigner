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
	int roadType = (ui.checkBoxRoadTypeAvenue->isChecked() ? 2 : 0) + (ui.checkBoxRoadTypeLocalStreet->isChecked() ? 1 : 0);

	int maxIteration = ui.lineEditGridMaxIteration->text().toInt();
	int numBins = ui.lineEditNumBins->text().toInt();
	float minTotalLength = ui.lineEditMinTotalLength->text().toFloat();
	float minMaxBinRatio = ui.lineEditMinMaxBinRatio->text().toFloat();
	float angleThreshold = ui.lineEditGridAngleThreshold->text().toFloat();
	float votingThreshold = ui.lineEditGridVotingThreshold->text().toFloat();
	float extendingDistanceThreshold = ui.lineEditGridExtendingDistanceThreshold->text().toFloat();
	float minOBBLength = ui.lineEditGridMinOBBLength->text().toFloat();

	GraphUtil::copyRoads(mainWin->glWidget->origRoads, mainWin->glWidget->roads);
	RoadSegmentationUtil::detectGrid(mainWin->glWidget->roads, mainWin->glWidget->selectedArea, roadType, mainWin->glWidget->gridFeatures, maxIteration, numBins, minTotalLength, minMaxBinRatio, angleThreshold, votingThreshold, extendingDistanceThreshold, minOBBLength);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Detect Plaza]
 */
void ControlWidget::detectPlaza() {
	GraphUtil::copyRoads(mainWin->glWidget->origRoads, mainWin->glWidget->roads);
	//RoadSegmentationUtil::detectPlaza(mainWin->glWidget->roads, mainWin->glWidget->selectedArea);

	mainWin->glWidget->updateGL();
}

/**
 * Event handler for button [Detect Radial]
 */
void ControlWidget::detectRadial() {
	int roadType = (ui.checkBoxRoadTypeAvenue->isChecked() ? 2 : 0) + (ui.checkBoxRoadTypeLocalStreet->isChecked() ? 1 : 0);

	int maxIteration = ui.lineEditRadialMaxIteration->text().toInt();
	float scale1 = ui.lineEditScale1->text().toFloat();
	float scale2 = ui.lineEditScale2->text().toFloat();
	float centerErrorTol2 = ui.lineEditCenterErrorTol2->text().toFloat();
	float angleThreshold2 = ui.lineEditAngleThreshold2->text().toFloat();
	float scale3 = ui.lineEditScale3->text().toFloat();
	float centerErrorTol3 = ui.lineEditCenterErrorTol3->text().toFloat();
	float angleThreshold3 = ui.lineEditAngleThreshold3->text().toFloat();
	float votingThreshold = ui.lineEditRadialVotingThreshold->text().toFloat();
	float seedDistance = ui.lineEditRadialSeedDistance->text().toFloat();
	float minSeedDirection = ui.lineEditRadialMinSeedDirections->text().toFloat();
	float extendingAngleThreshold = ui.lineEditRadialExtendingAngleThreshold->text().toFloat();

	GraphUtil::copyRoads(mainWin->glWidget->origRoads, mainWin->glWidget->roads);
	RoadSegmentationUtil::detectRadial(mainWin->glWidget->roads, mainWin->glWidget->selectedArea, roadType, mainWin->glWidget->radialFeatures, maxIteration, scale1, scale2, centerErrorTol2, angleThreshold2, scale3, centerErrorTol3, angleThreshold3, 150.0f, votingThreshold, seedDistance, minSeedDirection, extendingAngleThreshold);

	mainWin->glWidget->updateGL();
}

void ControlWidget::detectGridRadial() {
	int roadType = (ui.checkBoxRoadTypeAvenue->isChecked() ? 2 : 0) + (ui.checkBoxRoadTypeLocalStreet->isChecked() ? 1 : 0);

	int gridMaxIteration = ui.lineEditGridMaxIteration->text().toInt();
	int numBins = ui.lineEditNumBins->text().toInt();
	float minTotalLength = ui.lineEditMinTotalLength->text().toFloat();
	float minMaxBinRatio = ui.lineEditMinMaxBinRatio->text().toFloat();
	float gridAngleThreshold = ui.lineEditGridAngleThreshold->text().toFloat();
	float gridVotingThreshold = ui.lineEditGridVotingThreshold->text().toFloat();
	float gridExtendingDistanceThreshold = ui.lineEditGridExtendingDistanceThreshold->text().toFloat();
	float gridMinOBBLength = ui.lineEditGridMinOBBLength->text().toFloat();

	int radialMaxIteration = ui.lineEditRadialMaxIteration->text().toInt();
	float scale1 = ui.lineEditScale1->text().toFloat();
	float scale2 = ui.lineEditScale2->text().toFloat();
	float centerErrorTol2 = ui.lineEditCenterErrorTol2->text().toFloat();
	float angleThreshold2 = ui.lineEditAngleThreshold2->text().toFloat();
	float scale3 = ui.lineEditScale3->text().toFloat();
	float centerErrorTol3 = ui.lineEditCenterErrorTol3->text().toFloat();
	float angleThreshold3 = ui.lineEditAngleThreshold3->text().toFloat();
	float radialVotingThreshold = ui.lineEditRadialVotingThreshold->text().toFloat();
	float seedDistance = ui.lineEditRadialSeedDistance->text().toFloat();
	float minSeedDirection = ui.lineEditRadialMinSeedDirections->text().toFloat();
	float radialExtendingAngleThreshold = ui.lineEditRadialExtendingAngleThreshold->text().toFloat();


	GraphUtil::copyRoads(mainWin->glWidget->origRoads, mainWin->glWidget->roads);
	RoadSegmentationUtil::detectRadial(mainWin->glWidget->roads, mainWin->glWidget->selectedArea, roadType, mainWin->glWidget->radialFeatures, radialMaxIteration, scale1, scale2, centerErrorTol2, angleThreshold2, scale3, centerErrorTol3, angleThreshold3, 150.0f, radialVotingThreshold, seedDistance, minSeedDirection, radialExtendingAngleThreshold);
	RoadSegmentationUtil::detectGrid(mainWin->glWidget->roads, mainWin->glWidget->selectedArea, roadType, mainWin->glWidget->gridFeatures, gridMaxIteration, numBins, minTotalLength, minMaxBinRatio, gridAngleThreshold, gridVotingThreshold, gridExtendingDistanceThreshold, gridMinOBBLength);

	mainWin->glWidget->updateGL();
}

