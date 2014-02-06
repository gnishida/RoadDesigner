#pragma once

#include "RoadGraph.h"
#include "RoadArea.h"
#include "Polygon2D.h"
#include "RoadFeature.h"

class RoadGenerator {
public:
	RoadGenerator();
	~RoadGenerator();

	void generateRoadNetwork(RoadArea& roadArea, const RoadFeature& rf);

private:
};

