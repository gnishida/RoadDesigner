#pragma once

#include "RoadGraph.h"
#include "Polygon2D.h"
#include "GridFeature.h"

class RoadGenerator {
public:
	RoadGenerator();
	~RoadGenerator();

	void generateRoadNetwork(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf);

private:
	void generateHorizontalAvenues(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds);
	std::list<RoadVertexDesc> expandHorizontalAvenues(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds, int dir, float length);
};

