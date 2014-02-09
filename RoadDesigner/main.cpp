#include <list>
#include <QtGui/QApplication>
#include <common/Polygon2D.h>
#include <common/VoronoiUtil.h>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
	Polygon2D area;
	area.push_back(QVector2D(10, 20));
	area.push_back(QVector2D(10, 30));
	area.push_back(QVector2D(20, 30));
	area.push_back(QVector2D(10, 20));

	Polygon2D obb;
	QVector2D size = area.getOBB(QVector2D(1, 1), obb);

	Polygon2D area2;
	area.rotate(90, area2);

	QVector2D intPt;
	area.intersects(QVector2D(3, 5), QVector2D(3, -5), intPt);


	/*
	Polygon2D area2;
	area2.push_back(QVector2D(50, 50));
	area2.push_back(QVector2D(50, -1000));
	area2.push_back(QVector2D(-1000, -1000));
	area2.push_back(QVector2D(-1000, 50));

	area2.intersection(area);
	

	std::vector<VoronoiVertex> points;
	std::vector<Polygon2D> faces;
	points.push_back(VoronoiVertex(QVector2D(0, 0)));
	points.push_back(VoronoiVertex(QVector2D(100, 0)));
	points.push_back(VoronoiVertex(QVector2D(0, 100)));
	VoronoiUtil::voronoi(points, area, faces);
	*/

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
