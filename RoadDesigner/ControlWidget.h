#pragma once

#include <QDockWidget>
#include "ui_ControlWidget.h"
#include <road/RoadGraph.h>

class MainWindow;
class GLWidget;
class RoadGraphEditor;

class ControlWidget : public QDockWidget {
Q_OBJECT

private:
	Ui::ControlWidget ui;
	MainWindow* mainWin;

public:
	ControlWidget(MainWindow* mainWin);

	void setRoadVertex(RoadVertexDesc vertexDesc, RoadVertexPtr selectedVertex);
	void setRoadEdge(RoadEdgePtr selectedEdge);

public slots:
	void showRoad(int flag);
	void generateKDE();
	void clear();
	void connectRoads();
	void removeDeadends();
};

