#pragma once

#include <QDomNode>

class GenericFeature {
public:
	int group_id;

public:
	GenericFeature() {}
	GenericFeature(int group_id) : group_id(group_id) {}
	~GenericFeature() {}

	void load(QString filename);
	void load(QDomNode& node);
};

