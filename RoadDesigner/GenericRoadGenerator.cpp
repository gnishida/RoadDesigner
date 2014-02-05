﻿#include "GenericRoadGenerator.h"
#include "Util.h"
#include "GraphUtil.h"

GenericRoadGenerator::GenericRoadGenerator() {
}

GenericRoadGenerator::~GenericRoadGenerator() {
}

void GenericRoadGenerator::generateRoadNetwork(RoadArea& roadArea, const GenericFeature& gf) {
	std::list<RoadVertexDesc> initSeeds;
	std::list<RoadVertexDesc> newSeeds;

	generateInitialSeeds(roadArea.roads, roadArea.area, initSeeds);

	int iteCount = 0;

	//======= grow arterials
	while (!initSeeds.empty() && iteCount < 1000) {
		RoadVertexDesc tmpSeedDesc = initSeeds.front();
		initSeeds.pop_front();

		attemptExpansion(roadArea.roads, roadArea.area, tmpSeedDesc, newSeeds);

		//append seeds in newSeeds to initSeeds
		initSeeds.splice(initSeeds.end(), newSeeds);

		iteCount++;
	}

	//Remove dead ends
	//removeDeadEnds(inRoadGraph);

	//==============================================
	//======= grow streets
	//==============================================
	/*
	std::list<roadGraphVertexDesc> initStreetSeeds;

	if(!generateInitialStreetSeeds(urbanGeometry, initStreetSeeds, inRoadGraph)){
		return false;
	}

	iteCount = 0;
	while (!initStreetSeeds.empty() && iteCount < 1000) {
		tmpSeedDesc = initStreetSeeds.front();

		bool isPointWithinLimits = false;

		if(urbanGeometry->containsPoint(inRoadGraph->getRoadGraph()[tmpSeedDesc].getPt())){
			isPointWithinLimits = true;
		}

		if(!isPointWithinLimits){
			newSeedsCreated = false;
		} else {							
			float refAng = 0.0f;
			attemptExpansion(tmpSeedDesc, inRoadGraph, newSeeds, refAng);
			newSeedsCreated = !(newSeeds.empty());	
		}

		if(newSeedsCreated){
			initStreetSeeds.splice(initStreetSeeds.end(), newSeeds);
		}

		//remove seed from initSeeds
		initStreetSeeds.pop_front();

		iteCount++;
	}

	//Remove dead ends
	removeDeadEnds(inRoadGraph);
	*/
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
	roads.graph[desc]->lengths = generateRandomLengths(4);

	seeds.push_back(desc);
}

/**
* Attempts expansion of a segment in all possible directions and adds new edges to roadGraph.
* If new seeds are found, they are added to newSeeds.
* This function DOES modify the graph
**/
void GenericRoadGenerator::attemptExpansion(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, std::list<RoadVertexDesc> &newSeeds) {
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
			continue;
		}

		float threshold = (std::max)(0.25f * dist, 40.0f);

		// 近くに頂点があるか？
		RoadVertexDesc desc;
		RoadEdgeDesc e_desc;
		if (GraphUtil::getVertex(roads, pt, threshold, desc)) {
			tgtDesc = desc;
			snapped = true;
		} else if (GraphUtil::getEdge(roads, pt, threshold, e_desc)) {
			tgtDesc = GraphUtil::splitEdge(roads, e_desc, pt);
			snapped = true;
		} else {
			if (!area.contains(pt)) {
				// エリア外周との交点を求める
				float tab, tcd;
				QVector2D intPoint;
				area.intersect(roads.graph[srcDesc]->pt, pt, &tab, &tcd, pt);
				outside = true;
			}

			// 頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
			tgtDesc = GraphUtil::addVertex(roads, v);

			int numDirections = Util::uniform_rand() * 2 + 3;
			roads.graph[tgtDesc]->angles = generateRandomDirections(numDirections - 1, roads.graph[srcDesc]->pt - roads.graph[tgtDesc]->pt);
			roads.graph[tgtDesc]->lengths = generateRandomLengths(numDirections - 1);
		}

		// エッジを追加
		RoadEdgeDesc e = GraphUtil::addEdge(roads, srcDesc, tgtDesc, 2, 1);

		// シードに追加
		if (!snapped && !intersected && !outside) {
			newSeeds.push_back(tgtDesc);
		}




		/*
		if (!area.contains(pt)) {
			// エリア外周との交点を求める
			float tab, tcd;
			QVector2D intPoint;
			area.intersect(roads.graph[srcDesc]->pt, pt, &tab, &tcd, intPoint);

			// 外周上に頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(intPoint));
			tgtDesc = GraphUtil::addVertex(roads, v);

			roads.graph[tgtDesc]->angles = generateRandomDirections(3, roads.graph[srcDesc]->pt - roads.graph[tgtDesc]->pt);
			roads.graph[tgtDesc]->lengths = generateRandomLengths(3);

			// エッジを追加
			RoadEdgeDesc e = GraphUtil::addEdge(roads, srcDesc, tgtDesc, 2, 1);

			continue;
		}

		//the threshold should be the max between e.g. 1/4 of the length and e.g. 10m
		//float threshold = 10.0f;

		if (!intersected) {
			RoadVertexDesc desc;
			if (GraphUtil::getVertex(roads, pt, threshold, desc)) {
				tgtDesc = desc;
				snapped = true;
			}
		} else {
			RoadVertexDesc src = boost::source(closestEdge, roads.graph);
			RoadVertexDesc tgt = boost::target(closestEdge, roads.graph);

			float distToSrc = (roads.graph[src]->pt - pt).lengthSquared();
			float distToTgt = (roads.graph[tgt]->pt - pt).lengthSquared();

			if (distToSrc < threshold || distToTgt < threshold) {
				if(distToSrc < distToTgt){
					pt = roads.graph[src]->pt;
					tgtDesc = src;
				} else {
					pt = roads.graph[tgt]->pt;
					tgtDesc = tgt;
				}
				snapped = true;
			}			
		}

		// ANGLE REDUNDANCY -- if departing segment is redundant
		if (isSegmentRedundant(targetVtxPt, inRoadGraph->getRoadGraph()[srcVertexDesc].getPt(), srcVertexDesc, inRoadGraph, 0.2f*M_PI)) {
			continue;
		}
		if (snapped) {
			if (isSegmentRedundant(inRoadGraph->getRoadGraph()[srcVertexDesc].getPt(), targetVtxPt, tgtVertexDesc, inRoadGraph, 0.2f * M_PI)) {
				continue;
			}
		}

		if (intersected && !snap) {
			// 頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
			tgtDesc = GraphUtil::addVertex(roads, v);

			// エッジを分割
			// ...
		}



		if (!snap && !intersected) {
			// 頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
			tgtDesc = GraphUtil::addVertex(roads, v);

			int numDirections = Util::uniform_rand() * 2 + 3;
			roads.graph[tgtDesc]->angles = generateRandomDirections(numDirections - 1, roads.graph[srcDesc]->pt - roads.graph[tgtDesc]->pt);
			roads.graph[tgtDesc]->lengths = generateRandomLengths(numDirections - 1);
		}

		// エッジを追加
		RoadEdgeDesc e = GraphUtil::addEdge(roads, srcDesc, tgtDesc, 2, 1);

		if (!snap && !intersected) { //if not snap, vertex must be initialized
			//add target vertex to list of seeds
			newSeeds.push_back(tgtDesc);
		}
		*/
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
 * 指定された数のエッジ長を生成する。
 */
std::vector<float> GenericRoadGenerator::generateRandomLengths(int num) {
	std::vector<float> lengths;

	for (int i = 0; i < num; ++i) {
		float length = Util::uniform_rand(100.0f, 200.0f);

		lengths.push_back(length);
	}

	return lengths;

}