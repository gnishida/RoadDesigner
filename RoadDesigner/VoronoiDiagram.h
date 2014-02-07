#pragma once

#include "RoadGraph.h"
#include <QVector2D>
#include <vector>

class VoronoiDiagram {
public:
	RoadGraph edges;
	std::vector<QVector2D> points;
	std::vector<int> groups;

public:
	VoronoiDiagram();
	~VoronoiDiagram();

	void clear();
};

