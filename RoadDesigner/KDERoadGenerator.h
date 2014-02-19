#pragma once

#include <common/RoadGraph.h>
#include <common/Polygon2D.h>
#include <road/feature/KDEFeature.h>

class KDERoadGenerator {
public:
	KDERoadGenerator() {}
	~KDERoadGenerator() {}

	void generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const KDEFeature& kf, int numIterations, bool isGenerateLocalStreets);

private:
	void generateBoulevard(RoadGraph &roads, Polygon2D &area);
	void generateAvenueSeeds(RoadGraph &roads, Polygon2D &area, const KDEFeature& kf, std::list<RoadVertexDesc>& seeds);
	void generateStreetSeeds(RoadGraph &roads, const KDEFeature& kf, std::list<RoadVertexDesc>& seeds);

	void attemptExpansion(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, int roadType, const KDEFeature& kf, std::list<RoadVertexDesc> &seeds);
	bool growRoadSegment(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, int roadType, const KDEFeature& kf, const KDEFeatureItemEdge &edge, std::list<RoadVertexDesc> &seeds);

	bool intersects(RoadGraph &roads, const QVector2D& p0, const QVector2D& p1, RoadEdgeDesc &eiClosest, QVector2D &closestIntPt);

	KDEFeatureItem getItem(const KDEFeature& kf, int roadType, const QVector2D &pt, const Polyline2D &edge);
	KDEFeatureItem getItem(const KDEFeature& kf, int roadType, const QVector2D &pt);

	bool canSnapToVertex(RoadGraph& roads, const QVector2D& pos, float threshold, RoadVertexDesc srcDesc, RoadVertexDesc& snapDesc);
	bool canSnapToEdge(RoadGraph& roads, const QVector2D& pos, float threshold, RoadVertexDesc srcDesc, RoadEdgeDesc& snapEdge, QVector2D &closestPt);


	float getNearestVertex(RoadGraph& roads, const QVector2D& pos, RoadVertexDesc srcDesc, RoadVertexDesc& snapDesc);
	float getNearestEdge(RoadGraph& roads, const QVector2D& pt, RoadVertexDesc srcDesc, RoadEdgeDesc& snapEdge, QVector2D &closestPt);
	void connectAvenues(RoadGraph &roads, float threshold);
};

