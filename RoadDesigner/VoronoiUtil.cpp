#include "VoronoiUtil.h"
#include "GraphUtil.h"
#include "Util.h"

/**
 * Based on the existing road graph and the selected road graph, build a voronoi diagram.
 */
void VoronoiUtil::voronoi(std::vector<VoronoiVertex> points, Polygon2D& area, std::vector<Polygon2D>& faces) {
	faces.clear();
	faces.resize(points.size());

	// Construction of the Voronoi Diagram.
	boost::polygon::voronoi_diagram<double> vd;
	construct_voronoi(points.begin(), points.end(), &vd);

	// ボロノイ図の各セルについて
	for (boost::polygon::voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it) {
		const boost::polygon::voronoi_diagram<double>::cell_type& cell = *it;
		const boost::polygon::voronoi_diagram<double>::edge_type* edge = cell.incident_edge();

	    std::size_t cell_index = it->source_index();
		VoronoiVertex v = points[cell_index];

		// ボロノイ図の各セルについて、エッジのサイクルを辿る（反時計回り？）
		bool started = false;
		Polygon2D face;
		do {
			if (!edge->is_primary()) continue;
				
			if (edge->is_finite()) {
				QVector2D p0((float)edge->vertex0()->x() * 0.01f, (float)edge->vertex0()->y() * 0.01f);
				QVector2D p1((float)edge->vertex1()->x() * 0.01f, (float)edge->vertex1()->y() * 0.01f);

				faces[cell_index].push_back(p0);
			} else {
				if (!started) {
					started = true;

					// 隣接セルのVoronoi頂点を取得
					VoronoiVertex v2 = points[edge->twin()->cell()->source_index()];

					// 隣接セルへのベクトルを計算得
					QVector2D midPt = (v.pt + v2.pt) * 0.5f;

					if (edge->vertex1() != NULL) {
						QVector2D p1((float)edge->vertex1()->x() * 0.01f, (float)edge->vertex1()->y() * 0.01f);

						// p1から中間点方向に十分長い距離を伸ばした点を取得
						QVector2D p0 = midPt + (midPt - p1) * 1000.0f;

						faces[cell_index].push_back(p0);
					} else {
						QVector2D p0((float)edge->vertex0()->x() * 0.01f, (float)edge->vertex0()->y() * 0.01f);

						// p0から中間点方向に十分長い距離を伸ばした点を取得
						QVector2D p1 = midPt + (midPt - p0) * 1000.0f;

						faces[cell_index].push_back(p1);
					}
				} else {
					started = false;

					QVector2D p0((float)edge->vertex0()->x() * 0.01f, (float)edge->vertex0()->y() * 0.01f);
					faces[cell_index].push_back(p0);
					
					// 隣接セルのVoronoi頂点を取得
					VoronoiVertex v2 = points[edge->twin()->cell()->source_index()];

					// 隣接セルとの中間点を取得
					QVector2D midPt = (v.pt + v2.pt) * 0.5f;

					// p0から中間点方向に十分長い距離を伸ばした点を取得
					QVector2D p1 = midPt + (midPt - p0) * 1000.0f;

					faces[cell_index].push_back(p0);
				}
			}

			edge = edge->next();
		} while (edge != cell.incident_edge());
	}
}

