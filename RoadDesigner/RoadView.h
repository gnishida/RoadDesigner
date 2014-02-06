#pragma once

#include "RoadGraph.h"
#include <qgraphicsview.h>

class MyMainWindow;

class RoadView : public QGraphicsView {
public:
	MyMainWindow* mainWin;
	float size;
	QGraphicsScene* scene;
	RoadGraph* roads;

	QVector2D offset;

public:
	RoadView(MyMainWindow* parent, float size);
	~RoadView();

	void load(const char* filename);
	float showSimilarity(RoadGraph* roads, float sketchCanvasSize, bool zoomedIn);
	void updateView(RoadGraph* roads, bool showPairness = false);
};

