#pragma once

#include <common/Renderable.h>
#include <common/ElevationColorTable.h>
#include "SimpleTerrainCell.h"

class SimpleTerrain {
public:
	int width;
	int depth;
	int numCols;
	int numRows;
	int cellLength;
	std::vector<SimpleTerrainCell> cells;

	ElevationColorTable colorTable;
	std::vector<RenderablePtr> renderables;

public:
	SimpleTerrain() {}
	~SimpleTerrain() {}

	int getNumCols() { return numCols; }
	int getNumRows() { return numRows; }
	int getCellLength() { return cellLength; }
	SimpleTerrainCell& getCell(int idxX, int idxY);

	void init(int width, int depth, int cellLength);
	float getValue(int x, int y);
	void setValue(int x, int y, float elevation);
	void addValue(int x, int y, float change);
	float getSlope(int x, int y);
	float getSlopeAtCell(int idxX, int idxY);

	void generateMesh();
};

