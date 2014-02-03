#include "VBOTerrain.h"
#include <QFileInfo>
#include "VBOUtil.h"
#include "qimage.h"
#include <QGLWidget>

#include "VBORenderManager.h"

namespace LC {

	VBOTerrain::VBOTerrain() {
	}

	VBOTerrain::~VBOTerrain() {
	}

	GLuint elementbuffer;
	GLuint vbo;
	GLuint indicesCount;
	GLuint grassText;
	GLuint program;


	void VBOTerrain::init(VBORenderManager& rendManager){

		QVector3D minPos=rendManager.minPos;
		QVector3D maxPos=rendManager.maxPos;
		int resolutionX=10;
		int resolutionY=10;

		/*std::vector<Vertex> sky_Vert;
		float scale=1000.0f;
		QString imgName="data/textures/grass.jpg";
		GLuint grassText=VBOUtil::loadImage(imgName);//mirrored
		float r = 1.0005f;

		sky_Vert.push_back(Vertex(scale*QVector3D(-r,-r,0),QVector3D(0,0,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(r,-r,0),QVector3D(1,0,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(r, r,0),QVector3D(1,1,0)));
		sky_Vert.push_back(Vertex(scale*QVector3D(-r, r,0),QVector3D(0,1,0)));
		rendManager.addGeometry(QString("terrain"),sky_Vert,grassText,true);*/

		//QString imgName="data/textures/grass.jpg";
		//grassText=VBOUtil::loadImage(imgName);//mirrored
		//////////////////
		// TERRAIN LAYER
		terrainLayer.init(minPos,maxPos,resolutionX,resolutionY,0);
		terrainLayer.createRandomDistribution();
		terrainLayer.updateTexture();

		glActiveTexture(GL_TEXTURE7);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D,terrainLayer.texData); 
		glActiveTexture(GL_TEXTURE0);
		//////////////////
		// TEXTURE 3D
		int numLevels=4;
		glEnable(GL_TEXTURE_2D_ARRAY);	
		glTexParameterf (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf (GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_TRUE);
		for(int i=0;i<numLevels;i++){
			QString fileName="data/textures/0"+QString::number(i+1)+"_terrain.jpg";
			QImage img;
			if( ! img.load( fileName ) ){
				printf("ERROR: loading %s\n",fileName.toAscii().constData());
				return;
			}

			if(i==0){
				glGenTextures(1,&grassText);
				glBindTexture(GL_TEXTURE_2D_ARRAY,grassText);
				//glBindTexture(GL_TEXTURE_3D,grassText);
				//Allocate the storage.
				glTexStorage3D(GL_TEXTURE_2D_ARRAY, 4, GL_RGBA8, img.width(), img.height(), numLevels);
				//glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, img.width(), img.height(), numLevels);
			}

			QImage GL_formatted_image;
			GL_formatted_image = QGLWidget::convertToGLFormat(img);
			if( GL_formatted_image.isNull() ){
				printf("ERROR: GL_formatted_image\n");
				return;
			}
			printf("img[%d] %d %d\n",i,img.width(), img.height());
			//Upload pixel data.
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, img.width(), img.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, GL_formatted_image.bits());
			//glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, i, img.width(), img.height(), 1, GL_RGBA, GL_UNSIGNED_BYTE, GL_formatted_image.bits());
		}
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		//glGenerateMipmap(GL_TEXTURE_3D);
		//////////////////
		// VERTICES

		program=rendManager.program;

		/// resolution of vertices double than painting
		resolutionX*=10;//3
		resolutionY*=10;
		std::vector<Vertex> vert;
		float sideX=abs(maxPos.x()-minPos.x())/resolutionX;
		float sideY=abs(maxPos.y()-minPos.y())/resolutionY;

		/*QString fileName="data/layers/0_elevation.png";
		QImage img;
		if( ! img.load( fileName ) ){
			printf("ERROR: loading %s\n",fileName.toAscii().constData());
			return;
		}*/
		
		// VERTEX
		//stackoverflow.com/questions/5915753/generate-a-plane-with-triangle-strips
		vert.resize((resolutionX+1)*(resolutionY+1));
		int count=0;
		//for (int row=0; row<resolutionY+1; row++ ) {
		//	for (int col=0; col<resolutionX+1; col++ ) {
		int width=resolutionX+1;
		int height=resolutionY+1;
		float maxVertexHeight=0;
		for ( int row=0; row<height; row++ ) {
			for ( int col=0; col<width; col++ ) {
				QVector3D pos=QVector3D(col*sideX,row*sideY,0);
				pos+=minPos;
				//if(row!=0&&row!=height-1&&col!=0&&col!=width-1)
				//	pos+=QVector3D(0,0,100.0f*((float)qrand()/RAND_MAX));
				/*QRgb color=img.pixel(((float)col/resolutionX)*img.width(),((float)row/resolutionY)*img.height());
				float vertexHeight=qGray(color);
				maxVertexHeight=maxVertexHeight>vertexHeight?maxVertexHeight:vertexHeight;
				pos+=QVector3D(0,0,vertexHeight);*/
				//pos+=QVector3D(0,0,terrainLayer.sample(pos)+20);
				vert[count]=Vertex(pos,pos/100.0f);
				//printf("Col %d Row %d [%d]: %f %f\n",col,row,count,vert[count][0],vert[count][1]);
				count++;
			}
		}
		printf("max vertex height %f\n",maxVertexHeight);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vert.size(), vert.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// INDEX
		std::vector<ushort> indices;
		/*for ( int row=0; row<height-1; row++ ) {
			if ( (row&1)==0 ) { // even rows
				for ( int col=0; col<width; col++ ) {
					indices.push_back( col + row * width);
					indices.push_back(col + (row+1) * width);
				}
			} else { // odd rows
				for ( int col=width-1; col>0; col-- ) {
					indices.push_back(col + (row+1) * width);
					indices.push_back(col - 1 + + row * width);
				}
			}
		}
		if ( (height&1) && height>2 ) {
			indices.push_back((height-1) * width);
		}*/
		for ( int row=0; row<height-1; row++ ) {
			for ( int col=0; col<width-1; col++ ) {
				indices.push_back(col+row*width);
				indices.push_back((col+1)+row*width);
				indices.push_back((col+1)+(row+1)*width);
				indices.push_back((col)+(row+1)*width);
			}
		}
		/*for ( int row=height-1; row>0; row-- ) {
			for ( int col=width-1; col>0; col-- ) {
				indices.push_back(col+row*width);
				indices.push_back((col-1)+row*width);
				indices.push_back((col-1)+(row-1)*width);
				indices.push_back((col)+(row-1)*width);
			}
		}*/


		printf("Num %d -> %d\n",indices.size(),(width*height) + (width-1)*(height-2));
		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(ushort), &indices[0], GL_STATIC_DRAW);
		indicesCount=indices.size();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}//


	void VBOTerrain::render(bool editionMode,QVector3D mousePos){
		//////////////////////////////////////////
		// TERRAIN
		//glCullFace(GL_FRONT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		GLuint vao;
		glGenVertexArrays(1,&vao); 
		glBindVertexArray(vao); 
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D,0); 
		glBindTexture(GL_TEXTURE_2D_ARRAY, grassText);
		//glBindTexture(GL_TEXTURE_3D, grassText);
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, grassText);
		glUniform1i (glGetUniformLocation (program, "mode"), 3);//MODE: terrain
		glUniform1i (glGetUniformLocation (program, "tex_3D"), 8);//tex0: 0

		glUniform1i (glGetUniformLocation (program, "terrain_tex"), 7);//tex0: 0
		glUniform4f (glGetUniformLocation (program, "terrain_size"), 
			terrainLayer.minPos.x(),
			terrainLayer.minPos.y(),
			(terrainLayer.maxPos.x()-terrainLayer.minPos.x()), 
			(terrainLayer.maxPos.y()-terrainLayer.minPos.y())
		 );//tex0: 0

		printf("tex_3D %d\n",glGetUniformLocation (program, "tex_3D"));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(3*sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(6*sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(9*sizeof(float)));

		// Draw the triangles !
		/*glDrawElements(
			GL_TRIANGLE_STRIP, // mode
			indicesCount,    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
			);*/
		glDrawElements(
			GL_QUADS, // mode
			indicesCount,    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
			);

		 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		 glBindBuffer(GL_ARRAY_BUFFER, 0);
		 glCullFace(GL_BACK);
		 glEnable(GL_CULL_FACE);
		 glBindVertexArray(0);
		glDeleteVertexArrays(1,&vao);

		 //////////////////////
		 // EDITION
		 if(editionMode==true){
			 glLineWidth(5.0f);
			 glPointSize(10.0f);

			 VBOUtil::check_gl_error(">>editionMode");
			 printf("**************** editionMode %d\n",terrainLayer.samplePosition.size());
			const float pointOffset=5.0f;
			GLuint vao;
			glGenVertexArrays(1,&vao);
			glBindVertexArray(vao);
			GLuint edPointsVBO;
			glGenBuffers(1, &edPointsVBO);
			glBindBuffer(GL_ARRAY_BUFFER, edPointsVBO);
			
			std::vector<Vertex> points;
			int numPointsCircle=20;
			points.resize( terrainLayer.samplePosition.size()+numPointsCircle);
			// 1. sample points
			for(int i=0;i<points.size();i++){
				QVector3D pos=terrainLayer.samplePosition[i];
				//pos.setZ(2.55f*(terrainLayer.sample(terrainLayer.samplePosition[i]))+pointOffset);
				pos.setZ(pointOffset);
				points[i]=Vertex(pos.x(),pos.y(),pos.z(),1.0,0,0,0,0,0,0,0,0);//(&pos,new QVector3D(1.0f,0,0));//pos color
				
			}
			// 2. closest
			int closestSample=terrainLayer.closestSample(mousePos);
			// circle
			float radius=std::max(terrainLayer.sideX,terrainLayer.sideY);
			int degreesSegmentCircle=360.0f/numPointsCircle;
			for (int i=0; i < numPointsCircle; i++){
				float degInRad = i*degreesSegmentCircle*0.0174532925f;//degree to rad
				QVector3D pos(cos(degInRad)*radius,sin(degInRad)*radius,300.0f);
				pos+=terrainLayer.samplePosition[closestSample];
				//pos.setZ(2.55f*(terrainLayer.sample(pos))+pointOffset*2.0f);//terrain z
				pos.setZ(pointOffset*2.0f);
				points[terrainLayer.samplePosition.size()+i]=Vertex(pos.x(),pos.y(),pos.z(),0,0,1.0f,0,0,0,0,0,0);
			}


			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*points.size(), points.data(), GL_DYNAMIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
			glEnableVertexAttribArray(1);
			VBOUtil::check_gl_error("aa editionMode");
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(3*sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(6*sizeof(float)));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(9*sizeof(float)));
			glUniform1i (glGetUniformLocation (program, "mode"), 1|LC::mode_AdaptTerrain);//MODE: color

			//draw points
			glDrawArrays(GL_POINTS,0,points.size()-numPointsCircle);
			//draw circle
			glDrawArrays(GL_LINE_LOOP,points.size()-numPointsCircle,numPointsCircle);

			glDeleteBuffers(1,&edPointsVBO);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			glDeleteVertexArrays(1,&vao);

			
			printf("****************\n");
		 }
	}//

	void VBOTerrain::updateTerrain(QVector3D pos,float change){
		terrainLayer.updateLayer(pos,change);
		glActiveTexture(GL_TEXTURE7);

		glBindTexture(GL_TEXTURE_2D,terrainLayer.texData); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glActiveTexture(GL_TEXTURE0);
	}//

} // namespace LC