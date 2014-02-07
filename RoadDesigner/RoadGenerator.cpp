#include "RoadGenerator.h"
#include "Util.h"
#include "GraphUtil.h"
#include "GridRoadGenerator.h"
#include "RadialRoadGenerator.h"
#include "GenericRoadGenerator.h"
#include "VoronoiUtil.h"

RoadGenerator::RoadGenerator() {
}

RoadGenerator::~RoadGenerator() {
}

/**
 * 道路網を生成する
 * 暫定的な実装。要改善。
 */
void RoadGenerator::generateRoadNetwork(RoadArea& roadArea, const RoadFeature& rf) {
	//roadArea.roads.clear();

	if (rf.features.size() == 0) return;

	GridRoadGenerator rg1;
	RadialRoadGenerator rg2;
	GenericRoadGenerator rg3;

	/*
	QVector2D center = roadArea.area.getCentroid();

	std::vector<VoronoiVertex> points;
	for (int i = 0; i < rf.features.size(); ++i) {
		points.push_back(VoronoiVertex(rf.features[i]->center() + center));
	}
	std::vector<Polygon2D> faces;
	VoronoiUtil::voronoi(points, roadArea.area, faces);

	for (int i = 0; i < faces.size(); ++i) {
		// セルのポリゴンの順を時計回りになおす
		//faces[i].reverse();

		// このエリアで、各セルをcroppingする（ボロノイが完璧になれば、この処理は不要になるのだが、、、）
		faces[i].intersection(roadArea.area);

		switch (rf.features[0]->type()) {
		case AbstractFeature::TYPE_GRID:
			rg1.generateRoadNetwork(roadArea.roads, faces[i], dynamic_cast<GridFeature&>(*rf.features[0]));
			break;
		case AbstractFeature::TYPE_RADIAL:
			rg2.generateRoadNetwork(roadArea.roads, faces[i], dynamic_cast<RadialFeature&>(*rf.features[0]));
			break;
		case AbstractFeature::TYPE_GENERIC:
			rg3.generateRoadNetwork(roadArea.roads, faces[i], dynamic_cast<GenericFeature&>(*rf.features[0]));
			break;
		}

	}
	*/

	switch (rf.features[0]->type()) {
	case AbstractFeature::TYPE_GRID:
		rg1.generateRoadNetwork(roadArea.roads, roadArea.area, dynamic_cast<GridFeature&>(*rf.features[0]));
		break;
	case AbstractFeature::TYPE_RADIAL:
		rg2.generateRoadNetwork(roadArea.roads, roadArea.area, dynamic_cast<RadialFeature&>(*rf.features[0]));
		break;
	case AbstractFeature::TYPE_GENERIC:
		rg3.generateRoadNetwork(roadArea.roads, roadArea.area, dynamic_cast<GenericFeature&>(*rf.features[0]));
		break;
	}
}

