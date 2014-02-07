#pragma once

#include "GridFeature.h"
#include "RadialFeature.h"
#include "GenericFeature.h"
#include "AbstractFeature.h"
#include <vector>

class RoadFeature {
public:
	std::vector<AbstractFeaturePtr> features;

public:
	RoadFeature() {}
	~RoadFeature() {}

	void clear();
	void load(QString filename);
	void save(QString filename);

	void addFeature(AbstractFeaturePtr feature);

	void normalize();
};

