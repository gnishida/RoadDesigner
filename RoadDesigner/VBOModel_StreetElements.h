#pragma once

#include <vector>

#include "misctools\misctools.h"
#include "Geometry\block.h"
#include "VBOModel.h"

namespace LC {


		class VBOModel_StreetElements{

		public:

			static void clearVBOModel_StreetElements();
			static void initStreetElements();

			static void renderOneStreetElement(int programId,ModelSpec& strEleStr);

			static std::vector<VBOModel> streetElementsModels;
			static bool streetElements_wasInitialized;
		};
		
} // namespace LC
