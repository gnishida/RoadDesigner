#pragma once

#include "GridFeature.h"
#include "RadialFeature.h"
#include "GenericFeature.h"
#include <vector>

class RoadFeature {
public:
	std::vector<GridFeature> gridFeatures;
	std::vector<RadialFeature> radialFeatures;
	std::vector<GenericFeature> genericFeatures;

public:
	RoadFeature() {}
	~RoadFeature() {}

	void clear();
	void load(QString filename);
	void save(QString filename);

	void addFeature(GridFeature& gf);
	void addFeature(RadialFeature& rf);
	void addFeature(GenericFeature& gf);
};

