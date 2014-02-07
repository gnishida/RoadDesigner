#pragma once

#include "RoadGraph.h"
#include "RoadArea.h"
#include "Polygon2D.h"
#include "RadialFeature.h"

class RadialRoadGenerator {
public:
	RadialRoadGenerator();
	~RadialRoadGenerator();

	void generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const RadialFeature& rf);

private:
	void generateCircleAvenues(RoadGraph& roads, Polygon2D& area, const RadialFeature& rf, std::list<RoadVertexDesc>& seeds);
	void expandRadialAvenues(RoadGraph& roads, Polygon2D& area, const RadialFeature& rf, RoadVertexDesc desc);
};

