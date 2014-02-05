#pragma once

#include <vector>
#include <QVector2D>
#include <boost/shared_ptr.hpp>

class RoadVertex {
public:
	QVector2D pt;
	bool valid;
	bool virt;

	bool onBoundary;
	std::vector<float> angles;
	std::vector<float> lengths;

public:
	RoadVertex();
	RoadVertex(const QVector2D &pt);

	const QVector2D& getPt() const;
};

typedef boost::shared_ptr<RoadVertex> RoadVertexPtr;
