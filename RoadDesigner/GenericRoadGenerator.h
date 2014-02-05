#pragma once

#include "RoadGraph.h"
#include "RoadArea.h"
#include "Polygon2D.h"
#include "GenericFeature.h"

class GenericRoadGenerator {
public:
	GenericRoadGenerator();
	~GenericRoadGenerator();

	void generateRoadNetwork(RoadArea& roadArea, const GenericFeature& gf);

private:
	void generateInitialSeeds(RoadGraph &roads, Polygon2D &area, std::list<RoadVertexDesc>& seeds);
	void attemptExpansion(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, std::list<RoadVertexDesc> &newSeeds);
	bool intersects(RoadGraph &roads, const QVector2D& p0, const QVector2D& p1, RoadEdgeDesc &eiClosest, QVector2D &closestIntPt);

	std::vector<float> generateRandomDirections(int num);
	std::vector<float> generateRandomDirections(int num, const QVector2D &dir);
	std::vector<float> generateRandomLengths(int num);
};

