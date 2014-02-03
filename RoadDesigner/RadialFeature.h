#pragma once

#include "Polygon2D.h"
#include <QColor>
#include <QVector2D>

class RadialFeature {
public:
	int group_id;
	QVector2D center;
	float radius;
	
	Polygon2D _polygon;			// 領域を表すポリゴン（時計回り）

public:
	RadialFeature(int group_id) : group_id(group_id) {}
	~RadialFeature() {}

	QColor color();
	Polygon2D polygon();
};

