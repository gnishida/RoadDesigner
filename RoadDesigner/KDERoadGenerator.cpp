#include <common/Util.h>
#include <common/GraphUtil.h>
#include <common/TopNSearch.h>
#include "KDERoadGenerator.h"

void KDERoadGenerator::generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const KDEFeature& kf) {
	std::list<RoadVertexDesc> seeds;

	generateInitialSeeds(roads, area, kf, seeds);

	for (int i = 0; !seeds.empty() && i < 1000; ++i) {
		RoadVertexDesc desc = seeds.front();
		seeds.pop_front();

		attemptExpansion(roads, area, desc, 1, kf, seeds);
	}
}

/**
 * シード頂点を生成する。
 * エリアの中心に１つ頂点を生成し、それをシードとする。
 */
void KDERoadGenerator::generateInitialSeeds(RoadGraph &roads, Polygon2D &area, const KDEFeature& gf, std::list<RoadVertexDesc>& seeds) {
	seeds.clear();

	QVector2D center = area.centroid();

	RoadVertexPtr v = RoadVertexPtr(new RoadVertex(center));
	RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

	seeds.push_back(desc);
}

void KDERoadGenerator::attemptExpansion(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, int roadType, const KDEFeature& kf, std::list<RoadVertexDesc> &seeds) {
	KDEFeatureItem item;

	if (GraphUtil::getNumEdges(roads, srcDesc) == 0) {
		int id = Util::uniform_rand(0, kf.streetItems.size());
		item = kf.streetItems[id];
	} else {
		RoadOutEdgeIter ei, eend;
		boost::tie(ei, eend) = out_edges(srcDesc, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		QVector2D dir = roads.graph[tgt]->pt - roads.graph[srcDesc]->pt;

		item = getItem(kf, roadType, roads.graph[*ei]->polyLine);
	}
	
	for (int i = 0; i < item.edges.size(); ++i) {
		bool snapped = false;
		bool intersected = false;
		bool outside = false;

		RoadVertexDesc tgtDesc;

		Polyline2D polyline;
		for (int j = 0; j < item.edges[i].size(); ++j) {
			QVector2D pt = roads.graph[srcDesc]->pt + item.edges[i][j];

			// INTERSECTS -- If edge intersects other edge
			QVector2D intPoint;
			RoadEdgeDesc closestEdge;
			intersected = intersects(roads, roads.graph[srcDesc]->pt, pt, closestEdge, intPoint);
			if (intersected) {
				pt = intPoint;

				intersected = true;
			}

			// Densityをチェック
			float density = GraphUtil::getNumVertices(roads, pt, 50);
			if (density >= 1) continue;
			//float density = GraphUtil::getDensity(roads, pt, 50);
			//if (density >= kf._density / 400.0f) continue;

			float threshold;
			if (roadType == 1) {
				threshold = std::max(0.25f * (float)item.edges[i][j].length(), 10.0f);
			} else {
				threshold = (std::max)(0.25f * (float)item.edges[i][j].length(), 40.0f);
			}

			// 近くに頂点があるか？
			RoadVertexDesc desc;
			RoadEdgeDesc e_desc;
			if (GraphUtil::getVertex(roads, pt, threshold, srcDesc, desc)) {
				tgtDesc = desc;
				snapped = true;
				intersected = false;
			} else if (GraphUtil::getEdge(roads, pt, threshold, e_desc)) {
				tgtDesc = GraphUtil::splitEdge(roads, e_desc, pt);
				snapped = true;
				intersected = false;
			} else {
				if (!area.contains(pt)) {
					// エリア外周との交点を求める
					area.intersects(roads.graph[srcDesc]->pt, pt, pt);
					outside = true;
				}
			}

			if (intersected) {
				// 交差相手のエッジを分割
				tgtDesc = GraphUtil::splitEdge(roads, closestEdge, pt);
			}
			
			polyline.push_back(pt);

			if (!snapped && !intersected && !outside) break;
		}

		if (!snapped && !intersected) {
			// 頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(polyline[polyline.size() - 1]));
			tgtDesc = GraphUtil::addVertex(roads, v);
			
			if (outside) {
				roads.graph[tgtDesc]->onBoundary = true;
			}
		}

		// エッジを追加
		if (!GraphUtil::hasEdge(roads, srcDesc, tgtDesc)) {
			RoadEdgeDesc e = GraphUtil::addEdge(roads, srcDesc, tgtDesc, roadType, 1);
			roads.graph[e]->polyLine = polyline;
		}

		// シードに追加
		if (!snapped && !intersected && !outside && !item.deadends[i]) {
			seeds.push_back(tgtDesc);
		}
	}
}
/**
* Checks if new edge will intersect an existing edge
* This function DOES NOT modify the graph
**/
bool KDERoadGenerator::intersects(RoadGraph &roads, const QVector2D& p0, const QVector2D& p1, RoadEdgeDesc &eiClosest, QVector2D &closestIntPt) {
	float min_dist = std::numeric_limits<float>::max();
	bool intersect = false;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);

		//if new segment intersects other segment
		QVector2D intPt;
		float tab, tcd;
		if (Util::segmentSegmentIntersectXY(p0, p1, roads.graph[src]->pt, roads.graph[tgt]->pt, &tab, &tcd, true, intPt)) {
			float dist = (p0 - intPt).lengthSquared();

			//make sure we get only closest segment
			if (dist < min_dist) {
				min_dist = dist;
				eiClosest = *ei;
				closestIntPt = intPt;
				intersect = true;
			}
		}
	}	

	return intersect;
}

/**
 * 与えられたエッジの方向、長さを含むデータを検索し、近いものを返却する。
 */
KDEFeatureItem KDERoadGenerator::getItem(const KDEFeature& kf, int roadType, const Polyline2D &edge) {
	TopNSearch<int> tns;
	int id;
	QList<int> topn;

	switch (roadType) {
	case RoadEdge::TYPE_AVENUE:
		for (int i = 0; i < kf.avenueItems.size(); ++i) {
			float dist = kf.avenueItems[i].getMinDistance(edge);
			tns.add(dist, i);
		}
		topn = tns.topN(3, TopNSearch<int>::ORDER_DESC);
		id = Util::uniform_rand(0, topn.size());

		return kf.avenueItems[id];
	case RoadEdge::TYPE_STREET:
		for (int i = 0; i < kf.streetItems.size(); ++i) {
			float dist = kf.streetItems[i].getMinDistance(edge);
			tns.add(dist, i);
		}
		topn = tns.topN(3, TopNSearch<int>::ORDER_DESC);
		id = Util::uniform_rand(0, topn.size());

		return kf.avenueItems[id];
	}

	assert(false);
}