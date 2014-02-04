#pragma once

#include "RoadGraph.h"
#include "Polygon2D.h"
#include "GridFeature.h"
#include "RadialFeature.h"

class RoadGenerator {
public:
	RoadGenerator();
	~RoadGenerator();

	void generateRoadNetwork(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf);
	void generateRoadNetwork(RoadGraph& roads, const Polygon2D& area, const RadialFeature& gf);

private:
	void generateHorizontalAvenues(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds);
	std::list<RoadVertexDesc> expandHorizontalAvenues(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds, int dir, float length);

	void generateCircleAvenues(RoadGraph& roads, const Polygon2D& area, const RadialFeature& rf, std::list<RoadVertexDesc>& seeds);
	void expandRadialAvenues(RoadGraph& roads, const Polygon2D& area, const RadialFeature& rf, RoadVertexDesc desc);
};

