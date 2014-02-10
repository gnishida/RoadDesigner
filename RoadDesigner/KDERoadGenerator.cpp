#include <common/Util.h>
#include <common/GraphUtil.h>
#include "KDERoadGenerator.h"

void KDERoadGenerator::generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const KDEFeature& kf) {
	std::list<RoadVertexDesc> seeds;

	generateInitialSeeds(roads, area, kf, seeds);

	for (int i = 0; !seeds.empty() && i < 1000; ++i) {
		RoadVertexDesc desc = seeds.front();
		seeds.pop_front();

		attemptExpansion(roads, area, desc, 2, kf, seeds);
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
		int id = Util::uniform_rand(0, kf.items.size());
		item = kf.items[id];
	} else {
		RoadOutEdgeIter ei, eend;
		boost::tie(ei, eend) = out_edges(srcDesc, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		QVector2D dir = roads.graph[tgt]->pt - roads.graph[srcDesc]->pt;

		item = kf.getItem(dir);
	}
	
	for (int i = 0; i < item.edges.size(); ++i) {
		bool snapped = false;
		bool intersected = false;
		bool outside = false;

		RoadVertexDesc tgtDesc;
		QVector2D pt = roads.graph[srcDesc]->pt + item.edges[i];

		// INTERSECTS -- If edge intersects other edge
		QVector2D intPoint;
		RoadEdgeDesc closestEdge;
		intersected = intersects(roads, roads.graph[srcDesc]->pt, pt, closestEdge, intPoint);
		if (intersected) {
			pt = intPoint;

			intersected = true;
		}

		float threshold;
		if (roadType == 1) {
			threshold = std::max(0.25f * (float)item.edges[i].length(), 10.0f);
		} else {
			threshold = (std::max)(0.25f * (float)item.edges[i].length(), 40.0f);
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

			if (intersected) {
				// 交差相手のエッジを分割
				tgtDesc = GraphUtil::splitEdge(roads, closestEdge, pt);
			} else {
				// 頂点を追加
				RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
				tgtDesc = GraphUtil::addVertex(roads, v);
			
				if (outside) {
					roads.graph[tgtDesc]->onBoundary = true;
				}
			}
		}

		// エッジを追加
		if (!GraphUtil::hasEdge(roads, srcDesc, tgtDesc)) {
			GraphUtil::addEdge(roads, srcDesc, tgtDesc, roadType, 1);
		}

		// シードに追加
		if (!snapped && !intersected && !outside) {
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
