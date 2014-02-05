#pragma once

#include <vector>
#include <QVector2D>
#include <QMap>
#include <QDomNode>

class GenericFeature {
public:
	int group_id;
	QMap<float, float> avenueLengths;
	QMap<int, float> avenueNumDirections;
	QMap<float, float> streetLengths;
	QMap<int, float> streetNumDirections;

public:
	GenericFeature() {}
	GenericFeature(int group_id) : group_id(group_id) {}
	~GenericFeature() {}

	std::vector<float> getAngles(int num) const;
	std::vector<float> getAngles(int num, const QVector2D &dir) const;
	std::vector<float> getPerpendicularAngles(const QVector2D &dir) const;
	std::vector<float> getLengths(int roadType, int num) const;
	float generateLength(int roadType, float uniform_random_number) const;
	int getDirections(int roadType) const;

	void load(QString filename);
	void load(QDomNode& node);
	void loadAvenue(QDomNode& node);
	void loadStreet(QDomNode& node);
};

