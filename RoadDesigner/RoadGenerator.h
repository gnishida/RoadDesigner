#pragma once

#include "RoadGraph.h"
#include "RoadArea.h"
#include "Polygon2D.h"
#include "GridFeature.h"
#include "RadialFeature.h"

class RoadGenerator {
public:
	RoadGenerator();
	~RoadGenerator();

	void generateRoadNetwork(RoadArea& roadArea, const GridFeature& gf);
	void generateRoadNetwork(RoadArea& roadArea, const RadialFeature& gf);

private:
	void generateHorizontalAvenueSeeds(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds);
	std::list<RoadVertexDesc> generateHorizontalAvenues(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds, int dir, float length);
	void expandHorizontalAvenue(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, RoadVertexDesc start_desc, int direction, std::list<RoadVertexDesc>& seeds);

	void generateCircleAvenues(RoadGraph& roads, const Polygon2D& area, const RadialFeature& rf, std::list<RoadVertexDesc>& seeds);
	void expandRadialAvenues(RoadGraph& roads, const Polygon2D& area, const RadialFeature& rf, RoadVertexDesc desc);
};
