#pragma once

#include <common/RoadGraph.h>
#include <common/Polygon2D.h>

class KDERoadGenerator {
public:
	KDERoadGenerator(void);
	~KDERoadGenerator(void);

	void generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const KDEFeature& kf);
};

