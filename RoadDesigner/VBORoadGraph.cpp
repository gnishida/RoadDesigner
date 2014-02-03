/************************************************************************************************
*
*		@desc Class containing the road graph
*		@author igarciad
*
************************************************************************************************/

#include "VBORoadGraph.h"

#include "VBORenderManager.h"

namespace LC {

	VBORoadGraph::VBORoadGraph(){
		//linesVBO=INT_MAX;
	}//

	VBORoadGraph::~VBORoadGraph(){
		//clearVBORoadGraph();
	}//

	void VBORoadGraph::clearVBORoadGraph(VBORenderManager& rendManager){
		/*if(linesVBO!=INT_MAX){
			glDeleteBuffers(1, &linesVBO);
			linesVBO=INT_MAX;
		}
		line_Vert.clear();*/
		rendManager.removeStaticGeometry("roads");
	}//


	void VBORoadGraph::updateRoadGraph(VBORenderManager& rendManager,RoadGraph &roadGraph){
		//////////////////////////////////////////
		// LINES
		clearVBORoadGraph(rendManager);
		RoadGraph::roadGraphEdgeIter ei, eiEnd;
		QVector3D p0,p1;
		int numEdges=0;
		/*for(boost::tie(ei, eiEnd) = boost::edges(roadGraph.myRoadGraph);
			ei != eiEnd; ++ei)
		{
			numEdges++;
			QVector3D col;
			if( roadGraph.myRoadGraph[*ei].numberOfLanes==LC::misctools::Global::global()->cuda_arterial_numLanes){//.isArterial() ){
				glLineWidth(5.0f);
				col=QVector3D(1.0f, 0.99f, 0.54f);//yellow
			} else {
				glLineWidth(2.0f);
				col=QVector3D(0.9f, 0.9f, 0.9f);//white	
			}


			p0 = roadGraph.myRoadGraph[boost::source(*ei, roadGraph.myRoadGraph)].pt;
			p1 = roadGraph.myRoadGraph[boost::target(*ei, roadGraph.myRoadGraph)].pt;
			p0.setZ(1.0f);
			p1.setZ(1.0f);
			
			if(true){
				QVector3D dir=p1-p0;
				float length=(dir).length();
				dir/=length;
				int numSegments=ceil(length/5.0f);
				if(numSegments<2){
					line_Vert.push_back(Vertex(p0.x(),p0.y(),p0.z(),col.x(),col.y(),col.z(),0,0,0,0,0,0));
					line_Vert.push_back(Vertex(p1.x(),p1.y(),p1.z(),col.x(),col.y(),col.z(),0,0,0,0,0,0));
				}else{
					QVector3D p0_,p1_;
					p0_=p0;
					for(int sN=0;sN<numSegments;sN++){
						float lenghToMove=5.0f;
						if(length<10)
							lenghToMove=length;
						p1_=p0_+dir*lenghToMove;
						length-=lenghToMove;
						line_Vert.push_back(Vertex(p0_.x(),p0_.y(),p0_.z(),col.x(),col.y(),col.z(),0,0,0,0,0,0));
						line_Vert.push_back(Vertex(p1_.x(),p1_.y(),p1_.z(),col.x(),col.y(),col.z(),0,0,0,0,0,0));
						p0_=p1_;
					}

				}
			}else{
				//RendUtil::addLine(line_Vert,p0,p1,col);
				line_Vert.push_back(Vertex(p0.x(),p0.y(),p0.z(),col.x(),col.y(),col.z(),0,0,0,0,0,0));
				line_Vert.push_back(Vertex(p1.x(),p1.y(),p1.z(),col.x(),col.y(),col.z(),0,0,0,0,0,0));
			}
			
		}*/

		//////////////////////////////////////////
		// LINES QUADS

		std::vector<Vertex> vertROAD;
		std::vector<Vertex> vertINT;
		QVector3D a0,a1,a2,a3;
		float width=3.5f;
		for(boost::tie(ei, eiEnd) = boost::edges(roadGraph.myRoadGraph);
			ei != eiEnd; ++ei)
		{
			numEdges++;
			QVector3D col;
			if( roadGraph.myRoadGraph[*ei].numberOfLanes==LC::misctools::Global::global()->cuda_arterial_numLanes){//.isArterial() ){
				glLineWidth(5.0f);
				col=QVector3D(1.0f, 0.99f, 0.54f);//yellow
			} else {
				glLineWidth(2.0f);
				col=QVector3D(0.9f, 0.9f, 0.9f);//white	
			}


			p0 = roadGraph.myRoadGraph[boost::source(*ei, roadGraph.myRoadGraph)].pt;
			p1 = roadGraph.myRoadGraph[boost::target(*ei, roadGraph.myRoadGraph)].pt;
			p0.setZ(1.0f);
			p1.setZ(1.0f);


			QVector3D dir=(p1-p0);//.normalized();
			float length=dir.length();
			dir/=length;
			QVector3D per=(QVector3D::crossProduct(dir,QVector3D(0,0,1.0f)).normalized());

			int numSeg=0;
			const float intersectionClearance=3.5f;

			/// no room for one line
			if(length<2.0f*intersectionClearance){//no real line
				a0=QVector3D(p0.x()+per.x()*width, p0.y()+per.y()*width, p0.z());
				a1=QVector3D(p1.x()+per.x()*width, p1.y()+per.y()*width, p0.z());

				a2=QVector3D(p1.x()-per.x()*width, p1.y()-per.y()*width, p0.z());
				a3=QVector3D(p0.x()-per.x()*width, p0.y()-per.y()*width, p0.z());

				vertINT.push_back(Vertex(a0,QVector3D(),QVector3D(0,0,1.0f),a0));
				vertINT.push_back(Vertex(a1,QVector3D(),QVector3D(0,0,1.0f),a1));
				vertINT.push_back(Vertex(a2,QVector3D(),QVector3D(0,0,1.0f),a2));
				vertINT.push_back(Vertex(a3,QVector3D(),QVector3D(0,0,1.0f),a3));
			}else{
				QVector3D p0_,p1_;
				// START
				p0_=p0;
				p1_=p0+dir*intersectionClearance;
				a0=QVector3D(p0_.x()+per.x()*width, p0_.y()+per.y()*width, p0_.z());
				a1=QVector3D(p1_.x()+per.x()*width, p1_.y()+per.y()*width, p0_.z());

				a2=QVector3D(p1_.x()-per.x()*width, p1_.y()-per.y()*width, p0_.z());
				a3=QVector3D(p0_.x()-per.x()*width, p0_.y()-per.y()*width, p0_.z());

				vertINT.push_back(Vertex(a0,QVector3D(),QVector3D(0,0,1.0f),a0));
				vertINT.push_back(Vertex(a1,QVector3D(),QVector3D(0,0,1.0f),a1));
				vertINT.push_back(Vertex(a2,QVector3D(),QVector3D(0,0,1.0f),a2));
				vertINT.push_back(Vertex(a3,QVector3D(),QVector3D(0,0,1.0f),a3));

				// MIDDLE
				
				float middLenght=length-2.0f*intersectionClearance;
				float const maxSegmentLeng=5.0f;
				float segmentLeng;
				if(middLenght>2*maxSegmentLeng){
					int numSegments=ceil(length/5.0f);
					float lengthMoved=0;
					float dW=(2*width);
					for(int nS=0;nS<numSegments;nS++){
						segmentLeng=std::min(maxSegmentLeng,middLenght);
						a0=a1;
						a3=a2;
						a1=a1+dir*(segmentLeng);
						a2=a2+dir*(segmentLeng);
						vertROAD.push_back(Vertex(a0,QVector3D(),QVector3D(0,0,1.0f),QVector3D(0,lengthMoved/dW,0.0f)));
						vertROAD.push_back(Vertex(a1,QVector3D(),QVector3D(0,0,1.0f),QVector3D(0,(segmentLeng+lengthMoved)/dW,0)));
						vertROAD.push_back(Vertex(a2,QVector3D(),QVector3D(0,0,1.0f),QVector3D(1.0f,(segmentLeng+lengthMoved)/dW,0)));
						vertROAD.push_back(Vertex(a3,QVector3D(),QVector3D(0,0,1.0f),QVector3D(1.0f,lengthMoved/dW,0)));
						lengthMoved+=segmentLeng;
						middLenght-=segmentLeng;
					}

				}else{
					// JUST ONE MIDDLE SEGMENT
					a0=a1;
					a3=a2;
					a1=a1+dir*(middLenght);
					a2=a2+dir*(middLenght);

					vertROAD.push_back(Vertex(a0,QVector3D(),QVector3D(0,0,1.0f),QVector3D(0,0.0f,0)));
					vertROAD.push_back(Vertex(a1,QVector3D(),QVector3D(0,0,1.0f),QVector3D(0,middLenght/(2*width),0)));
					vertROAD.push_back(Vertex(a2,QVector3D(),QVector3D(0,0,1.0f),QVector3D(1.0f,middLenght/(2*width),0)));
					vertROAD.push_back(Vertex(a3,QVector3D(),QVector3D(0,0,1.0f),QVector3D(1.0f,0.0f,0)));
				}
				// END
				a0=a1;
				a3=a2;

				a1=a1+dir*intersectionClearance;
				a2=a2+dir*intersectionClearance;

				vertINT.push_back(Vertex(a0,QVector3D(),QVector3D(0,0,1.0f),a0));
				vertINT.push_back(Vertex(a1,QVector3D(),QVector3D(0,0,1.0f),a1));
				vertINT.push_back(Vertex(a2,QVector3D(),QVector3D(0,0,1.0f),a2));
				vertINT.push_back(Vertex(a3,QVector3D(),QVector3D(0,0,1.0f),a3));

			}
		}

		rendManager.addStaticGeometry("roads",vertINT,"data/textures/street_1.png",GL_QUADS, 2|LC::mode_AdaptTerrain);
		rendManager.addStaticGeometry("roads",vertROAD,"data/textures/street_0.png",GL_QUADS,2|LC::mode_AdaptTerrain);

		/*std::vector<Vertex> vert;
		for(boost::tie(ei, eiEnd) = boost::edges(roadGraph.myRoadGraph);
			ei != eiEnd; ++ei)
		{
			numEdges++;
			QVector3D col;
			if( roadGraph.myRoadGraph[*ei].numberOfLanes==LC::misctools::Global::global()->cuda_arterial_numLanes){//.isArterial() ){
				glLineWidth(5.0f);
				col=QVector3D(1.0f, 0.99f, 0.54f);//yellow
			} else {
				glLineWidth(2.0f);
				col=QVector3D(0.9f, 0.9f, 0.9f);//white	
			}


			p0 = roadGraph.myRoadGraph[boost::source(*ei, roadGraph.myRoadGraph)].pt;
			p1 = roadGraph.myRoadGraph[boost::target(*ei, roadGraph.myRoadGraph)].pt;
			p0.setZ(1.0f);
			p1.setZ(1.0f);


			QVector3D dir=(p1-p0);//.normalized();
			float length=dir.length();
			dir/=length;
			QVector3D per=(QVector3D::crossProduct(dir,QVector3D(0,0,1.0f)).normalized());

		
			a0=QVector3D(p0.x()+per.x()*width, p0.y()+per.y()*width, p0.z());
			a1=QVector3D(p1.x()+per.x()*width, p1.y()+per.y()*width, p0.z());

			a2=QVector3D(p1.x()-per.x()*width, p1.y()-per.y()*width, p0.z());
			a3=QVector3D(p0.x()-per.x()*width, p0.y()-per.y()*width, p0.z());

			vert.push_back(Vertex(a0,QVector3D(),QVector3D(0,0,1.0f),a0));
			vert.push_back(Vertex(a1,QVector3D(),QVector3D(0,0,1.0f),a1));
			vert.push_back(Vertex(a2,QVector3D(),QVector3D(0,0,1.0f),a2));
			vert.push_back(Vertex(a3,QVector3D(),QVector3D(0,0,1.0f),a3));

		}
		
		rendManager.addStaticGeometry("roads",vert,"data/textures/as1.jpg");*/

		//line_wasModified=true;
	}//


	void VBORoadGraph::renderRoadGraphLines(VBORenderManager& rendManager){
	
		/*//glLineWidth(6.0f);
		glLineWidth(6.0f);
		GLuint vao;
			glGenVertexArrays(1,&vao);
			glBindVertexArray(vao);
		//
		if(line_wasModified==true){// it was modified
			if(linesVBO!=INT_MAX){//it was already created--> remove
				glDeleteVertexArrays(1, &linesVBO);
			}
			glGenVertexArrays(1,&linesVBO);
			glBindBuffer(GL_ARRAY_BUFFER,linesVBO);
			glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex)*line_Vert.size(),line_Vert.data(),GL_STATIC_DRAW);
			line_wasModified=false;
		}else{
			glBindBuffer(GL_ARRAY_BUFFER,linesVBO);
		}
		
		glUniform1i (glGetUniformLocation (rendManager.program, "terrain_tex"), 7);//tex0: 0
		glUniform4f (glGetUniformLocation (rendManager.program, "terrain_size"), 
			rendManager.vboTerrain.terrainLayer.minPos.x(),
			rendManager.vboTerrain.terrainLayer.minPos.y(),
			(rendManager.vboTerrain.terrainLayer.maxPos.x()-rendManager.vboTerrain.terrainLayer.minPos.x()), 
			(rendManager.vboTerrain.terrainLayer.maxPos.y()-rendManager.vboTerrain.terrainLayer.minPos.y())
		 );//tex0: 0
		
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),0);
			glEnableVertexAttribArray(1);
			VBOUtil::check_gl_error("aa editionMode");
			glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(3*sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(6*sizeof(float)));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(9*sizeof(float)));
			glUniform1i (glGetUniformLocation (rendManager.program, "mode"), 1|LC::mode_AdaptTerrain);//MODE: color


		//RendUtil::enableAttributesInShader(programId);
		glDrawArrays(GL_LINES,0,line_Vert.size());
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(0);
		glDeleteVertexArrays(1,&vao);*/
	}//

}