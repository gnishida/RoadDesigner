#pragma once

#include "RoadBox.h"
#include "ui_RoadBoxList.h"
#include <QVBoxLayout>

class MyMainWindow;

class RoadBoxList : public QWidget {
Q_OBJECT

public:
	Ui::RoadBoxList ui;
	MyMainWindow* mainWin;
	std::vector<RoadBox*> references;
	std::vector<RoadBox*> largeReferences;
	std::vector<RoadBox*> smallReferences;
	//std::vector<RoadGraph*> largeRoads;
	//std::vector<RoadGraph*> smallRoads;

public:
	RoadBoxList(MyMainWindow* mainWin, std::vector<QString> filelist, float roadSizeroadSize);
	~RoadBoxList();

public slots:

};

