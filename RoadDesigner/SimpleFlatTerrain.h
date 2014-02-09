#pragma once

#include <common/Renderable.h>

class SimpleFlatTerrain {
public:
	float elevation;
	float width;
	float depth;
	bool modified;

	std::vector<RenderablePtr> renderables;

public:
	SimpleFlatTerrain();
	SimpleFlatTerrain(float elevation, int width, int depth);
	~SimpleFlatTerrain() {}

	void generateMesh();
};
