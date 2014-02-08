#pragma once

#include "../../common/Polygon2D.h"

class PolygonAreaBuilder {
private:
	bool _selecting;
	Loop2D _polyline;

public:
	PolygonAreaBuilder();
	~PolygonAreaBuilder() {}

	void start(const QVector2D& pt);
	void addPoint(const QVector2D& pt);
	void moveLastPoint(const QVector2D& pt);
	void end();
	void cancel();

	bool selected() const;
	bool selecting() const;
	Polygon2D polygon() const;
};

