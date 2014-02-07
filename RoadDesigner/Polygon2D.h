#pragma once

#include "BBox.h"
#include <vector>
#include <QVector2D>
#include <QVector3D>
#include "float.h"
//#define _USE_MATH_DEFINES
//#include <math.h>

class Loop2D : public std::vector<QVector2D> {
public:
	void close();
	void tessellate(std::vector<Loop2D>& trapezoids) const;
	bool contains(const QVector2D& pt) const;
	void findEdge(float x, float y, int& v1, int& v2, float& s) const;
	static Loop2D createRectangle(float width, float height);
};

class Polygon2D {
protected:
	/** Vector containing 3D points of polygon contour */
	Loop2D contour;
	std::vector<Loop2D> trapezoids;

	QVector2D centroid;

	bool isCentroidValid;
	bool tessellated;

public:
	Polygon2D();
	~Polygon2D();

	/**
	 * Copy constructor.
	 */
	/*
	inline Polygon2D::Polygon2D(const Polygon2D &ref) {	
		contour = ref.contour;
		centroid = ref.centroid;
	}
	*/

	/**
	 * Assignment operator.
	 */
	/*
	inline Polygon2D& operator=(const Polygon2D &ref) {				
		contour = ref.contour;
		centroid = ref.centroid;
		return (*this);
	}
	*/

	const QVector2D& operator[](const int idx) const;
	QVector2D& operator[](const int idx);
	Loop2D& getContour();
	void setContour(const Loop2D& contour);

	void clear();
	void push_back(const QVector2D& point);
	int size() const;

	/** Get center of vertices */
	QVector2D& getCentroid();

	/** Is self intersecting */
	//bool isSelfIntersecting();
	//static bool isSelfIntersecting(const Loop2D& contour);

	/** Only works for polygons with no holes in them */
	//bool splitMeWithPolyline(const std::vector<QVector2D>& pline, Loop2D &pgon1, Loop2D &pgon2);
	//bool splitMeWithPolyline2(const std::vector<QVector2D>& pline, Loop2D &pgon1, Loop2D &pgon2);

	/** Only works for polygons with no holes in them */
	//float computeInset(float offsetDistance, Loop2D &pgonInset, bool computeArea = true);
	//float computeInset(std::vector<float> offsetDistances, Loop2D &pgonInset, bool computeArea = true);
	//float computeArea(bool parallelToXY = false);
	bool reorientFace(bool onlyCheck = false);
	bool contains(const QVector2D& pt);
	//void tessellate(std::vector<Loop2D>& trapezoids) const;
	std::vector<Loop2D>& tessellate();

	bool intersect(const QVector2D& a, const QVector2D& b, float *tab, float *tcd, QVector2D &intPoint) const;

	static bool reorientFace(Loop2D& pface, bool onlyCheck = false);
	void reverse();
	//static int cleanLoop(const Loop2D& pin, Loop2D &pout, float threshold);
	static void transformLoop(const Loop2D& pin, Loop2D& pout, QMatrix4x4& transformMat);
	//static float computeLoopArea(const Loop2D& pin, bool parallelToXY = false);
	//static void sampleTriangularLoopInterior(const Loop2D& pin, std::vector<QVector3D>& pts, float density);
	static QVector2D getLoopAABB(const Loop2D& pin, QVector2D& minCorner, QVector2D& maxCorner);
	BBox getLoopAABB() const;
	static void getLoopOBB(const Loop2D& pin, QVector2D& size, QMatrix4x4& xformMat);
	void getLoopOBB(const QVector2D& dir,  Loop2D& bboxRotLoop) const;
	//void getMyOBB(QVector3D& size, QMatrix4x4& xformMat);
	//static void extrudePolygon(const Loop2D& basePgon, float height, std::vector<Polygon2D>& pgonExtrusion);

	/** Shortest distance from a point to a polygon */
	static float distanceXYToPoint(const Loop2D& pin, const QVector3D& pt);

	/** minimum distance from a loop to another loop (this considers the contour only) */
	static float distanceXYfromContourAVerticesToContourB(const Loop2D& pA, const Loop2D& pB);

	void intersection(Polygon2D &polygon2);
};

