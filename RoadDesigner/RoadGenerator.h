#pragma once

#include "RoadGraph.h"
#include "RoadArea.h"
#include "Polygon2D.h"
#include "RandomFeature.h"

class GenericRoadGenerator {
public:
	GenericRoadGenerator();
	~GenericRoadGenerator();

	void generateRoadNetwork(RoadArea& roadArea, const GenericFeature& rf);

private:
};

