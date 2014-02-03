#pragma once

#include "Building.h"

namespace LC {

	class VBORenderManager;

	class VBOBuilding {
	public:
		static void generateBuilding(VBORenderManager& rendManager,Building& building);//LC::misctools::Polygon3D& footprint, int numStories);
		static bool bldgInitialized;
		static void initBuildingsTex();
	};

} // namespace LC