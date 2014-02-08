#pragma once

#include "AbstractFeature.h"
#include "../../common/Polygon2D.h"
#include <QDomNode>
#include <QColor>
#include <QVector2D>
#include <boost/shared_ptr.hpp>

class RadialFeature : public AbstractFeature {
public:
	int group_id;
	std::vector<float> radii;
	int numDirections;
	
	Polygon2D _polygon;			// 領域を表すポリゴン（時計回り）

public:
	RadialFeature(int group_id);
	~RadialFeature() {}

	void load(QString filename);
	void load(QDomNode& node);
	void save(QString filename);
	void save(QDomDocument& doc, QDomNode& node);

	QColor color();
	Polygon2D polygon();
};

typedef boost::shared_ptr<RadialFeature> RadialFeaturePtr;