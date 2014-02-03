#pragma once

#include "misctools/misctools.h"
#include "Geometry/block.h"
#include "Geometry/placeTypeInstances.h"
#include "VBOModel_StreetElements.h"

namespace LC {

	class VBORenderManager;

	class VBOVegetation
	{
	public:
		VBOVegetation(void);

		~VBOVegetation(void);

		static bool generateVegetation(VBORenderManager& rendManager,PlaceTypesMainClass &placeTypesIn,std::vector< Block > &blocks);

	};

}// namespace LC
