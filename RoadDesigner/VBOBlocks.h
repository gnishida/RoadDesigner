#pragma once

#include <QVector3d>
#include <QDir>
#include "misctools\misctools.h"
#include "Geometry\block.h"
#include "VBOUtil.h"

#ifndef Q_MOC_RUN
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <boost/geometry/multi/multi.hpp>
#include <boost/polygon/polygon.hpp>
#endif

namespace LC {

	class VBORenderManager;

	class VBOBlocks{

	public: 

		static void clearAll(VBORenderManager& rendManager);

		static void generateVBOBlocks(VBORenderManager& rendManager,std::vector< Block > &blocks);
		static void generateVBOOneBlock(VBORenderManager& rendManager,Block &block);
		
		//textures
		static bool initialized;
		static void initVBOBlocks();
		
	};
		
}// namespace LC
