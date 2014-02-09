#include "SimpleFlatTerrain.h"
#include <common/Renderable.h>

SimpleFlatTerrain::SimpleFlatTerrain() {
	modified = true;
}

SimpleFlatTerrain::SimpleFlatTerrain(float elevation, int width, int depth) {
	this->elevation = elevation;
	this->width = width;
	this->depth = depth;
	modified = true;
}

void SimpleFlatTerrain::generateMesh() {
	renderables.clear();

	renderables.push_back(RenderablePtr(new Renderable(GL_QUADS)));

	Vertex v;
	v.color[0] = 233.0f / 255.0f;
	v.color[1] = 229.0f / 255.0f;
	v.color[2] = 220.0f / 255.0f;
	v.color[3] = 1.0f;
	v.normal[0] = 0.0f;
	v.normal[1] = 0.0f;
	v.normal[2] = 1.0f;

	v.location[0] = -width/2;
	v.location[1] = -depth/2;
	v.location[2] = elevation;
	renderables[0]->vertices.push_back(v);

	v.location[0] = width/2;
	v.location[1] = -depth/2;
	renderables[0]->vertices.push_back(v);

	v.location[0] = width/2;
	v.location[1] = depth/2;
	renderables[0]->vertices.push_back(v);

	v.location[0] = -width/2;
	v.location[1] = depth/2;
	renderables[0]->vertices.push_back(v);

	modified = false;
}

