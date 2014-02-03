#pragma once

#include "misctools\misctools.h"
#include "RoadGraph\roadGraph.h"
#include "VBOUtil.h"



namespace LC {

		class VBORenderManager;
		
		class VBORoadGraph{

		public:

			VBORoadGraph();
			~VBORoadGraph();
			
			void clearVBORoadGraph(VBORenderManager& rendManager);
			void updateRoadGraph(VBORenderManager& rendManager,RoadGraph &roadGraph);
			void renderRoadGraphLines(VBORenderManager& rendManager);
		

		private:
			
			/*// Line
			bool line_wasModified;
			GLuint linesVBO;
			
			std::vector<Vertex> line_Vert;*/
		};
		
}// namespace LC
