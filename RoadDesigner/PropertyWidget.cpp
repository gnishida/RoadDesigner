#include <road/GraphUtil.h>
#include "PropertyWidget.h"
#include "MainWindow.h"
#include "GLWidget.h"

PropertyWidget::PropertyWidget(MainWindow* mainWin) : QDockWidget("Property Widget", (QWidget*)mainWin) {
	this->mainWin = mainWin;

	// set up the UI
	ui.setupUi(this);

	// register the event handlers
	hide();
}

void PropertyWidget::setRoadVertex(RoadGraph &roads, RoadVertexDesc vertexDesc, RoadVertexPtr selectedVertex) {
	QString desc("");
	QString location("");
	QString parent("");
	QString uncles("");
	QString neighbors("");
	QString kernel("");

	desc.setNum(vertexDesc);

	location = QString("(%1, %2)").arg(roads.graph[vertexDesc]->pt.x(), 0, 'f', 0).arg(roads.graph[vertexDesc]->pt.y(), 0, 'f', 0);

	if (roads.graph[vertexDesc]->properties.contains("parent")) {
		parent.setNum(roads.graph[vertexDesc]->properties["parent"].toInt());
	}

	if (roads.graph[vertexDesc]->properties.contains("uncles")) {
		uncles = roads.graph[vertexDesc]->properties["uncles"].toString();
	}

	std::vector<RoadVertexDesc> n = GraphUtil::getNeighbors(roads, vertexDesc);
	for (int i = 0; i < n.size(); i++) {
		QString str;
		str.setNum(n[i]);

		neighbors += str;
		if (i < n.size() - 1) neighbors += ",";
	}

	kernel.setNum(selectedVertex->kernel.id);

	ui.lineEditVertexDesc->setText(desc);
	ui.lineEditVertexPos->setText(location);
	ui.lineEditVertexParent->setText(parent);
	ui.textEditVertexUncles->setText(uncles);
	ui.textEditVertexNeighbors->setText(neighbors);
	ui.lineEditKernel->setText(kernel);
}

/**
 * Display the selected edge information.
 */
void PropertyWidget::setRoadEdge(RoadEdgePtr selectedEdge) {
	QString type;
	QString numLanes;
	QString oneWay;
	QString link;
	QString roundabout;

	if (selectedEdge != NULL) {
		switch (selectedEdge->type) {
		case RoadEdge::TYPE_HIGHWAY:
			type = "Highway";
			break;
		case RoadEdge::TYPE_BOULEVARD:
			type = "Boulevard";
			break;
		case RoadEdge::TYPE_AVENUE:
			type = "Avenue";
			break;
		case RoadEdge::TYPE_STREET:
			type = "Street";
			break;
		default:
			type = "";
			break;
		}

		numLanes.setNum(selectedEdge->lanes);

		oneWay = selectedEdge->oneWay ? "Yes" : "No";
		link = selectedEdge->link ? "Yes" : "No";
		roundabout = selectedEdge->roundabout ? "Yes" : "No";
	}

	ui.lineEditEdgeType->setText(type);
	ui.lineEditEdgeLanes->setText(numLanes);
	ui.lineEditEdgeOneWay->setText(oneWay);
	ui.lineEditEdgeLink->setText(link);
	ui.lineEditEdgeRoundabout->setText(roundabout);
}

void PropertyWidget::resetRoadVertex() {
	ui.lineEditVertexDesc->setText("");
	ui.lineEditVertexPos->setText("");
	ui.lineEditVertexParent->setText("");
	ui.textEditVertexUncles->setText("");
	ui.textEditVertexNeighbors->setText("");
	ui.lineEditKernel->setText("");
}

void PropertyWidget::resetRoadEdge() {
	ui.lineEditEdgeType->setText("");
	ui.lineEditEdgeLanes->setText("");
	ui.lineEditEdgeOneWay->setText("");
	ui.lineEditEdgeLink->setText("");
	ui.lineEditEdgeRoundabout->setText("");
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

