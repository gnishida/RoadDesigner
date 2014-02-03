#pragma once

#include "glew.h"

namespace LC {

	class VBORenderManager;

	class VBOSkyBox {

	public:
		VBOSkyBox();
		~VBOSkyBox();

		//void render(VBORenderManager& rendManager);
		void init(VBORenderManager& rendManager);
	/*protected:
		GLuint skyVBO;
		GLuint skyVAO;
		GLuint skyboxTex;
		unsigned int numVertex;
		int initialized;*/

	};

} // namespace ucore