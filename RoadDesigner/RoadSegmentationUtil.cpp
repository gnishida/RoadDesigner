#include "RoadSegmentationUtil.h"
#include "GraphUtil.h"
#include "Util.h"
#include "BBox.h"
#include "ConvexHull.h"
#include "HoughTransform.h"
#include <math.h>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#define SQR(x)	((x) * (x))

RoadGraph* roadGraphPtr;
std::vector<RoadEdgeDesc> plaza;
int numOutingEdges;
float plazaLength;
std::vector<std::vector<RoadEdgeDesc> > plaza_list;

//Vertex visitor
struct faceVisitorForPlazaDetection : public boost::planar_face_traversal_visitor {
	void begin_face() {
		plaza.clear();
		numOutingEdges = 0;
		plazaLength = 0.0f;
	}

	void end_face() {
		if (plaza.size() > 4 && numOutingEdges >= 4 && plazaLength < 1000.0f && plazaLength > 200.0f) {
			plaza_list.push_back(plaza);
		}
	}

	template <typename Vertex> 
	void next_vertex(Vertex v) {
		//plaza.push_back(v);
		numOutingEdges += GraphUtil::getNumEdges(*roadGraphPtr, v) - 2;
	}

	template <typename Edge> 
	void next_edge(Edge e) {
		plaza.push_back(e);
		plazaLength += roadGraphPtr->graph[e]->getLength();
	}
};

/**
 * グリッドを検知する
 * まず、最初に最も大きいグリッド領域を抽出する。次に、それ以外の領域について、次に大きいグリッド領域を抽出する。以降、最大6回、一定サイズ以上のグリッド領域が見つからなくなるまで繰り返す。
 *
 * @param numBins				ヒストグラムのビン数
 * @param minTotalLength		最大頻度となるビンに入ったエッジの総延長距離が、この値より小さい場合、顕著な特徴ではないと考え、グリッド検知せずにfalseを返却する
 * @param minMaxBinRatio		最大頻度となるビンの割合が、この値より小さい場合は、顕著な特徴ではないと考え、グリッド検知せずにfalseを返却する
 * @param votingRatioThreshold	各エッジについて、構成するラインが所定のグリッド方向に従っているかの投票率を計算し、この閾値未満なら、グリッドに従っていないと見なす
 */
void RoadSegmentationUtil::detectGrid(RoadGraph& roads, Polygon2D& area, int roadType, RoadFeature& roadFeature, int maxIteration, float numBins, float minTotalLength, float minMaxBinRatio, float angleThreshold, float votingRatioThreshold, float extendingDistanceThreshold, float minObbLength) {
	for (int i = 0; i < maxIteration; i++) {
		GridFeature gf(i);
		if (!detectOneGrid(roads, area, roadType, gf, numBins, minTotalLength, minMaxBinRatio, angleThreshold, votingRatioThreshold, extendingDistanceThreshold, minObbLength)) break;
		roadFeature.addFeature(gf);
	}
}

/**
 * １つのグリッドを検知する。
 * 既にグループに属しているエッジはスキップする。
 *
 * @param numBins				ヒストグラムのビン数
 * @param minTotalLength		最大頻度となるビンに入ったエッジの総延長距離が、この値より小さい場合、顕著な特徴ではないと考え、グリッド検知せずにfalseを返却する
 * @param minMaxBinRatio		最大頻度となるビンの割合が、この値より小さい場合は、顕著な特徴ではないと考え、グリッド検知せずにfalseを返却する
 *
 * @param minObbLength			エッジ群を囲むOriented Bounding Boxの短い方の辺の長さが、この値より小さい場合、グリッド検知せずにfalseを返却する
 */
bool RoadSegmentationUtil::detectOneGrid(RoadGraph& roads, Polygon2D& area, int roadType, GridFeature& gf, int numBins, float minTotalLength, float minMaxBinRatio, float angleThreshold, float votingRatioThreshold, float extendingDistanceThreshold, float minObbLength) {
	// ヒストグラムの初期化
	cv::Mat dirMat = cv::Mat::zeros(numBins, 1, CV_32F);

	int count = 0;
	float total_edge_length = 0.0f;
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		// 指定されたタイプ以外は、スキップする。
		if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;

		// 既にshapeTypeが確定しているエッジは、スキップする
		if (roads.graph[*ei]->shapeType > 0) continue;

		// エリアの外のエッジは、スキップする
		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!area.contains(roads.graph[src]->pt) && !area.contains(roads.graph[tgt]->pt)) continue;		

		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
			QVector2D dir = roads.graph[*ei]->polyLine[i + 1] - roads.graph[*ei]->polyLine[i];
			float theta = atan2f(dir.y(), dir.x());
			if (theta < 0) theta += M_PI;
			if (theta > M_PI * 0.5f) theta -= M_PI * 0.5f;

			// どのビンか決定
			int bin_id = theta * (float)numBins / M_PI * 2.0f;
			if (bin_id >= numBins) bin_id = numBins - 1;

			// 投票する
			dirMat.at<float>(bin_id, 0) += dir.length();
			count++;
			total_edge_length += dir.length();
		}
	}

	// ヒストグラムの中で、最大頻度となるビンを探す
	float max_hist_value = 0.0f;
	int max_bin_id;
	for (int i = 0; i < dirMat.rows; i++) {
		if (dirMat.at<float>(i, 0) > max_hist_value) {
			max_hist_value = dirMat.at<float>(i, 0);
			max_bin_id = i;
		}
	}

	// 最頻値（モード）のビンの割合がminMaxBinRatio%未満なら、グリッドとは見なさない
	if (max_hist_value < minTotalLength || max_hist_value < total_edge_length * minMaxBinRatio) return false;

	// グリッド方向の近似値をセットする
	gf.setAngle(((float)max_bin_id + 0.5f) / (float)numBins * M_PI * 0.5f);

	// 最頻値（モード）のビンに入るエッジを使って、高精度のグリッド方向を計算する
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		// 指定されたタイプ以外は、スキップする。
		if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;
		
		// 既にshapeTypeが確定しているエッジは、スキップする
		if (roads.graph[*ei]->shapeType > 0) continue;

		// エリアの外のエッジは、スキップする
		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!area.contains(roads.graph[src]->pt) && !area.contains(roads.graph[tgt]->pt)) continue;	

		float length = 0.0f;
		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
			gf.addEdge(roads.graph[*ei]->polyLine[i + 1] - roads.graph[*ei]->polyLine[i], M_PI * 0.5f / (float)numBins);
		}
	}
	gf.computeFeature();

	// 正確なグリッド方向を使って、グリッド方向に近いエッジをグループに登録する
	QMap<RoadEdgeDesc, float> edges;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		// 指定されたタイプ以外は、スキップする。
		if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;
		
		// 既にshapeTypeが確定しているエッジは、スキップする
		if (roads.graph[*ei]->shapeType > 0) continue;

		// エリアの外のエッジは、スキップする
		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!area.contains(roads.graph[src]->pt) && !area.contains(roads.graph[tgt]->pt)) continue;

		float length = 0.0f;
		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
			QVector2D dir = roads.graph[*ei]->polyLine[i + 1] - roads.graph[*ei]->polyLine[i];

			// エッジの方向が、グリッド方向に近い場合、投票する
			if (gf.isClose(dir, angleThreshold)) {
				length += dir.length();
			}
		}

		// votingRatioThreshold%以上、グリッドの方向と同じ方向のエッジなら、そのエッジを当該グループに入れる
		if (length >= roads.graph[*ei]->getLength() * votingRatioThreshold) {
			edges.insert(*ei, length / roads.graph[*ei]->getLength());
		}
	}

	// 最大連結成分に属さないエッジは、グループから外す
	reduceGridGroup(roads, gf, edges);

	//
	extendGridGroup(roads, area, roadType, gf, edges, angleThreshold, votingRatioThreshold, extendingDistanceThreshold);

	// 候補のエッジの総延長を計算する
	float len = 0.0f;
	for (QMap<RoadEdgeDesc, float>::iterator it = edges.begin(); it != edges.end(); ++it) {
		for (int i = 0; i < roads.graph[it.key()]->polyLine.size() - 1; i++) {
			QVector2D dir = roads.graph[it.key()]->polyLine[i + 1] - roads.graph[it.key()]->polyLine[i];
			len += dir.length();
		}
	}
	if (len < minTotalLength || len < total_edge_length * minMaxBinRatio) return false;

	// 候補のエッジ群を囲むconvex hullを求める
	ConvexHull ch;
	for (QMap<RoadEdgeDesc, float>::iterator it = edges.begin(); it != edges.end(); ++it) {
		for (int i = 0; i < roads.graph[it.key()]->polyLine.size(); i++) {
			ch.addPoint(roads.graph[it.key()]->polyLine[i]);
		}
	}
	Loop2D hull;
	ch.convexHull(hull);

	// convex hullのOriented Bounding Boxを求める
	QVector2D obb_size;
	QMatrix4x4 obb_mat;
	Polygon2D::getLoopOBB(hull, obb_size, obb_mat);

	// もしOBBの短い方のエッジの長さがminObbLength未満なら、グリッドと見なさない
	if (std::min(obb_size.x(), obb_size.y()) < minObbLength) return false;

	// 領域を表すポリゴンを設定
	for (int i = 0; i < hull.size(); ++i) {
		gf._polygon.push_back(hull[i]);
	}

	// 領域の中心をセット
	gf.center = gf._polygon.getCentroid();

	// 領域内のエッジの数をセット
	gf.weight = edges.size();

	// 最後に、このグループに属するエッジを、RoadGraphオブジェクトに反映させる
	for (QMap<RoadEdgeDesc, float>::iterator it = edges.begin(); it != edges.end(); ++it) {
		RoadEdgeDesc e = it.key();
		roads.graph[e]->shapeType = RoadEdge::SHAPE_GRID;
		roads.graph[e]->group = gf.group_id;
		roads.graph[e]->gridness = it.value();
	}
	
	roads.setModified();

	return true;
}

/**
 * 指定されたエッジから、同じグループに属するエッジを辿っていく。
 */
int RoadSegmentationUtil::traverseConnectedEdges(RoadGraph& roads, RoadEdgeDesc e, QMap<RoadEdgeDesc, int>& edges, int segment_id) {
	int count = 0;

	QList<RoadVertexDesc> queue;
	RoadVertexDesc src = boost::source(e, roads.graph);
	queue.push_back(src);

	QList<RoadVertexDesc> visited;
	visited.push_back(src);

	while (!queue.empty()) {
		RoadVertexDesc v = queue.front();
		queue.pop_front();

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v, roads.graph); ei != eend; ++ei) {
			if (!roads.graph[*ei]->valid) continue;

			// 隣接エッジが、同じグループに属するなら、再帰的に辿っていく
			if (edges.contains(*ei)) {
				edges[*ei] = segment_id;

				RoadVertexDesc u = boost::target(*ei, roads.graph);
				if (!roads.graph[u]->valid) continue;
				if (visited.contains(u)) continue;

				visited.push_back(u);
				queue.push_back(u);
				count++;
			}
		}
	}

	return count;
}

/**
 * 最大連結成分に属さないエッジは、group_idを-1に戻す
 */
void RoadSegmentationUtil::reduceGridGroup(RoadGraph& roads, GridFeature& gf, QMap<RoadEdgeDesc, float>& edges) {
	// 各エッジが、どのグループに属するか
	QMap<RoadEdgeDesc, int> groups;

	// 指定されたグループに属するエッジのセットを生成する
	for (QMap<RoadEdgeDesc, float>::iterator it = edges.begin(); it != edges.end(); ++it) {
		groups.insert(it.key(), -1);
	}

	// 各エッジと接続されているエッジの数をカウントする
	int numSegments = 0;
	std::vector<int> hist;
	for (QMap<RoadEdgeDesc, int>::iterator it = groups.begin(); it != groups.end(); ++it) {
		if (groups[it.key()] >= 0) continue;

		int num = traverseConnectedEdges(roads, it.key(), groups, numSegments);
		hist.push_back(num);

		numSegments++;
	}

	// 最大グループを取得
	int max_num = 0;
	int max_segment;
	for (int i = 0; i < hist.size(); i++) {
		if (hist[i] > max_num) {
			max_num = hist[i];
			max_segment = i;
		}
	}

	// 当該グループに属するエッジについて、最大グループ以外は、グループから外す
	for (QMap<RoadEdgeDesc, float>::iterator it = edges.begin(); it != edges.end(); ) {
		if (groups[it.key()] != max_segment) {
			it = edges.erase(it);
		} else {
			++it;
		}
	}
}

/**
 * グリッドのグループの属さないエッジについて、近くに属するエッジがあれば、そのグループの仲間に入れちゃう。
 * これをしてあげないと、例えばラウンドアバウトに挟まれたエッジなどが、グリッドの仲間に入れない。
 */
void RoadSegmentationUtil::extendGridGroup(RoadGraph& roads, Polygon2D& area, int roadType, GridFeature& gf, QMap<RoadEdgeDesc, float>& edges, float angleThreshold, float votingRatioThreshold, float distanceThreshold) {
	float distanceThreshold2 = distanceThreshold * distanceThreshold;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		// 指定されたタイプ以外は、スキップする。
		if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;
		
		// 既にshapeTypeが確定しているエッジは、スキップする
		if (roads.graph[*ei]->shapeType > 0) continue;

		// エリアの外のエッジは、スキップする
		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!area.contains(roads.graph[src]->pt) && !area.contains(roads.graph[tgt]->pt)) continue;

		float length = 0.0f;
		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
			QVector2D dir = roads.graph[*ei]->polyLine[i + 1] - roads.graph[*ei]->polyLine[i];

			// エッジの方向が、グリッド方向に近い場合、投票する
			if (gf.isClose(dir, angleThreshold)) {
				length += dir.length();
			}
		}

		// votingRatioThreshold%以上、グリッドの方向と同じ方向のエッジなら、そのエッジを当該グループに入れる
		if (length < roads.graph[*ei]->getLength() * votingRatioThreshold) continue;

		for (QMap<RoadEdgeDesc, float>::iterator it = edges.begin(); it != edges.end(); ++it) {
			RoadVertexDesc src2 = boost::source(it.key(), roads.graph);
			RoadVertexDesc tgt2 = boost::target(it.key(), roads.graph);

			// 当該グループのエッジと距離が近い場合、当該グループに入れる
			if ((roads.graph[src]->pt - roads.graph[src2]->pt).lengthSquared() < distanceThreshold2 || 
				(roads.graph[src]->pt - roads.graph[tgt2]->pt).lengthSquared() < distanceThreshold2 || 
				(roads.graph[tgt]->pt - roads.graph[src2]->pt).lengthSquared() < distanceThreshold2 || 
				(roads.graph[tgt]->pt - roads.graph[tgt2]->pt).lengthSquared() < distanceThreshold2) {
				roads.graph[*ei]->shapeType = RoadEdge::SHAPE_GRID;
				roads.graph[*ei]->group = gf.group_id;
				roads.graph[*ei]->gridness = length / roads.graph[*ei]->getLength();
				break;
			}
		}
	}
}

/**
 * Plazaを検知する
 * Hough transformにより、円を検知する。
 */
void RoadSegmentationUtil::detectRadial(RoadGraph& roads, Polygon2D& area, int roadType, RoadFeature& roadFeature, int maxIteration, float scale1, float scale2, float centerErrorTol2, float angleThreshold2, float scale3, float centerErrorTol3, float angleThreshold3, float sigma, float votingRatioThreshold, float seedDistance, float minSeedDirections, float extendingAngleThreshold) {
	int count = 0;
	/*
	for (int i = 0; i < maxIteration; ++i) {
		RadialFeature rf(count);
		if (detectOneRadial(roads, area, roadType, rf, scale1, scale2, centerErrorTol2, angleThreshold2, scale3, centerErrorTol3, angleThreshold3, sigma, votingRatioThreshold, seedDistance, minSeedDirections, extendingAngleThreshold)) {
			radialFeatures.push_back(rf);
			count++;
		}
	}
	*/

	std::vector<RadialFeature> rfs = detectRadialCentersInScaled(roads, area, roadType, scale1, sigma, 30.0f, 20.0f, 150.0f);
	for (int i = 0; i < rfs.size(); ++i) {
		QMap<RoadEdgeDesc, bool> edges;
		if (findOneRadial(roads, area, roadType, angleThreshold3, votingRatioThreshold, seedDistance, minSeedDirections, extendingAngleThreshold, rfs[i], edges)) {
			roadFeature.addFeature(rfs[i]);
		}
	}

	for (int i = 0; i < roadFeature.radialFeatures.size(); ++i) {
		QMap<RoadEdgeDesc, bool> edges;
		findOneRadial(roads, area, roadType, angleThreshold3, votingRatioThreshold, seedDistance, minSeedDirections, extendingAngleThreshold, roadFeature.radialFeatures[i], edges);

		// 残したエッジから周辺のエッジを辿り、方向がほぼ同じなら、候補に登録していく
		extendRadialGroup(roads, area, roadType, roadFeature.radialFeatures[i], edges, extendingAngleThreshold, votingRatioThreshold);

		buildRadialArea(roads, edges, roadFeature.radialFeatures[i]);

		// 最後に、候補エッジを、実際にグループに登録する
		for (QMap<RoadEdgeDesc, bool>::iterator it = edges.begin(); it != edges.end(); ++it) {
			RoadEdgeDesc e = it.key();
			roads.graph[e]->shapeType = RoadEdge::SHAPE_RADIAL;
			roads.graph[e]->group = roadFeature.radialFeatures[i].group_id;
			roads.graph[e]->gridness = 0;
		}

		roads.setModified();
	}
}

/**
 * Plazaを検知する
 * Hough transformにより、円を検知する。
 */
bool RoadSegmentationUtil::detectOneRadial(RoadGraph& roads, Polygon2D& area, int roadType, RadialFeature& rf, float scale1, float scale2, float centerErrorTol2, float angleThreshold2, float scale3, float centerErrorTol3, float angleThreshold3, float detectCircleThreshold, float sigma, float votingRatioThreshold, float seedDistance, float minSeedDirections, float extendingAngleThreshold) {
	// 0.01スケールで、円の中心を求める
	detectRadialCenterInScaled(roads, area, roadType, scale1, sigma, rf);

	// 0.1スケールで、より正確な円の中心を求める
	refineRadialCenterInScaled(roads, area, roadType, scale2, sigma, rf, centerErrorTol2, angleThreshold2);

	// 0.2スケールで、より正確な円の中心を求める
	//refineRadialCenterInScaled(roads, area, roadType, scale3, sigma, rf, centerErrorTol3, angleThreshold3);

	// HoughTransformで円を検知
	if (!detectCircle(roads, area, roadType, detectCircleThreshold, rf)) return false;

	std::cout << "Center: " << rf.center.x() << ", " << rf.center.y() << std::endl;

	QMap<RoadEdgeDesc, bool> edges;
	if (!findOneRadial(roads, area, roadType, angleThreshold3, votingRatioThreshold, seedDistance, minSeedDirections, extendingAngleThreshold, rf, edges)) return false;

	return true;
}

/**
 * Plazaを１つ検知し、円の中心を返却する
 * Hough transformにより、円を検知する。
 */
void RoadSegmentationUtil::detectRadialCenterInScaled(RoadGraph& roads, Polygon2D& area, int roadType, float scale, float sigma, RadialFeature& rf) {
	HoughTransform ht(area, scale);

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;
		
		// 指定されたタイプ以外は、スキップする。
		if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;

		// 既にshapeTypeが確定しているエッジは、スキップする
		if (roads.graph[*ei]->shapeType > 0) continue;

		// 範囲の外のエッジはスキップする
		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!area.contains(roads.graph[src]->pt) && !area.contains(roads.graph[tgt]->pt)) continue;

		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
			ht.line(roads.graph[*ei]->polyLine[i], roads.graph[*ei]->polyLine[i + 1], sigma);
			if (i > 0) {
				//ht.circle(roads.graph[*ei]->polyLine[i], roads.graph[*ei]->polyLine[i + 1], sigma);
			}
		}
	}

	rf.center = ht.maxPoint();
}

/**
 * 円の中心候補のリストを返却する
 * Hough transformにより、円を検知する。
 */
std::vector<RadialFeature> RoadSegmentationUtil::detectRadialCentersInScaled(RoadGraph& roads, Polygon2D& area, int roadType, float scale, float sigma, float candidateCenterThreshold, float detectCircleThreshold, float min_dist) {
	float min_dist2 = min_dist * min_dist;

	HoughTransform ht(area, scale);

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;
		
		// 指定されたタイプ以外は、スキップする。
		if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;

		// 既にshapeTypeが確定しているエッジは、スキップする
		if (roads.graph[*ei]->shapeType > 0) continue;

		// 範囲の外のエッジはスキップする
		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!area.contains(roads.graph[src]->pt) && !area.contains(roads.graph[tgt]->pt)) continue;

		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
			ht.line(roads.graph[*ei]->polyLine[i], roads.graph[*ei]->polyLine[i + 1], sigma);
			//ht.circle(roads.graph[*ei]->polyLine[i], roads.graph[*ei]->polyLine[i + 1], sigma);
		}
	}

	// しきい値以上の投票があった点を、円の中心の候補として取得する
	std::vector<QVector2D> centers = ht.points(candidateCenterThreshold);
	
	// 円の候補について、HoughCircleにより、チェックする
	int count = 0;
	std::vector<RadialFeature> rfs;
	for (int i = 0; i < centers.size(); ++i) {
		// 既に検知された円と近い場合は、スキップ
		bool skip = false;
		for (int j = 0; j < rfs.size(); j++) {
			if ((rfs[j].center - centers[i]).lengthSquared() < min_dist2) {
				skip = true;
				break;
			}
		}
		if (skip) continue;

		RadialFeature rf(count);
		rf.center = centers[i];

		// HoughCircleにより円が検知されるか？
		if (detectCircle(roads, area, roadType, detectCircleThreshold, rf)) {
			rfs.push_back(rf);
			count++;
		}
	}

	return rfs;
}

/**
 * 円のだいたいの中心点を使って、より正確な円の中心を返却する
 * Hough transformにより、円を検知する。
 */
void RoadSegmentationUtil::refineRadialCenterInScaled(RoadGraph& roads, Polygon2D& area, int roadType, float scale, float sigma, RadialFeature& rf, float distanceThreshold, float angleThreshold) {
	float distanceThreshold2 = distanceThreshold * distanceThreshold;

	HoughTransform ht(area, scale);

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		// 指定されたタイプ以外は、スキップする。
		if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;

		// 既にshapeTypeが確定しているエッジは、スキップする
		if (roads.graph[*ei]->shapeType > 0) continue;

		// 範囲の外のエッジはスキップする
		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!area.contains(roads.graph[src]->pt) && !area.contains(roads.graph[tgt]->pt)) continue;

		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
			QVector2D v1 = roads.graph[*ei]->polyLine[i];
			QVector2D v2 = roads.graph[*ei]->polyLine[i + 1];

			// だいたいの中心点から離れすぎたエッジは、スキップする
			if ((v1 - rf.center).lengthSquared() > distanceThreshold2 && (v2 - rf.center).lengthSquared() > distanceThreshold2) continue;

			QVector2D dir = v2 - v1;

			// だいたいの中心点への方向が大きくずれている場合は、スキップする
			if (Util::diffAngle(v1 - rf.center, dir) > angleThreshold && 
				Util::diffAngle(v1 - rf.center, -dir) > angleThreshold &&
				Util::diffAngle(v2 - rf.center, dir) > angleThreshold && 
				Util::diffAngle(v2 - rf.center, -dir) > angleThreshold) continue;

			ht.line(v1, v2, sigma);
		}
	}

	// 最大値を取得する
	QVector2D center = ht.maxPoint();

	rf.center = center;
}

/**
 * OpenCVのHoughTransform関数で円を検知する。なければ、falseを返却する。
 * 予め予測された円の中心を中心とする500m x 500mの正方形の範囲の中で、HoughTransformで円を検知し、
 * 検知された円に基づいて、円の中心と半径情報を更新する。
 *
 * @param detectCircleThreshold		HoughTransformで見つけた円の中心が、予測された円の中心からこのしきい値よりも大きくずれている場合は、スキップ
 * @param rf						およその円の中心を保持している特徴量
 */
bool RoadSegmentationUtil::detectCircle(RoadGraph& roads, Polygon2D& area, int roadType, float detectCircleThreshold, RadialFeature& rf) {
	float detectCircleThreshold2 = detectCircleThreshold * detectCircleThreshold;

	cv::Mat img = cv::Mat::zeros(500, 500, CV_8U);
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; ++i) {
			QVector2D p1 = roads.graph[*ei]->polyLine[i] - rf.center + QVector2D(250, 250);
			QVector2D p2 = roads.graph[*ei]->polyLine[i + 1] - rf.center + QVector2D(250, 250);

			if (p1.x() < 0 || p1.x() >= 500 || p2.x() < 0 || p2.x() >= 500) continue;

			cv::line(img, cv::Point(p1.x(), p1.y()), cv::Point(p2.x(), p2.y()), cv::Scalar(255), 3);
		}
	}

	cv::vector<cv::Vec3f> circles;
	cv::HoughCircles(img, circles, CV_HOUGH_GRADIENT, 1, 150, 200, 22, 0, 80);

	for (int i = 0; i < circles.size(); i++) {
		QVector2D center(circles[i][0], circles[i][1]);
		int r = circles[i][2];

		bool duplicated = false;
		for (int j = 0; j < rf.radii.size(); ++j) {
			if (fabs(rf.radii[j] - r) < 3.0f) duplicated = true;
		}
		if (duplicated) continue;

		if ((center - QVector2D(250, 250)).lengthSquared() < detectCircleThreshold2) {
			std::cout << "Circle detected: (" << center.x() << "," << center.y() << ") R: " << r << std::endl;
			rf.center += center - QVector2D(250, 250);
			rf.radii.push_back(r);
		}
	}

	if (rf.radii.size() > 0) return true;
	else return false;
}

/**
 * 与えられた円の中心に基づき、Radialタイプの道路エッジを登録する。
 */
bool RoadSegmentationUtil::findOneRadial(RoadGraph& roads, Polygon2D& area, int roadType, float angleThreshold, float votingRatioThreshold, float seedDistance, float minSeedDirections, float extendingAngleThreshold, RadialFeature& rf, QMap<RoadEdgeDesc, bool>& edges) {
	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		// 指定されたタイプ以外は、スキップする。
		if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;

		// 既にshapeTypeが確定しているエッジは、スキップする
		if (roads.graph[*ei]->shapeType > 0) continue;

		// 範囲外のエッジはスキップ
		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);
		if (!area.contains(roads.graph[src]->pt) && !area.contains(roads.graph[tgt]->pt)) continue;

		float length = 0.0f;
		for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
			QVector2D v1 = roads.graph[*ei]->polyLine[i];
			QVector2D v2 = roads.graph[*ei]->polyLine[i + 1];
			QVector2D dir = v2 - v1;

			if (Util::diffAngle(v1 - rf.center, dir) <= angleThreshold ||
				Util::diffAngle(v1 - rf.center, -dir) <= angleThreshold ||
				Util::diffAngle(v2 - rf.center, dir) <= angleThreshold || 
				Util::diffAngle(v2 - rf.center, -dir) <= angleThreshold) {
				length += dir.length();
			}
		}

		// votingRatioThreshold%以上、このradialと同じ方向なら、そのエッジを当該グループに入れる
		if (length >= roads.graph[*ei]->getLength() * votingRatioThreshold) {
			edges[*ei] = true;
		}
	}

	// 円の中心から一定距離以内のエッジのみを残す
	reduceRadialGroup(roads, rf, edges, seedDistance);

	// 中心から伸びるアームの方向を量子化してカウントする
	rf.numDirections = countNumDirections(roads, rf, edges, 12);
	if (rf.numDirections < minSeedDirections) return false;

	return true;
}

/**
 * 円の中心からdistanceThreshold以内のRadialグループのエッジのみを残し、それ以外はグループから外す。
 *
 * @param edges					radialエッジの候補
 * @param distanceThreshold		円の中心から、この距離よりも遠いエッジは、グループから外す
 */
void RoadSegmentationUtil::reduceRadialGroup(RoadGraph& roads, RadialFeature& rf, QMap<RoadEdgeDesc, bool>& edges, float distanceThreshold) {
	float distanceThreshold2 = distanceThreshold * distanceThreshold;

	int count = 0;
	for (QMap<RoadEdgeDesc, bool>::iterator it = edges.begin(); it != edges.end(); ) {
		RoadVertexDesc src = boost::source(it.key(), roads.graph);
		RoadVertexDesc tgt = boost::target(it.key(), roads.graph);

		// 円の中心から離れているエッジは、候補から一旦外す
		if ((roads.graph[src]->pt - rf.center).lengthSquared() > distanceThreshold2 && (roads.graph[tgt]->pt - rf.center).lengthSquared() > distanceThreshold2) {
			it = edges.erase(it);
		} else {
			++it;
		}
	}
}

/**
 * 指定したグループに属するエッジについて、円の中心から離れる方向に周辺のエッジを辿り、グループに登録していく。
 */
void RoadSegmentationUtil::extendRadialGroup(RoadGraph& roads, Polygon2D& area, int roadType, RadialFeature& rf, QMap<RoadEdgeDesc, bool>& edges, float angleThreshold, float dirCheckRatio) {
	QList<RoadVertexDesc> queue;

	QList<RoadVertexDesc> visited;

	// シードの構築
	for (QMap<RoadEdgeDesc, bool>::iterator it = edges.begin(); it != edges.end(); ++it) {
		RoadVertexDesc src = boost::source(it.key(), roads.graph);
		RoadVertexDesc tgt = boost::target(it.key(), roads.graph);

		queue.push_back(src);
		queue.push_back(tgt);
		visited.push_back(src);
		visited.push_back(tgt);
	}

	while (!queue.empty()) {
		RoadVertexDesc v = queue.front();
		queue.pop_front();

		// 範囲の外の頂点は除外する
		if (!area.contains(roads.graph[v]->pt)) continue;

		RoadOutEdgeIter ei, eend;
		for (boost::tie(ei, eend) = boost::out_edges(v, roads.graph); ei != eend; ++ei) {
			if (!roads.graph[*ei]->valid) continue;

			// 指定されたタイプ以外は、スキップする。
			if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;

			float length = 0.0f;
			for (int i = 0; i < roads.graph[*ei]->polyLine.size() - 1; i++) {
				QVector2D dir1 = roads.graph[*ei]->polyLine[i + 1] - roads.graph[*ei]->polyLine[i];
				QVector2D dir2 = roads.graph[*ei]->polyLine[i] - rf.center;

				if (Util::diffAngle(dir1, dir2) < angleThreshold || Util::diffAngle(dir1, -dir2) < angleThreshold) {
					length += dir1.length();
				}
			}

			// 方向がずれてたら、そのエッジは当該グループに入れない
			if (length < roads.graph[*ei]->getLength() * dirCheckRatio) continue;

			// 候補に追加する
			edges[*ei] = true;

			RoadVertexDesc u = boost::target(*ei, roads.graph);
			if (!roads.graph[u]->valid) continue;
			if (visited.contains(u)) continue;

			// シードとして、追加する
			visited.push_back(u);
			queue.push_back(u);
		}
	}
}

/**
 * 指定された中心から伸びるradial道路の方向を、指定されたサイズで量子化し、道路が延びている方向の数を返却する。
 */
int RoadSegmentationUtil::countNumDirections(RoadGraph& roads, const RadialFeature& rf, QMap<RoadEdgeDesc, bool>& edges, int size) {
	QSet<int> directions;

	for (QMap<RoadEdgeDesc, bool>::iterator it = edges.begin(); it != edges.end(); ++it) {
		RoadVertexDesc src = boost::source(it.key(), roads.graph);
		RoadVertexDesc tgt = boost::target(it.key(), roads.graph);

		// 円の中心から離れている方の頂点と、円の中心を使って、方向を計算する
		QVector2D vec;
		if ((roads.graph[src]->pt - rf.center).lengthSquared() > (roads.graph[tgt]->pt - rf.center).lengthSquared()) {
			vec = roads.graph[src]->pt - rf.center;
		} else {
			vec = roads.graph[tgt]->pt - rf.center;
		}
		float angle = atan2f(vec.y(), vec.x());
		if (angle < 0) angle += M_PI * 2.0f;

		// 方向を量子化する
		int bin_id = angle / M_PI / 2.0f * size;
		if (bin_id < 0) bin_id = 0;
		if (bin_id >= size) bin_id = size - 1;

		directions.insert(bin_id);
	}

	return directions.size();
}

void RoadSegmentationUtil::buildRadialArea(RoadGraph& roads, QMap<RoadEdgeDesc, bool>& edges, RadialFeature& rf) {
	// 候補のエッジ群を囲むconvex hullを求める
	ConvexHull ch;
	for (QMap<RoadEdgeDesc, bool>::iterator it = edges.begin(); it != edges.end(); ++it) {
		for (int i = 0; i < roads.graph[it.key()]->polyLine.size(); i++) {
			ch.addPoint(roads.graph[it.key()]->polyLine[i]);
		}
	}
	Loop2D hull;
	ch.convexHull(hull);

	// 領域を表すポリゴンを設定
	for (int i = 0; i < hull.size(); ++i) {
		rf._polygon.push_back(hull[i]);
	}
}

/**
 * GridでもRadialでもないエッジについて、一般的な特徴量を抽出する。
 */
void RoadSegmentationUtil::extractGenericFeature(RoadGraph& roads, Polygon2D& area, RoadFeature& roadFeature) {
	GenericFeature gf(0);

	BBox bbox;

	RoadEdgeIter ei, eend;
	for (boost::tie(ei, eend) = boost::edges(roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;

		// GridまたはRadialのエッジは、スキップ
		if (roads.graph[*ei]->shapeType > 0) continue;

		RoadVertexDesc src = boost::source(*ei, roads.graph);
		RoadVertexDesc tgt = boost::target(*ei, roads.graph);

		// エリア外ならスキップ
		if (!area.contains(roads.graph[src]->pt) && !area.contains(roads.graph[tgt]->pt)) continue;

		// Bounding Boxを更新
		if (area.contains(roads.graph[src]->pt)) {
			bbox.addPoint(roads.graph[src]->pt);
		}
		if (area.contains(roads.graph[tgt]->pt)) {
			bbox.addPoint(roads.graph[tgt]->pt);
		}

		int roadType = roads.graph[*ei]->type;
		float length = roads.graph[*ei]->getLength();

		gf.addEdge(length, roadType);
	}

	RoadVertexIter vi, vend;
	for (boost::tie(vi, vend) = boost::vertices(roads.graph); vi != vend; ++vi) {
		if (!roads.graph[*vi]->valid) continue;

		if (getNumEdges(roads, *vi, 2, 0) > 0) {
			gf.addNumDiretions(GraphUtil::getNumEdges(roads, *vi, 2), 2);
		}

		if (getNumEdges(roads, *vi, 1, 0) > 0) {
			gf.addNumDiretions(GraphUtil::getNumEdges(roads, *vi, 3), 1);
		}
	}

	// 領域の中心を設定
	gf.center = bbox.midPt();

	gf.computeFeature();

	roadFeature.addFeature(gf);
}

int RoadSegmentationUtil::getNumEdges(RoadGraph &roads, RoadVertexDesc v, int roadType, int shapeType) {
	int count = 0;
	RoadOutEdgeIter ei, eend;
	for (boost::tie(ei, eend) = out_edges(v, roads.graph); ei != eend; ++ei) {
		if (!roads.graph[*ei]->valid) continue;
		
		if (!GraphUtil::isRoadTypeMatched(roads.graph[*ei]->type, roadType)) continue;

		if (roads.graph[*ei]->shapeType != shapeType) continue;

		count++;
	}

	return count;
}