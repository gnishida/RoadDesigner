#include "GenericRoadGenerator.h"
#include "Util.h"
#include "GraphUtil.h"

GenericRoadGenerator::GenericRoadGenerator() {
}

GenericRoadGenerator::~GenericRoadGenerator() {
}

void GenericRoadGenerator::generateRoadNetwork(RoadArea& roadArea, const GenericFeature& gf) {
	std::list<RoadVertexDesc> seeds;
	std::list<RoadVertexDesc> newSeeds;

	generateInitialSeeds(roadArea.roads, roadArea.area, seeds);

	int iteCount = 0;

	//======= grow arterials
	while (!seeds.empty() && iteCount < 1000) {
		RoadVertexDesc desc = seeds.front();
		seeds.pop_front();

		attemptExpansion(roadArea.roads, roadArea.area, desc, newSeeds, 100.0f, 300.0f);

		//append seeds in newSeeds to seeds
		seeds.splice(seeds.end(), newSeeds);

		iteCount++;
	}

	//Remove dead ends
	removeDeadEnds(roadArea.roads);

	//======= grow streets
	if (!generateInitialStreetSeeds(roadArea.roads, seeds)) {
		return;
	}

	iteCount = 0;
	while (!seeds.empty() && iteCount < 1000) {
		RoadVertexDesc desc = seeds.front();

		attemptExpansion(roadArea.roads, roadArea.area, desc, newSeeds, 20.0f, 50.0f);

		//append seeds in newSeeds to seeds
		seeds.splice(seeds.end(), newSeeds);

		iteCount++;
	}

	//Remove dead ends
	removeDeadEnds(roadArea.roads);
}

/**
 * Make a center as an initial seed, and add it as a vertex of the road graph.
 *
 * @param urbanGeometry
 * @param seeds
 * @param inRoadGraph
 */
void GenericRoadGenerator::generateInitialSeeds(RoadGraph &roads, Polygon2D &area, std::list<RoadVertexDesc>& seeds) {
	seeds.clear();

	BBox bbox = area.getLoopAABB();

	RoadVertexPtr v = RoadVertexPtr(new RoadVertex(bbox.midPt()));
	RoadVertexDesc desc = GraphUtil::addVertex(roads, v);
	roads.graph[desc]->angles = generateRandomDirections(4);
	roads.graph[desc]->lengths = generateRandomLengths(4, 100.0f, 300.0f);

	seeds.push_back(desc);
}

/**
 * ローカルストリートのシードを生成する。
 */
bool GenericRoadGenerator::generateInitialStreetSeeds(RoadGraph &roads, std::list<RoadVertexDesc>& seeds) {
	std::vector<RoadEdgeDesc> edges;

	// 対象となるエッジをリストアップ
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		// Avenueエッジのみを対象
		if (roads.graph[*ei]->type == 2) {
			edges.push_back(*ei);
		}
	}

	for (int i = 0; i < edges.size(); ++i) {
		float step = Util::uniform_rand(20.0f, 50.0f);
		float remained_step = step;

		for (int i = 0; i < roads.graph[edges[i]]->polyLine.size() - 1; ++i) {
			QVector2D dir = roads.graph[edges[i]]->polyLine[i + 1] - roads.graph[edges[i]]->polyLine[i];
			float length = dir.length();
			float remained_length = length;
			while (remained_length > remained_step) {
				remained_length -= remained_step;
				remained_step = step;

				QVector2D pt = roads.graph[edges[i]]->polyLine[i + 1] - dir * remained_length / length;
				RoadVertexDesc desc = GraphUtil::splitEdge(roads, edges[i], pt);

				// 頂点の情報をセット
				roads.graph[desc]->angles = generatePerpendicularDirections(dir);
				roads.graph[desc]->lengths = generateRandomLengths(2, 20.0f, 50.0f);

				// シードに追加
				seeds.push_back(desc);
			}

			remained_step -= remained_length;
		}
	}

	return true;
}



/**
* Attempts expansion of a segment in all possible directions and adds new edges to roadGraph.
* If new seeds are found, they are added to newSeeds.
* This function DOES modify the graph
**/
void GenericRoadGenerator::attemptExpansion(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, std::list<RoadVertexDesc> &newSeeds, float l1, float l2) {
	newSeeds.clear();

	for (int i = 0; i < roads.graph[srcDesc]->angles.size(); ++i) {
		float theta = roads.graph[srcDesc]->angles[i];
		QVector2D dir(cosf(theta), sinf(theta));

		bool snapped = false;
		bool intersected = false;
		bool outside = false;

		float dist = roads.graph[srcDesc]->lengths[i];
		dist = dist + Util::uniform_rand(dist * -0.3f, dist * 0.3f);

		RoadVertexDesc tgtDesc;
		QVector2D pt = roads.graph[srcDesc]->pt + dir * dist;

		// INTERSECTS -- If edge intersects other edge
		QVector2D intPoint;
		RoadEdgeDesc closestEdge;
		intersected = intersects(roads, roads.graph[srcDesc]->pt, pt, closestEdge, intPoint);
		if (intersected) {
			pt = intPoint;

			intersected = true;
		}

		float threshold = (std::max)(0.25f * dist, 40.0f);

		// 近くに頂点があるか？
		RoadVertexDesc desc;
		RoadEdgeDesc e_desc;
		if (GraphUtil::getVertex(roads, pt, threshold, desc)) {
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
				float tab, tcd;
				QVector2D intPoint;
				area.intersect(roads.graph[srcDesc]->pt, pt, &tab, &tcd, pt);
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

				int numDirections = Util::uniform_rand() * 2 + 3;
				roads.graph[tgtDesc]->angles = generateRandomDirections(numDirections - 1, roads.graph[srcDesc]->pt - roads.graph[tgtDesc]->pt);
				roads.graph[tgtDesc]->lengths = generateRandomLengths(numDirections - 1, l1, l2);
			}
		}

		// エッジを追加
		RoadEdgeDesc e = GraphUtil::addEdge(roads, srcDesc, tgtDesc, 2, 1);

		// シードに追加
		if (!snapped && !intersected && !outside) {
			newSeeds.push_back(tgtDesc);
		}
	}
}

/**
* Checks if new edge will intersect an existing edge
* This function DOES NOT modify the graph
**/
bool GenericRoadGenerator::intersects(RoadGraph &roads, const QVector2D& p0, const QVector2D& p1, RoadEdgeDesc &eiClosest, QVector2D &closestIntPt) {
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
 * 指定された数のエッジ方向を生成する。ただし、dirで指定された方向には既にエッジがあるので、この方向は避ける。
 */
std::vector<float> GenericRoadGenerator::generateRandomDirections(int num) {
	std::vector<float> angles;

	float angle = Util::uniform_rand(0, M_PI * 2.0f);
	float angle_step = M_PI * 2.0f / num;

	for (int i = 0; i < num; ++i) {
		angles.push_back(angle + Util::uniform_rand(-0.2f, 0.2f));

		angle += angle_step;		
	}

	return angles;
}

/**
 * 指定された数のエッジ方向を生成する。ただし、dirで指定された方向には既にエッジがあるので、この方向は避ける。
 */
std::vector<float> GenericRoadGenerator::generateRandomDirections(int num, const QVector2D &dir) {
	std::vector<float> angles;

	float angle = atan2f(dir.y(), dir.x());
	float angle_step = M_PI * 2.0f / (num + 1);

	for (int i = 0; i < num; ++i) {
		angle += angle_step;		

		angles.push_back(angle + Util::uniform_rand(-0.2f, 0.2f));
	}

	return angles;
}

/**
 * 指定された方向に垂直な２方向を生成する。
 */
std::vector<float> GenericRoadGenerator::generatePerpendicularDirections(const QVector2D &dir) {
	std::vector<float> angles;

	float angle = atan2f(dir.x(), -dir.y());
	float angle_step = M_PI;

	for (int i = 0; i < 2; ++i) {
		angles.push_back(angle + Util::uniform_rand(-0.2f, 0.2f));

		angle += angle_step;		
	}

	return angles;
}

/**
 * 指定された数のエッジ長を生成する。
 */
std::vector<float> GenericRoadGenerator::generateRandomLengths(int num, float l1, float l2) {
	std::vector<float> lengths;

	for (int i = 0; i < num; ++i) {
		float length = Util::uniform_rand(100.0f, 300.0f);

		lengths.push_back(length);
	}

	return lengths;

}

/**
 * Dead-endのエッジを削除する。
 * ただし、境界にあるエッジは対象外とする。
 */
void GenericRoadGenerator::removeDeadEnds(RoadGraph &roads) {
	bool removedOne = true;
	while (removedOne) {
		removedOne = false;
		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
			if (!roads.graph[*vi]->valid) continue;
			if (roads.graph[*vi]->onBoundary) continue;

			if (GraphUtil::getDegree(roads, *vi) == 1) {
				// invalidate all the outing edges.
				RoadOutEdgeIter ei, eend;
				for (boost::tie(ei, eend) = boost::out_edges(*vi, roads.graph); ei != eend; ++ei) {
					roads.graph[*ei]->valid = false;
				}

				// invalidate the vertex as well.
				roads.graph[*vi]->valid = false;

				removedOne = true;
			}
		}
	}
}