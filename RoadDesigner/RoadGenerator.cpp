#include "RoadGenerator.h"
#include "Util.h"
#include "GraphUtil.h"

RoadGenerator::RoadGenerator() {
}

RoadGenerator::~RoadGenerator() {
}

/**
 * グリッドパターンの道路網を生成する
 */
void RoadGenerator::generateRoadNetwork(RoadArea& roadArea, const GridFeature& gf) {
	// グリッド方向に合わせたBBoxのサイズを取得
	Loop2D bboxRotLoop;
	float angle0 = gf.getAngles()[0];
	QVector2D dir0 = QVector2D(cos(angle0), sinf(angle0));
	roadArea.area.getLoopOBB(dir0, bboxRotLoop);
	Polygon2D area2;
	area2.setContour(bboxRotLoop);
	
	// Avenue用シード
	std::list<RoadVertexDesc> initSeeds;

	// Avenue用のシードを生成
	generateHorizontalAvenueSeeds(roadArea.roads, area2, gf, initSeeds);

	// Avenue生成
	std::list<RoadVertexDesc> seeds = initSeeds;
	while (!seeds.empty()) {
		seeds = generateHorizontalAvenues(roadArea.roads, area2, gf, seeds, 1, gf.generateLength(1, Util::uniform_rand()));
	}
	seeds = initSeeds;
	while (!seeds.empty()) {
		seeds = generateHorizontalAvenues(roadArea.roads, area2, gf, seeds, 3, gf.generateLength(3, Util::uniform_rand()));
	}

	// 元々のエリアでcroppingする
	GraphUtil::extractRoads2(roadArea.roads, roadArea.area);
}

/**
 * Radialパターンの道路網を生成する
 */
void RoadGenerator::generateRoadNetwork(RoadArea& roadArea, const RadialFeature& rf) {
	// 円形の道路を生成すると共に、シードを生成する
	std::list<RoadVertexDesc> seeds;
	generateCircleAvenues(roadArea.roads, roadArea.area, rf, seeds);

	// シードから放射線状にAvenueを生成する
	while (!seeds.empty()) {
		RoadVertexDesc desc = seeds.front();
		seeds.pop_front();
		expandRadialAvenues(roadArea.roads, roadArea.area, rf, desc);
	}
}

/**
 * エリアの中心を通る横方向のライン上に、Avenueを生成し、シードとする。
 */
void RoadGenerator::generateHorizontalAvenueSeeds(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds) {
	seeds.clear();

	BBox bbox = area.getLoopAABB();

	// エリアの中心に頂点を作成する
	RoadVertexPtr center_v = RoadVertexPtr(new RoadVertex(bbox.midPt()));
	center_v->angles = gf.getAngles();
	center_v->lengths = gf.getLengths();

	RoadVertexDesc center_desc = GraphUtil::addVertex(roads, center_v);
	seeds.push_back(center_desc);

	// 中心点から、左方向に頂点を追加していく
	expandHorizontalAvenueSeeds(roads, area, gf, center_desc, 2, seeds);

	// 中心点から、右方向に頂点を追加していく
	expandHorizontalAvenueSeeds(roads, area, gf, center_desc, 0, seeds);
}

/**
 * 指定された頂点から、さらに横方向にAvenueのシードを伸ばしていく。
 *
 * @param start_desc			この頂点から、左または右に伸ばしていく
 * @param direction				0 -- 右 / 2 -- 左
 * @param seeds					新たにシードに追加される
 */
void RoadGenerator::expandHorizontalAvenueSeeds(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, RoadVertexDesc start_desc, int direction, std::list<RoadVertexDesc>& seeds) {
	QVector2D pt = roads.graph[start_desc]->pt;
	float angle = gf.getAngles()[direction];
	QVector2D dir = QVector2D(cosf(angle), sinf(angle));

	RoadVertexDesc prev_desc = start_desc;
	while (true) {
		pt += dir * gf.generateLength(direction, Util::uniform_rand());

		// エリアの外に出たらストップ
		if (!area.contains(pt)) {
			// エリア外周との交点を求める
			float tab, tcd;
			QVector2D intPoint;
			area.intersect(roads.graph[prev_desc]->pt, pt, &tab, &tcd, intPoint);

			// 外周上に頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(intPoint));
			RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

			// １つ前の頂点との間にエッジを生成
			GraphUtil::addEdge(roads, prev_desc, desc, 2, 1);

			break;
		}

		// 頂点を追加
		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
		RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

		// エッジを追加
		GraphUtil::addEdge(roads, prev_desc, desc, 2, 1);
		prev_desc = desc;

		// シードに追加
		if (direction == 0) {
			seeds.push_back(desc);
		} else {
			seeds.push_front(desc);
		}
	}
}

/**
 * 横方向のシードAvenueを上または下にシフトさせて、横方向のAvenueを生成し、生成された頂点を返却する。
 */
std::list<RoadVertexDesc> RoadGenerator::generateHorizontalAvenues(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, std::list<RoadVertexDesc>& seeds, int dir, float length) {
	std::list<RoadVertexDesc> newSeeds;

	std::vector<float> angles = gf.getAngles();

	QVector2D offset;
	switch (dir) {
	case 1:
		offset = QVector2D(cosf(angles[1]), sinf(angles[1])) * length;
		break;
	case 3:
		offset = QVector2D(cosf(angles[3]), sinf(angles[3])) * length;
		break;
	}

	// 最初のシード頂点の情報を保管しておく
	RoadVertexDesc start_seed_desc = seeds.front();
	RoadVertexDesc start_desc;

	// 各シード頂点からオフセットの位置に新たな頂点を追加していく
	RoadVertexDesc prev_desc;
	bool prev_valid = false;
	while (!seeds.empty()) {
		RoadVertexDesc seed_desc = seeds.front();
		seeds.pop_front();

		QVector2D pt = roads.graph[seed_desc]->pt + offset;
		
		// エリアの外なら、スキップ
		if (!area.contains(pt)) {
			// エリア外周との交点を求める
			float tab, tcd;
			QVector2D intPoint;
			area.intersect(roads.graph[seed_desc]->pt, pt, &tab, &tcd, intPoint);

			// 外周上に頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(intPoint));
			RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

			// 縦方向のエッジを追加
			GraphUtil::addEdge(roads, seed_desc, desc, 2, 1);

			continue;
		}

		// 頂点を追加
		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
		RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

		// 縦方向のエッジを追加
		GraphUtil::addEdge(roads, seed_desc, desc, 2, 1);

		// 横方向のエッジを追加
		if (prev_valid) {
			GraphUtil::addEdge(roads, prev_desc, desc, 2, 1);
		} else {
			start_desc = desc;
		}

		// シードに追加
		newSeeds.push_back(desc);

		prev_desc = desc;
		prev_valid = true;
	}

	// さらに、右方向に伸ばしてみる
	if (prev_valid) {
		expandHorizontalAvenue(roads, area, gf, prev_desc, 0, newSeeds);
	}

	// 最初のシード頂点を基点とし、左方向に伸ばしてみる
	if (prev_valid) {
		expandHorizontalAvenue(roads, area, gf, start_desc, 2, newSeeds);
	}

	return newSeeds;
}

/**
 * 指定された頂点から、さらに横方向にAvenueを伸ばしていく。
 *
 * @param start_desc			この頂点から、左または右に伸ばしていく
 * @param direction				0 -- 右 / 2 -- 左
 * @param newSeeds				新たにシードに追加される
 */
void RoadGenerator::expandHorizontalAvenue(RoadGraph& roads, const Polygon2D& area, const GridFeature& gf, RoadVertexDesc start_desc, int direction, std::list<RoadVertexDesc>& seeds) {
	RoadVertexDesc prev_desc = start_desc;

	QVector2D pt = roads.graph[start_desc]->pt;
	float angle = gf.getAngles()[direction];
	QVector2D dir = QVector2D(cosf(angle), sinf(angle));

	while (true) {
		pt += dir * gf.generateLength(direction, Util::uniform_rand());

		// エリアの外に出たらストップ
		if (!area.contains(pt)) {
			// エリア外周との交点を求める
			float tab, tcd;
			QVector2D intPoint;
			area.intersect(roads.graph[prev_desc]->pt, pt, &tab, &tcd, intPoint);

			// 外周上に頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(intPoint));
			RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

			// １つ前の頂点との間にエッジを生成
			GraphUtil::addEdge(roads, prev_desc, desc, 2, 1);

			break;
		}

		// 頂点を追加
		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
		RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

		// 横方向のエッジを追加
		GraphUtil::addEdge(roads, prev_desc, desc, 2, 1);
		prev_desc = desc;

		// シードに追加
		if (direction == 0) {
			seeds.push_back(desc);
		} else {
			seeds.push_front(desc);
		}
	}
}

/**
 * 指定されたRadial Featureに基づき、円形の道路を生成する。
 */
void RoadGenerator::generateCircleAvenues(RoadGraph& roads, const Polygon2D& area, const RadialFeature& rf, std::list<RoadVertexDesc>& seeds) {
	seeds.clear();

	BBox bbox = area.getLoopAABB();
	QVector2D center = bbox.midPt();

	float theta = Util::uniform_rand() * M_PI * 2.0f;
	float theta_step = M_PI * 2.0f / rf.numDirections;

	std::vector<RoadVertexDesc> first_descs;
	std::vector<RoadVertexDesc> prev_descs;
	first_descs.resize(rf.radii.size());
	prev_descs.resize(rf.radii.size());

	for (int i = 0; i < rf.numDirections; ++i, theta += theta_step) {
		for (int j = 0; j < rf.radii.size(); ++j) {
			// 頂点を追加
			QVector2D pt = center + QVector2D(cosf(theta), sinf(theta)) * rf.radii[j];
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
			RoadVertexDesc desc = GraphUtil::addVertex(roads, v);
			roads.graph[desc]->angles.push_back(Util::normalizeAngle(theta));

			if (j > 0) {
				// １つ内側の頂点との間に、エッジを生成
				GraphUtil::addEdge(roads, desc - 1, desc, 2, 1);
			}

			if (j == rf.radii.size() - 1) {
				// 一番外の頂点を、シードに追加
				seeds.push_back(desc);
			}

			if (i == 0) {
				// 最初の頂点を保存しておく
				first_descs[j] = desc;
			} else {
				// 時計方向に１つ前の頂点との間に、エッジを生成
				GraphUtil::addEdge(roads, prev_descs[j], desc, 2, 1);
			}

			prev_descs[j] = desc;
		}
	}

	// 最後の頂点と最初の頂点を結ぶエッジを追加
	for (int i = 0; i < rf.radii.size(); ++i) {
		GraphUtil::addEdge(roads, prev_descs[i], first_descs[i], 2, 1);
	}
}

/** 
 * 指定された頂点から、道路を延長していく。
 * この道路は、放射線上の道路のうちの１つの線を構成する。
 */
void RoadGenerator::expandRadialAvenues(RoadGraph& roads, const Polygon2D& area, const RadialFeature& rf, RoadVertexDesc desc) {
	RoadVertexDesc prev_desc = desc;

	while (true) {
		float theta = roads.graph[prev_desc]->angles[0];
		QVector2D pt = roads.graph[prev_desc]->pt + QVector2D(cosf(theta), sinf(theta)) * 200.0f;

		// エリア内かどうかチェック
		if (!area.contains(pt)) {
			// エリア外周との交点を求める
			float tab, tcd;
			QVector2D intPoint;
			area.intersect(roads.graph[prev_desc]->pt, pt, &tab, &tcd, intPoint);

			// 外周上に頂点を追加
			RoadVertexPtr v = RoadVertexPtr(new RoadVertex(intPoint));
			RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

			// １つ前の頂点との間にエッジを生成
			GraphUtil::addEdge(roads, prev_desc, desc, 2, 1);

			break;
		}

		// 頂点を追加
		RoadVertexPtr v = RoadVertexPtr(new RoadVertex(pt));
		RoadVertexDesc desc = GraphUtil::addVertex(roads, v);

		// とりあえず、角度は１つ前のものをコピーしておく
		roads.graph[desc]->angles.push_back(theta);

		// １つ前の頂点との間にエッジを生成
		GraphUtil::addEdge(roads, prev_desc, desc, 2, 1);

		prev_desc = desc;
	}
}
