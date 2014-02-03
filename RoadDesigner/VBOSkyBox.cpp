#include "VBOSkyBox.h"
#include <QFileInfo>
#include "VBOUtil.h"

#include "VBORenderManager.h"

namespace LC {

	VBOSkyBox::VBOSkyBox() {
		//initialized=0;
	}

	VBOSkyBox::~VBOSkyBox() {
	}

	void VBOSkyBox::init(VBORenderManager& rendManager){
		float scale=5000.0f;
		QString imgName="data/sky/skyCombined.png";
		/*// 1. load textures
		QFileInfo commbinedImg(imgName);
		if(!commbinedImg.exists()){
			printf("ERORR: SkyBox images were not found at data/sky/skyCombined.png");
			initialized=2;//error loading
			return;
		}
		//glActiveTexture(GL_TEXTURE8);
		skyboxTex=VBOUtil::loadImage(imgName);//mirrored*/
		//glBindTexture(GL_TEXTURE_2D, skyboxTex);
		//glActiveTexture(GL_TEXTURE0);
		//GLenum err;
		//while ((err = glGetError()) != GL_NO_ERROR) qDebug() << "**Init SkyBox ERROR INIT: OpenGL-->" << err << endl;
		VBOUtil::check_gl_error("Init SkyboxVBO");

		float r = 1.0005f; // If you have border issues change this to 1.005f
		// Common Axis X - LEFT side
		std::vector<Vertex> sky_Vert;
		sky_Vert.push_back(Vertex(scale*QVector3D(-1.0f,-r,-r),QVector3D(0.0f,2/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(-1.0f, r,-r),QVector3D(0.5f,2/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(-1.0f, r, r),QVector3D(0.5f,1.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(-1.0f,-r, r),QVector3D(0.0f,1.0f,0)));	

		// Common Axis Z - FRONT side
	
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,1.0f,-r),QVector3D(0.5f,2/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D( r,1.0f,-r),QVector3D(1.0f,2/3.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D( r,1.0f, r),QVector3D(1.0f,1.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,1.0f, r),QVector3D(0.5f,1.0f,0)));

		// Common Axis X - Right side

		sky_Vert.push_back(Vertex(scale*QVector3D(1.0f, r,-r),QVector3D(0.0f,1/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(1.0f,-r,-r),QVector3D(0.5f,1/3.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D(1.0f,-r, r),QVector3D(0.5f,2/3.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(1.0f, r, r),QVector3D(0.0f,2/3.0f,0)));

		// Common Axis Z - BACK side
	
		sky_Vert.push_back(Vertex(scale*QVector3D( r,-1.0f,-r),QVector3D(0.5f,1/3.0f,0)));	
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,-1.0f,-r),QVector3D(1.0f,1/3.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,-1.0f, r),QVector3D(1.0f,2/3.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D( r,-1.0f, r),QVector3D(0.5f,2/3.0f,0)));

		// Common Axis Y - Draw Up side
	
		sky_Vert.push_back(Vertex(scale*QVector3D(-r, r,1.0f),QVector3D(0.0f,0.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D( r, r,1.0f),QVector3D(0.5f,0.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D( r,-r,1.0f),QVector3D(0.5f,1/3.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,-r,1.0f),QVector3D(0.0f,1/3.0f,0)));


		// Common Axis Y - Draw Down side
	
		sky_Vert.push_back(Vertex(scale*QVector3D(-r,-r,-1.0f),QVector3D(0.5f,0.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D( r,-r,-1.0f),QVector3D(1.0f,0.0f,0))); 
		sky_Vert.push_back(Vertex(scale*QVector3D( r, r,-1.0f),QVector3D(1.0f,1/3.0f,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(-r, r,-1.0f),QVector3D(0.5f,1/3.0f,0)));

		//rendManager.createVAO(sky_Vert,skyVBO,skyVAO,numVertex);
		rendManager.addStaticGeometry(QString("sky"),sky_Vert,imgName,GL_QUADS,2);// texture not lighting

		/*glGenVertexArrays(1, &skyVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*sky_Vert.size(), sky_Vert.data(), GL_STATIC_DRAW);
		//while ((err = glGetError()) != GL_NO_ERROR) qDebug() << "SKY BOX ERROR 4ERROR INIT: OpenGL-->" << err << endl;
		RendUtil::check_gl_error("Init SkyboxVBO2");*/
		//initialized=1;
		printf("Loading SkyBox images... loaded %d\n",sky_Vert.size());


		//initialized=1;
	}//

	/*void VBOSkyBox::render(VBORenderManager& rendManager) {
		if(initialized==2)
			return;

		if(initialized==0){
			init(rendManager);
		}
		glUniform1i (glGetUniformLocation (rendManager.program, "mode"), 2);//MODE: tex
		glUniform1i (glGetUniformLocation (rendManager.program, "tex0"), 8);//tex0: 8

		glBindVertexArray(skyVAO);
		glDrawArrays(GL_QUADS,0,numVertex);
		glBindVertexArray(0);
	}*/

} // namespace LC