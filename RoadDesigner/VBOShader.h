#pragma once

#include "glew.h"
#include "VBOUtil.h"
#include "qfile.h"
#include <vector>
#include <map>
#include <QHash>

namespace LC {

	class Shader{

	public: 
		static std::vector<uint> programs;
		static std::vector<uint> vss;
		static std::vector<uint> fragments;
		//static std::vector<QHash<QString,uint>> nameToLocation;

		//static std::map<uint,uint> programIdVectorIndex;
		//static std::vector<QHash<QString,GLint>> nameToLocation;

		static uint initShader(QString& vertFileName,QString& fragFileName);
		static void cleanShaders();
		//static GLint getLocation(uint programId,QString name,bool isAtt);

		//static void init();
	};

}

