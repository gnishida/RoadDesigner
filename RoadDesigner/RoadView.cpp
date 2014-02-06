#include "RoadView.h"
#include "MyMainWindow.h"
#include "GraphUtil.h"
#include "BFSTree.h"
#include <qmap.h>
#include <QLineF>
#include <QGraphicsSimpleTextItem>

RoadView::RoadView(MyMainWindow* mainWin, float size) : QGraphicsView((QWidget*)mainWin) {
	this->mainWin = mainWin;
	this->size = size;

	// set up the graphics view
	this->setSceneRect(0, 0, size, size);
	this->setFixedSize(220, 220);
	this->scale(200.0f / size, 200.0f / size);
	this->setBackgroundBrush(QColor(233, 229, 220));
	this->setRenderHint(QPainter::Antialiasing);

	// set up the scene
	scene = new QGraphicsScene();
	this->setScene(scene);	

	roads = NULL;
	offset = QVector2D(0, 0);
}

RoadView::~RoadView() {
}

void RoadView::load(const char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (roads != NULL) {
		delete roads;
	}
	roads = new RoadGraph();
	roads->load(fp, 7);
	fclose(fp);

	updateView(roads);
}

/**
 * Compute the similarity between this road and the sketch (roads2).
 */
float RoadView::showSimilarity(RoadGraph* roads2, float sketchCanvasSize, bool zoomedIn) {
	RoadGraph* r1 = GraphUtil::copyRoads(roads);
	RoadGraph* r2 = GraphUtil::copyRoads(roads2);

	// Compute the importance of each edge
	//GraphUtil::computeImportanceOfEdges(r1, 1.0f, 1.0f, 1.0f);
	//GraphUtil::computeImportanceOfEdges(r2, 1.0f, 1.0f, 1.0f);

	// Find the central vertex in the sketch
	RoadVertexDesc root2 = GraphUtil::getCentralVertex(r2);

	// Define the central vertex in this road graph
	RoadVertexDesc root1;
	if (zoomedIn) {
		root1 = GraphUtil::getCentralVertex(r1);

		// update the offset
		offset = r2->graph[root2]->pt - r1->graph[root1]->pt;
	} else {
		float scale = size / sketchCanvasSize;

		QVector2D center = r2->graph[root2]->pt * scale;
		root1 = GraphUtil::getVertex(r1, center);

		// update the offset
		offset = center - r1->graph[root1]->pt;
	}

	// Create a tree
	BFSTree tree1(r1, root1);
	BFSTree tree2(r2, root2);

	// Find the matching
	QMap<RoadVertexDesc, RoadVertexDesc> map1;
	QMap<RoadVertexDesc, RoadVertexDesc> map2;
	GraphUtil::findCorrespondence(r1, &tree1, r2, &tree2, false, 0.75f, map1, map2);

	// Update the view based on the matching
	updateView(r1, true);

	// Compute the similarity
	float similarity = GraphUtil::computeSimilarity(r1, map1, r2, map2, 1.0f, 5.0f);
	QString str;
	str.setNum(similarity);
	//score->setText(str);
	QGraphicsSimpleTextItem* score = scene->addSimpleText(str, QFont("Times", size * 0.1f));
	score->setPen(QPen(Qt::blue));
	score->setPos(0, 0);

	// Delete the temporal roads
	delete r1;
	delete r2;

	update();

	return similarity;
}

/**
 * Update the view based on the road graph with matching infromation.
 * If the edge has a corresponding one, color it with red. Otherwise, color itt with black.
 */
void RoadView::updateView(RoadGraph* roads, bool showPairness) {
	scene->clear();

	QPen pen(QColor(0, 0, 255));

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads->graph); ei != eend; ++ei) {
		if (!roads->graph[*ei]->valid) continue;
		//if (roads->graph[*ei]->type != 2) continue;

		for (int i = 0; i < roads->graph[*ei]->polyLine.size() - 1; i++) {
			QLineF line(roads->graph[*ei]->polyLine[i].x(), -roads->graph[*ei]->polyLine[i].y(), roads->graph[*ei]->polyLine[i+1].x(), -roads->graph[*ei]->polyLine[i+1].y());
			line.translate(size / 2.0f, size / 2.0f);
			QGraphicsLineItem* item = scene->addLine(line, pen);

			if (showPairness && !roads->graph[*ei]->fullyPaired) {
				item->setOpacity(0.1);
			}
		}
	}

	// Draw the square for the central vertex
	//RoadVertexDesc v1 = GraphUtil::getCentralVertex(roads);
	//scene->addRect(roads->graph[v1]->pt.x() + 4900, roads->graph[v1]->pt.y() + 4900, 200, 200, QPen(Qt::blue));

	scene->update();
}
