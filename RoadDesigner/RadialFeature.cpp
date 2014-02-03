#include "RadialFeature.h"

QColor RadialFeature::color() {
	return QColor(255 - group_id * 64 % 255, 0, 0);
}

/**
 * 領域を表すポリゴンを返却する。
 */
Polygon2D RadialFeature::polygon() {
	return _polygon;
}