#pragma once

#include "RoadGraph.h"
#include "RoadArea.h"
#include "Polygon2D.h"
#include "GridFeature.h"

class GridRoadGenerator {
public:
	GridRoadGenerator();
	~GridRoadGenerator();

	void generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const GridFeature& gf);

private:
	void generateHorizontalAvenueSeeds(RoadGraph& roads, Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds);
	void expandHorizontalAvenueSeeds(RoadGraph& roads, Polygon2D& area, const GridFeature& gf, RoadVertexDesc start_desc, int direction, std::list<RoadVertexDesc>& seeds);
	std::list<RoadVertexDesc> generateHorizontalAvenues(RoadGraph& roads, Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds, int dir, float length);
	void expandHorizontalAvenue(RoadGraph& roads, Polygon2D& area, const GridFeature& gf, RoadVertexDesc start_desc, int direction, std::list<RoadVertexDesc>& seeds);
};

