#include "AbstractFeature.h"

AbstractFeature::AbstractFeature() {
	_weight = 0.0f;
}

int AbstractFeature::type() const {
	return _type;
}

QVector2D AbstractFeature::center() const {
	return _center;
}

void AbstractFeature::setCenter(const QVector2D &center) {
	_center = center;
}

float AbstractFeature::weight() const {
	return _weight;
}

void AbstractFeature::setWeight(float weight) {
	_weight = weight;
}
