#pragma once

#include <common/Polygon2D.h>
#include <common/RoadGraph.h>
#include <road/feature/RadialFeature.h>

class RadialRoadGenerator {
public:
	RadialRoadGenerator();
	~RadialRoadGenerator();

	void generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const RadialFeature& rf);

private:
	void generateCircleAvenues(RoadGraph& roads, Polygon2D& area, const RadialFeature& rf, std::list<RoadVertexDesc>& seeds);
	void expandRadialAvenues(RoadGraph& roads, Polygon2D& area, const RadialFeature& rf, RoadVertexDesc desc);
};

