#pragma once

#include <qvector2d.h>
#include <boost/shared_ptr.hpp>

class RoadVertex {
public:
	QVector2D pt;
	bool valid;
	bool virt;

public:
	RoadVertex();
	RoadVertex(const QVector2D &pt);

	const QVector2D& getPt() const;
};

typedef boost::shared_ptr<RoadVertex> RoadVertexPtr;
