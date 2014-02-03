#pragma once


#include "LC_Layer.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace LC {

	class VBORenderManager;

	class VBOTerrain {



	public:
		VBOTerrain();
		~VBOTerrain();


		void init(VBORenderManager& rendManager);
		void render(bool editionMode,QVector3D mousePos);
		void updateTerrain(QVector3D pos,float change);

		// edit
		Layer terrainLayer;

	};

} // namespace ucore