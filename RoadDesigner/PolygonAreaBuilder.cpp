#include "PolygonAreaBuilder.h"

PolygonAreaBuilder::PolygonAreaBuilder() {
	_selecting = false;
}

void PolygonAreaBuilder::start(const QVector2D& pt) {
	_polyline.clear();
	_polyline.push_back(pt);

	_selecting = true;
}

void PolygonAreaBuilder::addPoint(const QVector2D& pt) {
	_polyline.push_back(pt);
}

void PolygonAreaBuilder::moveLastPoint(const QVector2D& pt) {
	if (_polyline.size() == 0) return;

	_polyline[_polyline.size() - 1] = pt;
}

void PolygonAreaBuilder::end() {
	_selecting = false;
}

void PolygonAreaBuilder::cancel() {
	_polyline.clear();
	_selecting = false;
}

bool PolygonAreaBuilder::selected() const {
	return !_selecting && _polyline.size() >= 3;
}

bool PolygonAreaBuilder::selecting() const {
	return _selecting;
}

Polygon2D PolygonAreaBuilder::polygon() const {
	Polygon2D area;

	for (int i = 0; i < _polyline.size(); ++i) {
		area.push_back(_polyline[i]);
	}

	area.correct();

	return area;
}
