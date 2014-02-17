#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <common/Util.h>
#include <common/GraphUtil.h>
#include <common/TopNSearch.h>
#include "KDERoadGenerator.h"

RoadGraph* roadGraphPtr;
Polygon2D face;
std::vector<Polygon2D*>* facesPtr;

//Vertex visitor
struct faceVisitor : public boost::planar_face_traversal_visitor {
	void begin_face() {
		face.clear();
	}

	void end_face() {
		face.push_back(face[0]);
		facesPtr->push_back(new Polygon2D(face));
	}

	template <typename Vertex> 
	void next_vertex(Vertex v) {
		face.push_back(roadGraphPtr->graph[v]->pt);
	}

	template <typename Edge> 
	void next_edge(Edge e) {
	}
};

void KDERoadGenerator::generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const KDEFeature& kf) {
	srand(12345);

	std::list<RoadVertexDesc> seeds;

	// Boulevardを生成
	generateBoulevard(roads, area);

	while (true) {
		// Avenueを生成
		generateAvenueSeeds(roads, area, kf, seeds);

		if (seeds.empty()) break;

		for (int i = 0; !seeds.empty() && i < 1000; ++i) {
			RoadVertexDesc desc = seeds.front();
			seeds.pop_front();

			std::cout << "attemptExpansion (avenue): " << i << std::endl;
			if (i == 4) break;
			attemptExpansion(roads, area, desc, RoadEdge::TYPE_AVENUE, kf, seeds);
		}

		// clean up
		GraphUtil::clean(roads);
		GraphUtil::reduce(roads);
		GraphUtil::clean(roads);

		break;
	}

	return;

	// Avenueをできる限りつなぐ
	//connectAvenues(roads, 400.0f);

	// Local streetを生成
	generateStreetSeeds(roads, kf, seeds);

	for (int i = 0; !seeds.empty() && i < 1000; ++i) {
		RoadVertexDesc desc = seeds.front();
		seeds.pop_front();

		std::cout << "attemptExpansion (street): " << i << std::endl;
		//if (i == 3) break;
		attemptExpansion(roads, area, desc, RoadEdge::TYPE_STREET, kf, seeds);
	}
}

/**
 * Areaの境界上に、Boulevardを生成する。
 */
void KDERoadGenerator::generateBoulevard(RoadGraph &roads, Polygon2D &area) {
	RoadVertexDesc prevDesc;

	for (int i = 0; i < area.size(); ++i) {
		RoadVertexDesc desc;
		if (!GraphUtil::getVertex(roads, area[i], 0.1f, desc)) {
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(area[i]));
			desc = GraphUtil::addVertex(roads, v);
		}

		if (i > 0) {
			GraphUtil::addEdge(roads, prevDesc, desc, RoadEdge::TYPE_BOULEVARD, 1);
		}

		prevDesc = desc;
	}
}

/**
 * シード頂点を生成する。
 * 密度に応じて、エリア内にランダムにシードを生成する。
 */
void KDERoadGenerator::generateAvenueSeeds(RoadGraph &roads, Polygon2D &area, const KDEFeature& gf, std::list<RoadVertexDesc>& seeds) {
	seeds.clear();

	BBox bbox = area.envelope();

	std::vector<QVector2D> seedsPoints;
	
	float numExpectedVertices = gf.density(RoadEdge::TYPE_AVENUE) * area.area();
	int numSeeds = numExpectedVertices / 30 + 1;
	float threshold = area.area() / numExpectedVertices;

	std::cout << "Area: " << area.area() << std::endl;
	std::cout << "Expected num vertices: " << numExpectedVertices << std::endl;
	std::cout << "Num seeds: " << numSeeds << std::endl;

	TopNSearch<QVector2D> tns;
	/*
	for (int i = 0; i < 1000; ++i) {
		QVector2D pt;
		bool retry = true;
		while (retry) {
			retry = false;

			if (bbox.dx() > 400) {
				pt.setX(bbox.minPt.x() + 200 + Util::uniform_rand(0, bbox.dx() - 400));
			} else {
				pt.setX(bbox.midPt().x());
			}
			if (bbox.dy() > 400) {
				pt.setY(bbox.minPt.y() + 200 + Util::uniform_rand(0, bbox.dy() - 400));
			} else {
				pt.setY(bbox.midPt().y());
			}
			if (!area.contains(pt)) {
				retry = true;
			}
		}

		float density = GraphUtil::getNumVertices(roads, pt, 400);
		if (density < gf.density(RoadEdge::TYPE_AVENUE) * 400 * 400 * M_PI / 3) {
			tns.add(density, pt);
		}
	}
	*/
	float min_dist = std::numeric_limits<float>::max();
	int min_index = -1;
	for (int i = 0; i < gf.items(RoadEdge::TYPE_AVENUE).size(); ++i) {
		float dist = gf.items(RoadEdge::TYPE_AVENUE)[i].pt.lengthSquared();
		if (dist < min_dist) {
			min_dist = dist;
			min_index = i;
		}
	}

	RoadVertexPtr v = RoadVertexPtr(new RoadVertex(gf.items(RoadEdge::TYPE_AVENUE)[min_index].pt));
	RoadVertexDesc desc = GraphUtil::addVertex(roads, v);
	seeds.push_back(desc);

	/*
	if (tns.size() > 0) {
		QList<QVector2D> result = tns.topN(1, TopNSearch<QVector2D>::ORDER_ASC);
		
		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(result[0]));
		//v->seed = true;
		RoadVertexDesc desc = GraphUtil::addVertex(roads, v);
		seeds.push_back(desc);
	}
	*/
}

/**
 * Local street用のシードを生成する。
 * Avenueが既に生成済みであることを前提とする。Avenueにより生成されるFaceを抽出し、その中心をシードとする。
 */
void KDERoadGenerator::generateStreetSeeds(RoadGraph &roads, const KDEFeature& kf, std::list<RoadVertexDesc>& seeds) {
	std::vector<Polygon2D*> faces;

	roadGraphPtr = &roads;
	facesPtr = &faces;

	bool isPlanar = false;
	bool converges = true;

	//Make sure graph is planar
	typedef std::vector<RoadEdgeDesc > tEdgeDescriptorVector;
	std::vector<tEdgeDescriptorVector> embedding(boost::num_vertices(roads.graph));

	int cont=0;
	while (!isPlanar && converges) {	
		if (cont>2) {
			std::cerr << "ERROR : Graph is not planar." << std::endl;
			return;
		}
		// Test for planarity		
		if (boost::boyer_myrvold_planarity_test(boost::boyer_myrvold_params::graph = roads.graph, boost::boyer_myrvold_params::embedding = &embedding[0])) {
			isPlanar = true;
		} else {	
			//Remove intersecting edges
			//if (!removeIntersectingEdges(roadGraph)) {
			//	converges = false;
			//}
		}
		cont++;
	}

	if (!isPlanar) {
		std::cout << "ERROR: Graph could not be planarized (generateBlocks)\n";
		return;
	}

	//Create edge index property map?	
	typedef std::map<RoadEdgeDesc, size_t> EdgeIndexMap;
	EdgeIndexMap mapEdgeIdx;
	boost::associative_property_map<EdgeIndexMap> pmEdgeIndex(mapEdgeIdx);		
	RoadEdgeIter ei, ei_end;
	int edge_count = 0;
	for (boost::tie(ei, ei_end) = boost::edges(roads.graph); ei != ei_end; ++ei) {
		mapEdgeIdx.insert(std::make_pair(*ei, edge_count++));	
	}

	// Extract blocks from road graph using boost graph planar_face_traversal
	faceVisitor v_vis;	
	boost::planar_face_traversal(roads.graph, &embedding[0], v_vis, pmEdgeIndex);

	// Remove the outmost boundary
	float max_area = 0.0f;
	int max_face_index = -1;
	for (int i = 0; i < faces.size(); ++i) {
		if (faces[i]->area() > max_area) {
			max_area = faces[i]->area();
			max_face_index = i;
		}
	}
	if (max_face_index != -1) {
		faces.erase(faces.begin() + max_face_index);
	}

	std::cout << "Faces: " << faces.size() << std::endl;

	for (int i = 0; i < faces.size(); ++i) {
		//if (faces[i]->area() < 100.0f) continue;

		QVector2D center = faces[i]->centroid();
		if (!faces[i]->contains(center)) continue;

		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(center));
		//v->seed = true;
		RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

		seeds.push_back(desc);
	}
}

void KDERoadGenerator::attemptExpansion(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, int roadType, const KDEFeature& kf, std::list<RoadVertexDesc> &seeds) {
	QVector2D center = area.centroid();

	KDEFeatureItem item;

	if (GraphUtil::getNumEdges(roads, srcDesc) == 0) {
		item = getItem(kf, roadType, roads.graph[srcDesc]->pt - center);
		//int id = Util::uniform_rand(0, kf.items(roadType).size());
		//item = kf.items(roadType)[id];
	} else {
		RoadOutEdgeIter ei, eend;
		boost::tie(ei, eend) = out_edges(srcDesc, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		Polyline2D polyline = roads.graph[*ei]->polyLine;
		if ((polyline[0] - roads.graph[srcDesc]->pt).lengthSquared() > (polyline[0] - roads.graph[tgt]->pt).lengthSquared()) {
			std::reverse(polyline.begin(), polyline.end());
		}
		item = getItem(kf, roadType, roads.graph[srcDesc]->pt - center, polyline);
	}
	
	for (int i = 0; i < item.edges.size(); ++i) {
		growRoadSegment(roads, area, srcDesc, roadType, kf, item.edges[i], seeds);
	}
}

/**
 * 指定されたpolylineに従って、srcDesc頂点からエッジを伸ばす。
 */
bool KDERoadGenerator::growRoadSegment(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, int roadType, const KDEFeature& kf, const Polyline2D &edge, std::list<RoadVertexDesc> &seeds) {
	RoadVertexDesc tgtDesc;
	RoadVertexDesc snapDesc;

	bool snapped = false;
	bool intersected = false;
	bool outside = false;

	Polyline2D polyline;
	polyline.push_back(roads.graph[srcDesc]->pt);

	QVector2D pt;
	for (int j = 0; j < edge.size(); ++j) {
		pt = roads.graph[srcDesc]->pt + edge[j];

		// INTERSECTS -- If edge intersects other edge
		QVector2D intPoint;
		RoadEdgeDesc closestEdge;
		intersected = intersects(roads, roads.graph[srcDesc]->pt, pt, closestEdge, intPoint);
		if (intersected) {
			pt = intPoint;

			intersected = true;
		}

		// Densityをチェック
		if (roadType == RoadEdge::TYPE_STREET) {
			float density = GraphUtil::getNumVertices(roads, pt, 50);
			if (density >= (kf.density(roadType) + kf.density(RoadEdge::TYPE_AVENUE)) * 50.0f * 50.0f * M_PI) return false;
		} else {
			//float density = GraphUtil::getNumVertices(roads, pt, 400);
			//if (density >= kf.density(roadType) * 400.0f * 400.0f * M_PI) return false;
		}

		float threshold;
		if (roadType == RoadEdge::TYPE_STREET || j < edge.size() - 1) {
			//threshold = std::max(0.25f * (float)edge[j].length(), 10.0f);
			threshold = 10.0f;
		} else {
			//threshold = (std::max)(0.25f * (float)edge[j].length(), 40.0f);
			threshold = 40.0f;
		}

		// 近くに頂点があるか？
		RoadVertexDesc desc;
		RoadEdgeDesc e_desc;

		/*
		QVector2D closestPt;

		float min_dist_v = getNearestVertex(roads, pt, srcDesc, desc);
		float min_dist_e = getNearestEdge(roads, pt, srcDesc, e_desc, closestPt);
		if (min_dist_v < threshold) {
			snapped = true;
			snapDesc = desc;
			intersected = false;
		} else if (min_dist_e < (float)edge[j].length() * 0.25f) {
			snapped = true;
			snapDesc = GraphUtil::splitEdge(roads, e_desc, closestPt);
			intersected = false;
		} else {
			if (!area.contains(pt)) {
				// エリア外周との交点を求める
				area.intersects(roads.graph[srcDesc]->pt, pt, pt);
				outside = true;
			}
		}
		*/
		
		if (canSnap(roads, pt, threshold, srcDesc, desc)) {
			snapDesc = desc;
			//pt = roads.graph[tgtDesc]->pt;
			snapped = true;
			intersected = false;
		} else if (GraphUtil::getEdge(roads, pt, srcDesc, threshold, e_desc)) {
			snapDesc = GraphUtil::splitEdge(roads, e_desc, pt);
			//pt = roads.graph[tgtDesc]->pt;
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

		if (snapped || intersected || outside) break;
	}

	if (!intersected) {
		// 頂点を追加
		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
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

	if (snapped) {
		//GraphUtil::addEdge(roads, srcDesc, snapDesc, roadType, 1);
		GraphUtil::snapVertex(roads, tgtDesc, snapDesc);
	}

	// シードに追加
	if (!snapped && !intersected && !outside) {// && !item.deadends[i]) {
		seeds.push_back(tgtDesc);
	}

	return true;
}

/**
* Checks if new edge will intersect an existing edge
**/
bool KDERoadGenerator::intersects(RoadGraph &roads, const QVector2D& p0, const QVector2D& p1, RoadEdgeDesc &eiClosest, QVector2D &closestIntPt) {
	float min_dist = std::numeric_limits<float>::max();
	bool intersected = false;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {

		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; ++i) {
			//if new segment intersects other segment
			QVector2D intPt;
			float tab, tcd;
			if (Util::segmentSegmentIntersectXY(p0, p1, roads.graph[*ei]->polyLine[i], roads.graph[*ei]->polyLine[i + 1], &tab, &tcd, true, intPt)) {
				float dist = (p0 - intPt).lengthSquared();

				//make sure we get only closest segment
				if (dist < min_dist) {
					min_dist = dist;
					eiClosest = *ei;
					closestIntPt = intPt;
					intersected = true;
				}
			}

		}
	}	

	return intersected;
}

/**
 * 与えられたエッジの方向、長さを含むデータを検索し、近いものを返却する。
 *
 * @param kf					特徴量
 * @param roadType				道路タイプ
 * @param offsetPosOfVertex		与えられた頂点の、このエリアの中心からのオフセット位置
 * @param edge					与えられた頂点に既に登録済みのエッジのpolyline（頂点から出る方向に、並べ替え済み）
 */
KDEFeatureItem KDERoadGenerator::getItem(const KDEFeature& kf, int roadType, const QVector2D &offsetPosOfVertex, const Polyline2D &existingEdge) {
	float fitting_weight = 1.0f;
	float distribution_weight = 1.0f;

	float min_diff = std::numeric_limits<float>::max();
	int min_index = -1;
	for (int i = 0; i < kf.items(roadType).size(); ++i) {
		// エッジのフィッティング度を計算
		float fitting_diff = kf.items(roadType)[i].getMinDistance(existingEdge);

		// 位置のフィッティング度を計算
		float distributoin_diff = (kf.items(roadType)[i].pt - offsetPosOfVertex).length();

		// フィッティングスコアを計算
		float diff = fitting_diff * fitting_weight + distributoin_diff * distribution_weight;
		if (diff < min_diff) {
			min_diff = diff;
			min_index = i;
		}
	}

	KDEFeatureItem item = kf.items(roadType)[min_index];

	// 与えられたエッジの方向に近いエッジを削除する
	float min_angle = std::numeric_limits<float>::max();
	int min_edge_index = -1;
	for (int i = 0; i < item.edges.size(); ++i) {
		float angle = Util::diffAngle(item.edges[i][0], existingEdge[1] - existingEdge[0]);
		if (angle < min_angle) {
			min_angle = angle;
			min_edge_index = i;
		}
	}
	if (min_angle < 0.6f) {
		item.edges.erase(item.edges.begin() + min_edge_index);
	}

	return item;
}

/**
 * 与えられたエッジの方向、長さを含むデータを検索し、近いものを返却する。
 *
 * @param kf					特徴量
 * @param roadType				道路タイプ
 * @param offsetPosOfVertex		与えられた頂点の、このエリアの中心からのオフセット位置
 */
KDEFeatureItem KDERoadGenerator::getItem(const KDEFeature& kf, int roadType, const QVector2D &offsetPosOfVertex) {
	float min_diff = std::numeric_limits<float>::max();
	int min_index = -1;
	for (int i = 0; i < kf.items(roadType).size(); ++i) {
		// 位置のフィッティング度を計算
		float diff = (kf.items(roadType)[i].pt - offsetPosOfVertex).length();

		if (diff < min_diff) {
			min_diff = diff;
			min_index = i;
		}
	}

	KDEFeatureItem item = kf.items(roadType)[min_index];

	return item;
}

bool KDERoadGenerator::canSnap(RoadGraph& roads, const QVector2D& pos, float threshold, RoadVertexDesc srcDesc, RoadVertexDesc& snapDesc) {
	float min_dist = std::numeric_limits<float>::max();

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);

		if (src == srcDesc || tgt == srcDesc) continue;

		float dist1 = (roads.graph[src]->pt - pos).lengthSquared();
		float dist2 = (roads.graph[tgt]->pt - pos).lengthSquared();
		if (dist1 < min_dist) {
			min_dist = dist1;
			snapDesc = src;
		}
		if (dist2 < min_dist) {
			min_dist = dist2;
			snapDesc = tgt;
		}
	}

	if (min_dist <= threshold * threshold) return true;
	else return false;
}

float KDERoadGenerator::getNearestVertex(RoadGraph& roads, const QVector2D& pos, RoadVertexDesc srcDesc, RoadVertexDesc& snapDesc) {
	float min_dist = std::numeric_limits<float>::max();

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);

		if (src == srcDesc || tgt == srcDesc) continue;

		float dist1 = (roads.graph[src]->pt - pos).lengthSquared();
		float dist2 = (roads.graph[tgt]->pt - pos).lengthSquared();
		if (dist1 < min_dist) {
			min_dist = dist1;
			snapDesc = src;
		}
		if (dist2 < min_dist) {
			min_dist = dist2;
			snapDesc = tgt;
		}
	}

	return min_dist;
}

float KDERoadGenerator::getNearestEdge(RoadGraph& roads, const QVector2D& pt, RoadVertexDesc srcDesc, RoadEdgeDesc& snapEdge, QVector2D &closestPt) {
	float min_dist = std::numeric_limits<float>::max();
	RoadEdgeDesc min_e;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);

		if (!roads.graph[src]->valid) continue;
		if (!roads.graph[tgt]->valid) continue;

		if (src == srcDesc || tgt == srcDesc) continue;

		QVector2D pt2;
		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
			float dist = Util::pointSegmentDistanceXY(roads.graph[*ei]->polyLine[i], roads.graph[*ei]->polyLine[i + 1], pt, pt2);
			if (dist < min_dist) {
				min_dist = dist;
				snapEdge = *ei;
				closestPt = pt2;
			}
		}
	}

	return min_dist;
}

/**
 * degree=1の頂点について、近くの頂点とできる限りつなぐ。
 */
void KDERoadGenerator::connectAvenues(RoadGraph &roads, float threshold) {
	float threshold2 = threshold * threshold;

	std::vector<RoadVertexDesc> deadendVertices;

	// degree = 1の頂点リストを取得
	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
		if (!roads.graph[*vi]->valid) continue;
		
		// 境界の頂点は、除外する
		if (roads.graph[*vi]->onBoundary) continue;

		if (GraphUtil::getDegree(roads, *vi) == 1) {
			deadendVertices.push_back(*vi);
		}
	}

	// degree=1の各頂点について、最も近い頂点またはエッジを探す
	for (int i = 0; i < deadendVertices.size(); ++i) {
		// 最も近い頂点を探す
		float min_dist_v = std::numeric_limits<float>::max();
		RoadVertexDesc min_v_desc;
		RoadVertexIter vi, vend;
		for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
			if (!roads.graph[*vi]->valid) continue;
			if (*vi == deadendVertices[i]) continue;

			float dist = (roads.graph[*vi]->pt - roads.graph[deadendVertices[i]]->pt).lengthSquared();
			if (dist < min_dist_v) {
				min_dist_v = dist;
				min_v_desc = *vi;
			}
		}

		// 最も近いエッジを探す
		float min_dist_e = std::numeric_limits<float>::max();
		RoadEdgeDesc min_e_desc;
		RoadEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
			if (!roads.graph[*ei]->valid) continue;

			RoadVertexDesc src = boost::source(*ei, roads.graph);
			RoadVertexDesc tgt = boost::target(*ei, roads.graph);
			if (src == deadendVertices[i] || tgt == deadendVertices[i]) continue;

			float dist = GraphUtil::distance(roads, roads.graph[deadendVertices[i]]->pt, *ei);
			if (dist < min_dist_e) {
				min_dist_e = dist;
				min_e_desc = *ei;
			}
		}

		if (min_dist_v < threshold2) {
			//GraphUtil::snapVertex(roads, deadendVertices[i], min_v_desc);
			GraphUtil::addEdge(roads, deadendVertices[i], min_v_desc, RoadEdge::TYPE_AVENUE, 1);
		} else {
			RoadVertexDesc desc = GraphUtil::splitEdge(roads, min_e_desc, roads.graph[deadendVertices[i]]->pt);

			//GraphUtil::snapVertex(roads, deadendVertices[i], desc);
			GraphUtil::addEdge(roads, deadendVertices[i], desc, RoadEdge::TYPE_AVENUE, 1);
		}
	}
}
