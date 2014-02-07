#include "MainWindow.h"
#include "Polygon2D.h"
#include "VoronoiUtil.h"
#include <QtGui/QApplication>
#include <list>

int main(int argc, char *argv[]) {
	/*
	Polygon2D area;
	area.push_back(QVector2D(-100, -100));
	area.push_back(QVector2D(-100, 200));
	area.push_back(QVector2D(200, 200));
	area.push_back(QVector2D(200, -100));
	
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
