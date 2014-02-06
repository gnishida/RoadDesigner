#include "RoadGenerator.h"
#include "Util.h"
#include "GraphUtil.h"
#include "GridRoadGenerator.h"
#include "RadialRoadGenerator.h"
#include "GenericRoadGenerator.h"

RoadGenerator::RoadGenerator() {
}

RoadGenerator::~RoadGenerator() {
}

/**
 * 道路網を生成する
 * 暫定的な実装。要改善。
 */
void RoadGenerator::generateRoadNetwork(RoadArea& roadArea, const RoadFeature& rf) {
	if (rf.gridFeatures.size() > 0) {
		GridRoadGenerator grg;
		grg.generateRoadNetwork(roadArea, rf.gridFeatures[0]);
	} else if (rf.radialFeatures.size() > 0) {
		RadialRoadGenerator rrg;
		rrg.generateRoadNetwork(roadArea, rf.radialFeatures[0]);
	} else if (rf.genericFeatures.size() > 0) {
		GenericRoadGenerator grg;
		grg.generateRoadNetwork(roadArea, rf.genericFeatures[0]);
	}
}

