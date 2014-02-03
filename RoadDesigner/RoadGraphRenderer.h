#pragma once

#include "RoadGraph.h"
#include "Renderable.h"
#include "AbstractArea.h"
#include "Polygon2D.h"

class RoadGraphRenderer {
public:
	unsigned int dispList;

public:
	RoadGraphRenderer();

	void render(std::vector<RenderablePtr>& renderables);
	void renderOne(RenderablePtr renderable);

	void renderArea(const Polygon2D& area, GLenum lineType, float height);
	void renderDenseArea(const AbstractArea& area, float height);
	void renderPoint(const QVector2D& pt, const QColor& color, float height);
	void renderPolyline(const Polygon2D& polyline, GLenum lineType, float height);

	//void tessellate(const Loop2D& polygon);
	void renderConcave(const Polygon2D& polygon, const QColor& color, float height);
};

