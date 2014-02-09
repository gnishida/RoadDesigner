#pragma once

#include <QVector2D>
#include <common/common.h>
#include <common/Polygon2D.h>

class PolygonAreaBuilder {
private:
	bool _selecting;
	Polyline2D _polyline;

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

