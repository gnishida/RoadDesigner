#include <common/Renderable.h>
#include "SimpleTerrain.h"

SimpleTerrainCell& SimpleTerrain::getCell(int idxX, int idxY) {
	if (idxX < 0) idxX = 0;
	if (idxX >= numCols) idxX = numCols - 1;
	if (idxY < 0) idxY = 0; 
	if (idxY >= numRows) idxY = numRows - 1;

	return cells[idxY * numCols + idxX];
}

/**
 * Initialize the terrain with the elevation 0m.
 */
void SimpleTerrain::init(int width, int depth, int cellLength) {
	this->width = width;
	this->depth = depth;
	this->cellLength = cellLength;
	this->numCols = width / cellLength;
	this->numRows = depth / cellLength;

	this->cells.clear();
	for (int i = 0; i < numRows; ++i) {
		for (int j = 0; j < numCols; ++j) {
			SimpleTerrainCell cell = SimpleTerrainCell(this, j, i, j * cellLength - width/2.0f, i * cellLength - depth/2.0f, cellLength);
			this->cells.push_back(cell);
		}
	}
}

float SimpleTerrain::getValue(int x, int y) {
	int idxX = ((float)x + width/2.0f) / cellLength;
	int idxY = ((float)y + depth/2.0f) / cellLength;

	return getCell(idxX, idxY).getElevation(x, y);
}

/**
 * Set the elevation at the specified point.
 * Notice that this function does not recalculate the corners' elevation.
 * The caller is responsible for calling recalcCornersElevation method() if necessary.
 *
 * @param x		X coordinate
 * @param y		Y coordinate
 * @param elevation	new elevation
 */
void SimpleTerrain::setValue(int x, int y, float elevation) {
	int idxX = ((float)x + width/2.0f) / cellLength;
	int idxY = ((float)y + depth/2.0f) / cellLength;

	getCell(idxX, idxY).setElevation(elevation);

	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			getCell(idxX + i, idxY + j).setCornersElevationModified();
		}
	}
}

void SimpleTerrain::addValue(int x, int y, float change) {
	int idxX = ((float)x + width/2.0f) / cellLength;
	int idxY = ((float)y + depth/2.0f) / cellLength;

	getCell(idxX, idxY).addElevation(change);

	//recalcCornersElevation();
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			getCell(idxX + i, idxY + j).setCornersElevationModified();
		}
	}
}

float SimpleTerrain::getSlope(int x, int y) {
	int idxX = ((float)x + width/2.0f) / cellLength;
	int idxY = ((float)y + depth/2.0f) / cellLength;

	return getSlopeAtCell(idxX, idxY);
}

float SimpleTerrain::getSlopeAtCell(int idxX, int idxY) {
	return getCell(idxX, idxY).getSlope();
}

void SimpleTerrain::generateMesh() {
	renderables.clear();

	renderables.push_back(RenderablePtr(new Renderable(GL_QUADS)));

	for (int i = 0; i < numRows; ++i) {
		int y = i * cellLength - depth * 0.5f;
		for (int j = 0; j < numCols; ++j) {
			int x = j * cellLength - width * 0.5f;

			float value = getValue(x, y);
			QColor color = colorTable.getColor(value);

			renderables[0]->addQuad(QVector3D(x, y, getCell(j, i).getElevationSW()), QVector3D(x + cellLength, y, getCell(j, i).getElevationSE()), QVector3D(x + cellLength, y + cellLength, getCell(j, i).getElevationNE()), QVector3D(x, y + cellLength, getCell(j, i).getElevationNW()), getCell(j, i).getNormal(), color);

		}
	}
}
