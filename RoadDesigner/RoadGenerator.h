#pragma once

#include <common/Polygon2D.h>
#include <common/RoadGraph.h>
#include <road/feature/RoadFeature.h>
#include "RoadArea.h"

class RoadGenerator {
public:
	RoadGenerator() {}
	~RoadGenerator() {}

	void generateRoadNetwork(RoadArea& roadArea, const RoadFeature& rf);

private:
};

