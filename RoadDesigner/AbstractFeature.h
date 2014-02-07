#pragma once

#include <QVector2D>
#include <QDomDocument>
#include <QDomNode>
#include <boost/shared_ptr.hpp>

class AbstractFeature {
public:
	static enum { TYPE_GRID = 0, TYPE_RADIAL, TYPE_GENERIC };

protected:
	int _type;
	QVector2D _center;
	float _weight;

public:
	AbstractFeature();
	~AbstractFeature() {}

	int type() const;
	QVector2D center() const;
	void setCenter(const QVector2D &center);
	float weight() const;
	void setWeight(float weight);

	virtual void load(QDomNode& node) = 0;
	virtual void save(QDomDocument& doc, QDomNode& node) = 0;
};

typedef boost::shared_ptr<AbstractFeature> AbstractFeaturePtr;