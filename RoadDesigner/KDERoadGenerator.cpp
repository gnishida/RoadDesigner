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

void KDERoadGenerator::generateRoadNetwork(RoadGraph &roads, Polygon2D &area, const KDEFeature& kf, int numIterations, bool isGenerateLocalStreets) {
	srand(12345);

	std::list<RoadVertexDesc> seeds;

	// Boulevardを生成
	generateBoulevard(roads, area);

	while (true) {
		// Avenueを生成
		generateAvenueSeeds(roads, area, kf, seeds);

		if (seeds.empty()) break;

		for (int i = 0; !seeds.empty() && i < numIterations; ++i) {
			RoadVertexDesc desc = seeds.front();
			seeds.pop_front();

			std::cout << "attemptExpansion (avenue): " << i << " (Seed: " << desc << ")" << std::endl;
			//if (i == 17) break;
			attemptExpansion(roads, area, desc, RoadEdge::TYPE_AVENUE, kf, seeds);
		}

		// clean up
		GraphUtil::clean(roads);
		GraphUtil::reduce(roads);
		GraphUtil::clean(roads);

		break;
	}

	if (!isGenerateLocalStreets) return;

	// Avenueをできる限りつなぐ
	//connectAvenues(roads, 400.0f);

	// Local streetを生成
	generateStreetSeeds(roads, kf, seeds);

	for (int i = 0; !seeds.empty() && i < numIterations; ++i) {
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
void KDERoadGenerator::generateAvenueSeeds(RoadGraph &roads, Polygon2D &area, const KDEFeature& f, std::list<RoadVertexDesc>& seeds) {
	seeds.clear();

	QVector2D center = area.centroid();
	
	float numExpectedVertices = f.density(RoadEdge::TYPE_AVENUE) * area.area();
	int numSeeds = numExpectedVertices / 30 + 1;
	float threshold = area.area() / numExpectedVertices;

	std::cout << "Area: " << area.area() << std::endl;
	std::cout << "Expected num vertices: " << numExpectedVertices << std::endl;
	std::cout << "Num seeds: " << numSeeds << std::endl;

	QSet<int> usedKernels;
	for (int i = 0; i < f.area().size() - 1; ++i) {
		addAvenueSeed(roads, area, f, (f.area()[i] - center) * 0.5f, usedKernels, seeds);
	}
}

void KDERoadGenerator::addAvenueSeed(RoadGraph &roads, const Polygon2D &area, const KDEFeature &f, const QVector2D &offset, QSet<int> &usedKernels, std::list<RoadVertexDesc>& seeds) {
	QVector2D center = area.centroid();

	float min_dist = std::numeric_limits<float>::max();
	int min_index = -1;
	for (int i = 0; i < f.items(RoadEdge::TYPE_AVENUE).size(); ++i) {
		float dist = (f.items(RoadEdge::TYPE_AVENUE)[i].pt - offset).lengthSquared();
		if (dist < min_dist) {
			min_dist = dist;
			min_index = i;
		}
	}

	if (usedKernels.contains(min_index)) return;

	usedKernels.insert(min_index);

	RoadVertexPtr v = RoadVertexPtr(new RoadVertex(f.items(RoadEdge::TYPE_AVENUE)[min_index].pt + center));
	RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

	// このシード頂点のカーネルを決定する
	roads.graph[desc]->kernel = getItem(f, RoadEdge::TYPE_AVENUE, roads.graph[desc]->pt - center);

	seeds.push_back(desc);
}

/**
 * Local street用のシードを生成する。
 * Avenueが既に生成済みであることを前提とする。Avenueにより生成されるFaceを抽出し、その中心をシードとする。
 */
void KDERoadGenerator::generateStreetSeeds(RoadGraph &roads, const KDEFeature& f, std::list<RoadVertexDesc>& seeds) {
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

void KDERoadGenerator::attemptExpansion(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, int roadType, const KDEFeature& f, std::list<RoadVertexDesc> &seeds) {
	QVector2D center = area.centroid();

	KDEFeatureItem item = roads.graph[srcDesc]->kernel;
	
	for (int i = 0; i < item.edges.size(); ++i) {
		growRoadSegment(roads, area, srcDesc, roadType, f, item.edges[i], seeds);
	}
}

/**
 * 指定されたpolylineに従って、srcDesc頂点からエッジを伸ばす。
 */
bool KDERoadGenerator::growRoadSegment(RoadGraph &roads, Polygon2D &area, RoadVertexDesc &srcDesc, int roadType, const KDEFeature& f, const KDEFeatureItemEdge &edge, std::list<RoadVertexDesc> &seeds) {
	RoadVertexDesc tgtDesc;
	RoadVertexDesc snapDesc;

	bool snapped = false;
	bool intersected = false;
	bool outside = false;

	Polyline2D polyline;
	polyline.push_back(roads.graph[srcDesc]->pt);

	QVector2D pt;
	for (int j = 0; j < edge.edge.size(); ++j) {
		pt = roads.graph[srcDesc]->pt + edge.edge[j];

		// INTERSECTS -- If edge intersects other edge
		QVector2D intPoint;
		RoadEdgeDesc closestEdge;
		intersected = intersects(roads, roads.graph[srcDesc]->pt, pt, closestEdge, intPoint);
		if (intersected) {
			RoadVertexDesc src = boost::source(closestEdge, roads.graph);
			RoadVertexDesc tgt = boost::target(closestEdge, roads.graph);

			// 自分のエッジに交差した場合は、このエッジのgrowをキャンセル
			if (src == srcDesc || tgt == srcDesc) return false;

			pt = intPoint;

			intersected = true;
		}

		// Densityをチェック
		if (roadType == RoadEdge::TYPE_STREET) {
			float density = GraphUtil::getNumVertices(roads, pt, 50);
			if (density >= (f.density(roadType) + f.density(RoadEdge::TYPE_AVENUE)) * 50.0f * 50.0f * M_PI) return false;
		} else {
			//float density = GraphUtil::getNumVertices(roads, pt, 400);
			//if (density >= f.density(roadType) * 400.0f * 400.0f * M_PI) return false;
		}

		float threshold;
		if (roadType == RoadEdge::TYPE_STREET || j < edge.edge.size() - 1) {
			//threshold = std::max(0.25f * (float)edge[j].length(), 10.0f);
			threshold = std::min(0.25f * (float)edge.edge[j].length(), 10.0f);
		} else {
			//threshold = (std::max)(0.25f * (float)edge[j].length(), 40.0f);
			threshold = std::min(0.25f * (float)edge.edge[j].length(), 40.0f);
		}

		// 近くに頂点があるか？
		RoadVertexDesc desc;
		RoadEdgeDesc e_desc;
		QVector2D closestPt;		
		if (canSnapToVertex(roads, pt, threshold, srcDesc, desc)) {
			snapDesc = desc;
			snapped = true;
			intersected = false;
		} else if (canSnapToEdge(roads, pt, threshold, srcDesc, e_desc, closestPt)) {
			// 実験。既存のエッジを分割させないよう、キャンセルさせてみる
			if (roads.graph[e_desc]->type == roadType) {
				return false;
			}

			snapDesc = GraphUtil::splitEdge(roads, e_desc, pt);
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
		//GraphUtil::addEdge(roads, tgtDesc, snapDesc, roadType, 1);
		GraphUtil::snapVertex(roads, tgtDesc, snapDesc);
	}

	// シードに追加
	if (!snapped && !intersected && !outside && !edge.deadend) {
		seeds.push_back(tgtDesc);

		// 追加した頂点に、カーネルを割り当てる
		QVector2D offsetPos = roads.graph[srcDesc]->kernel.pt + roads.graph[tgtDesc]->pt - roads.graph[srcDesc]->pt;
		std::reverse(polyline.begin(), polyline.end());
		roads.graph[tgtDesc]->kernel = getItem(f, roadType, offsetPos, polyline);
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
		if (!roads.graph[*ei]->valid) continue;

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
	float location_weight = 1.0f;

	float min_diff = std::numeric_limits<float>::max();
	int min_index = -1;
	for (int i = 0; i < kf.items(roadType).size(); ++i) {
		// エッジのフィッティング度を計算
		float fitting_diff = kf.items(roadType)[i].getMinDistance(existingEdge);

		float location_diff = 0.0f;

		// 位置が元のエリア内なら、位置のフィッティング度を計算
		if (kf.area().contains(offsetPosOfVertex)) {
			location_diff = (kf.items(roadType)[i].pt - offsetPosOfVertex).length();
		}

		// フィッティングスコアを計算
		float diff = fitting_diff * fitting_weight + location_diff * location_weight;
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
		float angle = Util::diffAngle(item.edges[i].edge[0], existingEdge[1] - existingEdge[0]);
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

/**
 * 近くの頂点にsnapすべきか、チェックする。
 * srcDescから伸ばしてきたエッジの先端posに近い頂点を探し、エッジの延長方向とのなす角度が90度以下で、距離が閾値以下のものがあるか探す。
 * 
 * @param pos				エッジ先端
 * @param threshold			距離の閾値
 * @param srcDesc			この頂点からエッジを延ばしている
 * @param snapDesc			最も近い頂点
 * @return					もしsnapすべき頂点があれば、trueを返却する
 */
bool KDERoadGenerator::canSnapToVertex(RoadGraph& roads, const QVector2D& pos, float threshold, RoadVertexDesc srcDesc, RoadVertexDesc& snapDesc) {
	float min_dist = std::numeric_limits<float>::max();

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);

		if (src == srcDesc || tgt == srcDesc) continue;

		if (QVector2D::dotProduct(roads.graph[src]->pt - roads.graph[srcDesc]->pt, pos - roads.graph[srcDesc]->pt) > 0) {
			float dist1 = (roads.graph[src]->pt - pos).lengthSquared();
			if (dist1 < min_dist) {
				min_dist = dist1;
				snapDesc = src;
			}
		}

		if (QVector2D::dotProduct(roads.graph[tgt]->pt - roads.graph[srcDesc]->pt, pos - roads.graph[srcDesc]->pt) > 0) {
			float dist2 = (roads.graph[tgt]->pt - pos).lengthSquared();
			if (dist2 < min_dist) {
				min_dist = dist2;
				snapDesc = tgt;
			}
		}
	}

	if (min_dist <= threshold * threshold) return true;
	else return false;
}

/**
 * 近くのエッジにsnapすべきか、チェックする。
 * srcDescから伸ばしてきたエッジの先端posに近いエッジを探し、エッジの延長方向とのなす角度が90度以下で、距離が閾値以下のものがあるか探す。
 * 
 * @param pos				エッジ先端
 * @param threshold			距離の閾値
 * @param srcDesc			この頂点からエッジを延ばしている
 * @param snapDesc			最も近い頂点
 * @return					もしsnapすべき頂点があれば、trueを返却する
 */
bool KDERoadGenerator::canSnapToEdge(RoadGraph& roads, const QVector2D& pos, float threshold, RoadVertexDesc srcDesc, RoadEdgeDesc& snapEdge, QVector2D &closestPt) {
	float min_dist = std::numeric_limits<float>::max();

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		QVector2D closePt;
		float dist = GraphUtil::distance(roads, pos, *ei, closePt);

		if (QVector2D::dotProduct(closePt - roads.graph[srcDesc]->pt, pos - roads.graph[srcDesc]->pt) > 0) {
			if (dist < min_dist) {
				min_dist = dist;
				snapEdge = *ei;
				closestPt = closePt;
			}
		}
	}		

	if (min_dist < threshold) return true;
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
		QVector2D closestPt;
		float min_dist_e = std::numeric_limits<float>::max();
		RoadEdgeDesc min_e_desc;
		RoadEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
			if (!roads.graph[*ei]->valid) continue;

			RoadVertexDesc src = boost::source(*ei, roads.graph);
			RoadVertexDesc tgt = boost::target(*ei, roads.graph);
			if (src == deadendVertices[i] || tgt == deadendVertices[i]) continue;

			QVector2D closePt;
			float dist = GraphUtil::distance(roads, roads.graph[deadendVertices[i]]->pt, *ei, closePt);
			if (dist < min_dist_e) {
				min_dist_e = dist;
				min_e_desc = *ei;
				closestPt = closePt;
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
