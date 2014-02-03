#pragma once

#include "misctools\misctools.h"
#include "Geometry\placeTypeInstances.h"


namespace LC {

		class VBORenderManager;

		class VBOGUI{

		public:

			VBOGUI();
			~VBOGUI();
			
			void clearVBOGUI(VBORenderManager& rendManager);
			void updateVBOGUI(VBORenderManager& rendManager,std::vector<PlaceType>& myPlaceTypes,int placeTypeSelInd);
			void renderVBOGUI(VBORenderManager& rendManager);
			//void renderGUIControlPoints(GLuint programId);

		private:
			void addLine(QVector3D pos1,QVector3D pos2,QVector3D color);

			/*// points
			std::vector<std::vector<Vertex>> point_vboVector_Vert;
			std::vector<GLuint> point_vboVector_vboInd;
			std::vector<float> point_vboVector_pointSize;
			std::vector<bool> point_wasModified;*/
			//std::vector<QString> pointStatic;
		
			void addPoint(VBORenderManager& rendManager,QVector3D pos,QVector3D col,float size);
			QSet<QString> pointSizes;
		};
		
}
