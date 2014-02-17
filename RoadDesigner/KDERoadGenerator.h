#pragma once

#include <common/RoadGraph.h>
#include <common/Polygon2D.h>
#include <road/feature/KDEFeature.h>

class KDERoadGenerator {
public:
	KDERoadGenerator() {}
	~KDERoadGenerator() {}

	void generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const KDEFeature& kf);

private:
	void generateAvenueSeeds(RoadGraph &roads, Polygon2D &area, const KDEFeature& kf, std::list<RoadVertexDesc>& seeds);
	void generateStreetSeeds(RoadGraph &roads, const KDEFeature& kf, std::list<RoadVertexDesc>& seeds);

	void attemptExpansion(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, int roadType, const KDEFeature& kf, std::list<RoadVertexDesc> &seeds);
	bool growRoadSegment(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, int roadType, const KDEFeature& kf, const Polyline2D &edge, std::list<RoadVertexDesc> &seeds);

	bool intersects(RoadGraph &roads, const QVector2D& p0, const QVector2D& p1, RoadEdgeDesc &eiClosest, QVector2D &closestIntPt);
	KDEFeatureItem getItem(const KDEFeature& kf, int roadType, const Polyline2D &edge);
	bool canSnap(RoadGraph& roads, const QVector2D& pos, float threshold, RoadVertexDesc srcDesc, RoadVertexDesc& snapDesc);

	void connectAvenues(RoadGraph &roads, float threshold);
};

