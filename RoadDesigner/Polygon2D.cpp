#include "Polygon2D.h"
#include "Util.h"
#include <QVector2D>
#include <QMatrix4x4>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/ring.hpp>
#include <boost/geometry/multi/multi.hpp>
#include <boost/polygon/polygon.hpp>

void Loop2D::close() {
	if (size() == 0) return;

	if (this->at(0) != this->at(size() - 1)) {
		push_back(this->at(0));
	}
}

void Loop2D::tessellate(std::vector<Loop2D>& trapezoids) const {
	trapezoids.clear();

	if (size() < 3) return;

	// create 2D polygon data
	std::vector<boost::polygon::point_data<double> > polygon;
	polygon.resize(size());
	for (int i = 0; i < size(); i++) {
		polygon[i] = boost::polygon::construct<boost::polygon::point_data<double> >(at(i).x(), at(i).y());
	}

	// create 2D polygon with holes data
	boost::polygon::polygon_with_holes_data<double> temp;
	boost::polygon::set_points(temp, polygon.begin(), polygon.end());

	// create 2D polygon set
	boost::polygon::polygon_set_data<double> polygonSet;
	polygonSet.insert(temp);

	// tessellation
	std::vector<boost::polygon::polygon_with_holes_data<double> > results;
	polygonSet.get_trapezoids(results);

	for (int i = 0; i < results.size(); i++) {
		boost::polygon::polygon_with_holes_data<double>::iterator_type it = results[i].begin();
		Loop2D trapezoid;
		Loop2D tex_coord;
		while (it != results[i].end()) {
			float z = 0.0f;
			bool done = false;

			trapezoid.push_back(QVector2D((*it).x(), (*it).y()));
			it++;
		}

		if (trapezoid.size() < 3) continue;

		Polygon2D::reorientFace(trapezoid);

		// The resulting polygon is usually closed, so remove the last point.
		if ((trapezoid[trapezoid.size() - 1] - trapezoid[0]).lengthSquared() == 0) {
			trapezoid.pop_back();
		}

		if (trapezoid.size() >= 3) trapezoids.push_back(trapezoid);
	}
}

bool Loop2D::contains(const QVector2D& pt) const {
	for (int i = 0; i < size(); ++i) {
		QVector2D vec1 = at((i + 1) % size()) - at(i);
		QVector2D vec2 = pt - at(i);
		if (vec1.x() * vec2.y() - vec1.y() * vec2.x() > 0) return false;
	}

	return true;
}

void Loop2D::findEdge(float x, float y, int& v1, int& v2, float& s) const {
	float minDist = (std::numeric_limits<float>::max)();

	for (int i = 0; i < size(); i++) {
		int next = (i + 1) % size();

		float dist = Util::pointSegmentDistanceXY(at(i), at(next), QVector3D(x, y, 0));
		if (dist < minDist) {
			minDist = dist;
			v1 = i;
			v2 = next;
			if (fabs(at(next).x() - at(i).x()) > fabs(at(next).y() - at(i).y())) {
				s = (x - at(i).x()) / (at(next).x() - at(i).x());
			} else {
				if (at(next).y() - at(i).y() != 0.0f) {
					s = (y - at(i).y()) / (at(next).y() - at(i).y());                                
				} else {
					s = 0.0f;
				}
			}
		}
	}
}

Loop2D Loop2D::createRectangle(float width, float height) {
	Loop2D rectangle;
	rectangle.push_back(QVector2D(-width/2.0f, -height/2.0f));
	rectangle.push_back(QVector2D(width/2.0f, -height/2.0f));
	rectangle.push_back(QVector2D(width/2.0f, height/2.0f));
	rectangle.push_back(QVector2D(-width/2.0f, height/2.0f));

	return rectangle;
}

Polygon2D::Polygon2D() {
	isCentroidValid = false;
	tessellated = false;
	//centroid = QVector3D(FLT_MAX, FLT_MAX, FLT_MAX);
}

Polygon2D::~Polygon2D() {
}

const QVector2D& Polygon2D::operator[](const int idx) const {	
	return contour[idx];
}

QVector2D& Polygon2D::operator[](const int idx) {	
	return contour[idx];
}

Loop2D& Polygon2D::getContour() {
	return contour;
}

void Polygon2D::setContour(const Loop2D& contour) {
	this->contour = contour;

	// normalVec and centroid are to be updated later when they are requested
	isCentroidValid = false;
	tessellated = false;
}

void Polygon2D::clear() {
	contour.clear();
	isCentroidValid = false;
	tessellated = false;
	//centroid = QVector3D(FLT_MAX, FLT_MAX, FLT_MAX);
}

void Polygon2D::push_back(const QVector2D& point) {
	contour.push_back(point);

	// normalVec and centroid are to be updated later when they are requested
	isCentroidValid = false;
	tessellated = false;
}

int Polygon2D::size() const {
	return contour.size();
}

/**
 * Get center of vertices
 */
QVector2D& Polygon2D::getCentroid() {
	if (isCentroidValid) {
		return centroid;
	} else {
		QVector2D newCentroid(0.0f, 0.0f);
		int cSz = contour.size();
		for (int i = 0; i < cSz; ++i) {
			newCentroid = newCentroid + contour[i];
		}
		if (cSz > 0) {
			centroid = (newCentroid/cSz);						
		}			
		isCentroidValid = true;

		return centroid;
	}
}

/*bool Polygon2D::isSelfIntersecting() {
	return isSelfIntersecting(this->contour);
}

bool Polygon2D::isSelfIntersecting(const Loop2D& contour) {
	boost::geometry::ring_type<Polygon2D>::type bg_pgon;
	boost::geometry::assign(bg_pgon, contour);
	boost::geometry::correct(bg_pgon);
	return boost::geometry::intersects(bg_pgon);
}*/

/**
 * Only works for polygons with no holes in them
 */
/*bool Polygon2D::splitMeWithPolyline(const std::vector<QVector2D>& pline, Loop2D &pgon1, Loop2D &pgon2) {
	bool polylineIntersectsPolygon = false;

	size_t plineSz = pline.size();
	size_t contourSz = this->contour.size();

	if(plineSz < 2 || contourSz < 3){
		//std::cout << "ERROR: Cannot split if polygon has fewer than three vertices of if polyline has fewer than two points\n.";
		return false;
	}

	QVector2D tmpIntPt;
	QVector3D firstIntPt;
	QVector3D secondIntPt;
	float tPline, tPgon;
	int firstIntPlineIdx    = -1;
	int secondIntPlineIdx   = -1;
	int firstIntContourIdx  = -1;
	int secondIntContourIdx = -1;
	int intCount = 0;

	//iterate along polyline
	for (int i = 0; i < plineSz - 1; ++i) {
		int iNext = i+1;

		for (int j = 0; j < contourSz; ++j) {
			int jNext = (j+1)%contourSz;

			if (Util::segmentSegmentIntersectXY(QVector2D(pline[i]), QVector2D(pline[iNext]), QVector2D(contour[j]), QVector2D(contour[jNext]), &tPline, &tPgon, true, tmpIntPt)) {
				polylineIntersectsPolygon = true;
				float z = contour[j].z() + (contour[jNext].z() - contour[j].z()) * tPgon;

				//first intersection
				if (intCount == 0) {
					firstIntPlineIdx = i;
					firstIntContourIdx = j;
					firstIntPt = QVector3D(tmpIntPt.x(), tmpIntPt.y(), z);
				} else if (intCount == 1) {
					secondIntPlineIdx = i;
					secondIntContourIdx = j;
					secondIntPt = QVector3D(tmpIntPt.x(), tmpIntPt.y(), z);
				} else {
					return false;
				}
				intCount++;
			}
		}
	}

	if (intCount != 2) return false;

	//Once we have intersection points and indexes, we reconstruct the two polygons
	pgon1.clear();
	pgon2.clear();
	int pgonVtxIte;
	int plineVtxIte;

	//If first polygon segment intersected has an index greater
	//	than second segment, modify indexes for correct reconstruction
	if (firstIntContourIdx > secondIntContourIdx) {
		secondIntContourIdx += contourSz;
	}

	//==== Reconstruct first polygon
	//-- append polygon contour
	pgon1.push_back(firstIntPt);
	pgonVtxIte = firstIntContourIdx;
	while (pgonVtxIte < secondIntContourIdx) {
		pgon1.push_back(contour[(pgonVtxIte+1)%contourSz]);
		pgonVtxIte++;
	}
	pgon1.push_back(secondIntPt);
	//-- append polyline points
	plineVtxIte = secondIntPlineIdx;
	while (plineVtxIte > firstIntPlineIdx) {
		pgon1.push_back(pline[(plineVtxIte)]);
		plineVtxIte--;
	}

	//==== Reconstruct second polygon
	//-- append polygon contour
	pgon2.push_back(secondIntPt);
	pgonVtxIte = secondIntContourIdx;
	while (pgonVtxIte < firstIntContourIdx + contourSz) {
		pgon2.push_back(contour[(pgonVtxIte+1) % contourSz]);
		pgonVtxIte++;
	}
	pgon2.push_back(firstIntPt);
	//-- append polyline points
	plineVtxIte = firstIntPlineIdx;
	while (plineVtxIte < secondIntPlineIdx) {
		pgon2.push_back(pline[(plineVtxIte + 1)]);
		plineVtxIte++;
	}

	//verify that two polygons are created after the split. If not, return false
	if (pgon1.size() < 3 || pgon2.size() < 3) {
		return false;
	}

	return polylineIntersectsPolygon;
}
*/

/**
 * Only works for polygons with no holes in them
 */
/*
bool Polygon2D::splitMeWithPolyline2(const std::vector<QVector2D>& pline, Loop2D &pgon1, Loop2D &pgon2) {
	bool polylineIntersectsPolygon = false;

	size_t plineSz = pline.size();
	size_t contourSz = this->contour.size();

	if(plineSz < 2 || contourSz < 3){
		//std::cout << "ERROR: Cannot split if polygon has fewer than three vertices of if polyline has fewer than two points\n.";
		return false;
	}

	QVector2D tmpIntPt;
	QVector2D firstIntPt;
	QVector2D secondIntPt;
	float tPline, tPgon;
	int firstIntPlineIdx    = -1;
	int secondIntPlineIdx   = -1;
	int firstIntContourIdx  = -1;
	int secondIntContourIdx = -1;
	int intCount = 0;

	//iterate along polyline
	for (int i = 0; i < plineSz - 1; ++i) {
		int iNext = i+1;

		for (int j = 0; j < contourSz; ++j) {
			int jNext = (j+1)%contourSz;

			if (Util::segmentSegmentIntersectXY(QVector2D(pline[i]), QVector2D(pline[iNext]), QVector2D(contour[j]), QVector2D(contour[jNext]), &tPline, &tPgon, true, tmpIntPt)) {
				polylineIntersectsPolygon = true;

				//first intersection
				if (intCount == 0) {
					firstIntPlineIdx = i;
					firstIntContourIdx = j;
					firstIntPt = tmpIntPt;
				} else if (intCount == 1) {
					secondIntPlineIdx = i;
					secondIntContourIdx = j;
					secondIntPt = tmpIntPt;
				} else {
					return false;
				}
				intCount++;
			}
		}
	}

	if (intCount != 2) return false;

	//Once we have intersection points and indexes, we reconstruct the two polygons
	pgon1.clear();
	pgon2.clear();
	int pgonVtxIte;
	int plineVtxIte;

	//If first polygon segment intersected has an index greater
	//	than second segment, modify indexes for correct reconstruction
	if (firstIntContourIdx > secondIntContourIdx) {
		secondIntContourIdx += contourSz;
	}

	//==== Reconstruct first polygon
	//-- append polygon contour
	pgon1.push_back(firstIntPt);
	pgonVtxIte = firstIntContourIdx;
	while (pgonVtxIte < secondIntContourIdx) {
		pgon1.push_back(contour[(pgonVtxIte+1)%contourSz]);
		pgonVtxIte++;
	}
	pgon1.push_back(secondIntPt);
	//-- append polyline points
	plineVtxIte = secondIntPlineIdx;
	while (plineVtxIte > firstIntPlineIdx) {
		pgon1.push_back(pline[(plineVtxIte)]);
		plineVtxIte--;
	}

	//==== Reconstruct second polygon
	//-- append polygon contour
	pgon2.push_back(secondIntPt);
	pgonVtxIte = secondIntContourIdx;
	while (pgonVtxIte < firstIntContourIdx + contourSz) {
		pgon2.push_back(contour[(pgonVtxIte+1) % contourSz]);
		pgonVtxIte++;
	}
	pgon2.push_back(firstIntPt);
	//-- append polyline points
	plineVtxIte = firstIntPlineIdx;
	while (plineVtxIte < secondIntPlineIdx) {
		pgon2.push_back(pline[(plineVtxIte + 1)]);
		plineVtxIte++;
	}

	//verify that two polygons are created after the split. If not, return false
	if (pgon1.size() < 3 || pgon2.size() < 3) {
		return false;
	}

	return polylineIntersectsPolygon;
}
*/

/**
* @brief: Given a polygon, this function computes the polygon's inwards offset. The offset distance
* is not assumed to be constant and must be specified in the vector offsetDistances. The size of this
* vector must be equal to the number of vertices of the polygon.
* Note that the i-th polygon segment is defined by vertices i and i+1.
* The polygon vertices are assumed to be oriented clockwise
* @param[in] offsetDistances: Perpendicular distance from offset segment i to polygon segment i.
* @param[out] pgonInset: The vertices of the polygon inset
* @return insetArea: Returns the area of the polygon inset		
**/
/*float Polygon2D::computeInset(float offsetDistance, Loop2D &pgonInset, bool computeArea) {
	if(contour.size() < 3) return 0.0f;				
	std::vector<float> offsetDistances(contour.size(), offsetDistance);

	return computeInset(offsetDistances, pgonInset, computeArea);
}*/

/*float Polygon2D::computeInset(std::vector<float> offsetDistances, Loop2D &pgonInset, bool computeArea) {
	Loop2D cleanPgon;
	double tol = 0.01f;

	cleanPgon = this->contour;

	int prev, next;
	int cSz = cleanPgon.size();

	if (cSz < 3) return 0.0f;

	if (Polygon2D::reorientFace(cleanPgon)) {
		std::reverse(offsetDistances.begin(), offsetDistances.end());
	}

	//if offsets are zero, add a small epsilon just to avoid division by zero
	for(size_t i=0; i<offsetDistances.size(); ++i){
		if(fabs(offsetDistances[i]) < tol){
			offsetDistances[i] = tol;
		}
	}

	pgonInset.resize(cSz);
	QVector3D intPt;

	for (int cur = 0; cur < cSz; ++cur) {
		//point p1 is the point with index cur
		prev = (cur-1+cSz)%cSz; //point p0
		next = (cur+1)%cSz;	  //point p2

		Util::getIrregularBisector(cleanPgon[prev], cleanPgon[cur], cleanPgon[next], offsetDistances[prev], offsetDistances[cur], intPt);

		pgonInset[cur] = intPt;
	}

	//Compute inset area
	if(computeArea){

		boost::geometry::ring_type<Polygon2D>::type bg_contour;
		boost::geometry::ring_type<Polygon2D>::type bg_contour_inset;
		float contArea;
		float contInsetArea;

		if (pgonInset.size() > 0) {
			boost::geometry::assign(bg_contour_inset, pgonInset);
			boost::geometry::correct(bg_contour_inset);

			if (boost::geometry::intersects(bg_contour_inset)) {
				pgonInset.clear();
				return 0.0f;
			} else {

				boost::geometry::assign(bg_contour, cleanPgon);
				boost::geometry::correct(bg_contour);
				//if inset is not within polygon
				if (!Util::is2DRingWithin2DRing(bg_contour_inset, bg_contour)) {
					pgonInset.clear();
					return 0.0f;
				} else {
					contArea = fabs(boost::geometry::area(bg_contour));
					contInsetArea = fabs(boost::geometry::area(bg_contour_inset));

					if (contInsetArea < contArea) {
						//return boost::geometry::area(bg_contour_inset);
						return contInsetArea;
					} else {
						pgonInset.clear();
						return 0.0f;
					}
				}
			}
		} else {
			pgonInset.clear();
			return 0.0f;
		}
	}
	return 0.0f;

}
*/

/*float Polygon2D::computeArea(bool parallelToXY) {
	return Polygon2D::computeLoopArea(this->contour, parallelToXY);
}*/

/**
 * Reorient polygon vertices so that they are ordered in CCW.
 *
 * @param onlyCheck true: the polygon is not modified
 * @return true if the original polygon is ordered in CW.
 **/
bool Polygon2D::reorientFace(bool onlyCheck) {
	return Polygon2D::reorientFace(contour, onlyCheck);
	/*
	int pfaceSz = contour.size();
	int next;
	float tmpSum = 0.0f;

	for (int i = 0; i < pfaceSz; ++i) {
		next = (i+1) % pfaceSz;
		tmpSum = tmpSum + (contour[next].x() - contour[i].x()) * (contour[next].y() + contour[i].y());
	}			

	if (tmpSum > 0.0f) {
		if (!onlyCheck) {
			std::reverse(contour.begin(), contour.end());
		}
		return true;
	} else {
		return false;
	}
	*/
}

bool Polygon2D::contains(const QVector2D& pt) {
	tessellate();

	for (int i = 0; i < trapezoids.size(); ++i) {
		bool outside = false;

		int sz = trapezoids[i].size();
		for (int j = 0; j < sz; ++j) {
			QVector2D vec1 = trapezoids[i][(j + 1) % sz] - trapezoids[i][j];
			QVector2D vec2 = pt - trapezoids[i][j];
			if (vec1.x() * vec2.y() - vec1.y() * vec2.x() < 0) outside = true;
		}

		if (!outside) return true;
	}

	/*
	for (int i = 0; i < size(); ++i) {
		QVector2D vec1 = contour[(i + 1) % size()] - contour[i];
		QVector2D vec2 = pt - contour[i];
		if (vec1.x() * vec2.y() - vec1.y() * vec2.x() > 0) return false;
	}
	return true;
	*/
	return false;
}

/**
 * このポリゴンを三角形または凸四角形の集合に分割する。
 * 各図形の頂点は、openでCCWオーダである。
 */
std::vector<Loop2D>& Polygon2D::tessellate() {
	if (tessellated) return trapezoids;

	trapezoids.clear();

	if (size() < 3) return trapezoids;

	// create 2D polygon data
	std::vector<boost::polygon::point_data<double> > polygon;
	polygon.resize(size());
	for (int i = 0; i < size(); i++) {
		polygon[i] = boost::polygon::construct<boost::polygon::point_data<double> >(contour[i].x(), contour[i].y());
	}

	// create 2D polygon with holes data
	boost::polygon::polygon_with_holes_data<double> temp;
	boost::polygon::set_points(temp, polygon.begin(), polygon.end());

	// create 2D polygon set
	boost::polygon::polygon_set_data<double> polygonSet;
	polygonSet.insert(temp);

	// tessellation
	std::vector<boost::polygon::polygon_with_holes_data<double> > results;
	polygonSet.get_trapezoids(results);

	for (int i = 0; i < results.size(); i++) {
		boost::polygon::polygon_with_holes_data<double>::iterator_type it = results[i].begin();
		Loop2D trapezoid;
		Loop2D tex_coord;
		while (it != results[i].end()) {
			float z = 0.0f;
			bool done = false;

			trapezoid.push_back(QVector2D((*it).x(), (*it).y()));
			it++;
		}

		if (trapezoid.size() < 3) continue;

		Polygon2D::reorientFace(trapezoid);

		// The resulting polygon is usually closed, so remove the last point.
		if ((trapezoid[trapezoid.size() - 1] - trapezoid[0]).lengthSquared() == 0) {
			trapezoid.pop_back();
		}

		if (trapezoid.size() >= 3) trapezoids.push_back(trapezoid);
	}

	tessellated = true;

	return trapezoids;
}

/**
 * このポリゴンと、与えられた線（または線分）との交点を求める
 */
bool Polygon2D::intersect(const QVector2D& a, const QVector2D& b, float *tab, float *tcd, QVector2D &intPoint) const {
	for (int i = 0; i < size(); ++i) {
		QVector2D c = contour[i];
		QVector2D d = contour[(i + 1) % size()];

		if (Util::segmentSegmentIntersectXY(a, b, c, d, tab, tcd, true, intPoint)) {
			return true;
		}
	}

	return false;
}

/**
 * Reorient polygon vertices so that they are ordered in CCW.
 *
 * @param pface the vertices of the polygon
 * @param onlyCheck true: the polygon is not modified
 * @return true if the original polygon is ordered in CW.
 **/
bool Polygon2D::reorientFace(Loop2D& pface, bool onlyCheck) {
	int pfaceSz = pface.size();
	int next;
	float tmpSum = 0.0f;

	for (int i = 0; i < pfaceSz; ++i) {
		next = (i + 1) % pfaceSz;
		tmpSum = tmpSum + (pface[next].x() - pface[i].x()) * (pface[next].y() + pface[i].y());
	}			

	if (tmpSum > 0.0f) {				
		if (!onlyCheck) {
			std::reverse(pface.begin(), pface.end());
		}
		return true;
	}
	return false;
}

/**
 * @brief: Merge consecutive vertices that are within a distance threshold to each other
 * This function does not support Z coordinate. 
 **/
/*int Polygon2D::cleanLoop(const Loop2D& pin, Loop2D& pout, float threshold = 1.0f) {
	float thresholdSq = threshold * threshold;

	if (pin.size() < 3) {
		return 1;
	}

	boost::geometry::ring_type<Polygon2D>::type bg_pin;
	boost::geometry::ring_type<Polygon2D>::type bg_pout;
	boost::geometry::assign(bg_pin, pin);
	boost::geometry::correct(bg_pin);
	boost::geometry::simplify(bg_pin, bg_pout, threshold);

	//strategy::simplify::douglas_peucker

	//copy points back
	QVector3D tmpPt;
	for (size_t i = 0; i < bg_pout.size(); ++i) {
		tmpPt.setX(bg_pout[i].x());
		tmpPt.setY(bg_pout[i].y());
		pout.push_back(tmpPt);						
	}

	//remove last point
	if ((pout[0] - pout[pout.size()-1]).lengthSquared() < thresholdSq) {
		pout.pop_back();				
	}

	//clean angles
	int next, prev;
	QVector3D cur_prev, cur_next;
	float ang;
	float angleThreshold = 0.01f;
	for (size_t i = 0; i < pout.size(); ++i) {
		next = (i+1) % pout.size();
		prev = (i-1+pout.size()) % pout.size();
		cur_prev = pout[prev] - pout[i];
		cur_next = pout[next] - pout[i];

		ang = Util::angleBetweenVectors(cur_prev, cur_next);
		if ((fabs(ang) < angleThreshold) || (fabs(ang - M_PI ) < angleThreshold) || (!(ang==ang))) {
			pout.erase(pout.begin() + i);
			--i;
		}
	}

	return 0;
}*/

void Polygon2D::transformLoop(const Loop2D& pin, Loop2D& pout, QMatrix4x4& transformMat) {
	pout = pin;
	for (int i=0; i<pin.size(); ++i) {
		QVector3D pt = transformMat * QVector3D(pin[i].x(), pin[i].y(), 0.0f);
		pout[i].setX(pt.x());
		pout[i].setY(pt.y());
	}
}

/*float Polygon2D::computeLoopArea(const Loop2D& pin, bool parallelToXY) {
	float _area = 0.0f;

	if (parallelToXY) {	
		boost::geometry::ring_type<Polygon2D>::type bg_pgon;
		boost::geometry::assign(bg_pgon, pin);
		bg_pgon.close();
		_area = fabs(boost::geometry::area(bg_pgon));
	} else {				
		_area = fabs(Util::area3D_Polygon(pin));
	}
	return _area;
}*/

/**
 * density: number of points to be generated per square meter
 */
/*void Polygon2D::sampleTriangularLoopInterior(const Loop2D& pin, std::vector<QVector3D>& pts, float density) {
	if (pin.size() == 3) {
		QVector3D tmpPt;
		QVector3D v1_minus_v0;
		QVector3D v2_minus_v0;				
		QVector3D crossP;

		v1_minus_v0 = pin.at(1) - pin.at(0);
		v2_minus_v0 = pin.at(2) - pin.at(0);

		//float loopArea = computeLoopArea(pin);
		float loopArea = 0.5f*(QVector3D::crossProduct(v1_minus_v0, v2_minus_v0).length());

		int numSamples = (int)(density*loopArea);

		float rand1, rand2;

		for (int i=0; i<numSamples; ++i) {
			rand1 = Util::genRand();
			rand2 = Util::genRand();
			if (rand1 + rand2 > 1.0f) {
				rand1 = 1.0 - rand1;
				rand2 = 1.0 - rand2;
			}

			tmpPt = pin.at(0) + rand1 * v1_minus_v0 + rand2 * v2_minus_v0;
			pts.push_back(tmpPt);
		}
	}
}*/

/**
* @brief: Get polygon axis aligned bounding box
* @return: The dimensions of the AABB 
**/
QVector2D Polygon2D::getLoopAABB(const Loop2D& pin, QVector2D& minCorner, QVector2D& maxCorner) {
	maxCorner = QVector2D(-FLT_MAX, -FLT_MAX);
	minCorner = QVector2D( FLT_MAX,  FLT_MAX);

	for (int i = 0; i < pin.size(); ++i) {
		if (pin[i].x() > maxCorner.x()) {
			maxCorner.setX(pin[i].x());
		}
		if (pin[i].y() > maxCorner.y()) {
			maxCorner.setY(pin[i].y());
		}

		if (pin[i].x() < minCorner.x()) {
			minCorner.setX(pin[i].x());
		}
		if (pin[i].y() < minCorner.y()) {
			minCorner.setY(pin[i].y());
		}
	}
	return maxCorner - minCorner;
}

BBox Polygon2D::getLoopAABB() const {
	BBox bbox;

	for (int i = 0; i < contour.size(); ++i) {
		bbox.addPoint(contour[i]);
	}

	return bbox;
}

/**
* Get polygon oriented bounding box
**/
void Polygon2D::getLoopOBB(const Loop2D& pin, QVector2D& size, QMatrix4x4& xformMat) {
	float alpha = 0.0f;			
	float deltaAlpha = 0.05 * M_PI;
	float bestAlpha;

	Loop2D rotLoop;
	QMatrix4x4 rotMat;
	QVector2D minPt, maxPt;
	QVector2D origMidPt;
	QVector2D boxSz;
	QVector2D bestBoxSz;
	float curArea;
	float minArea = FLT_MAX;

	rotLoop = pin;
	Polygon2D::getLoopAABB(rotLoop, minPt, maxPt);
	origMidPt = 0.5f*(minPt + maxPt);

	int cSz = pin.size();
	QVector2D difVec;
	for (int i=0; i<pin.size(); ++i) {
		difVec = (pin.at((i+1) % cSz) - pin.at(i)).normalized();
		alpha = atan2(difVec.x(), difVec.y());
		rotMat.setToIdentity();				
		rotMat.rotate(Util::rad2deg(alpha), 0.0f, 0.0f, 1.0f);				

		transformLoop(pin, rotLoop, rotMat);
		boxSz = Polygon2D::getLoopAABB(rotLoop, minPt, maxPt);
		curArea = boxSz.x() * boxSz.y();
		if (curArea < minArea) {
			minArea = curArea;
			bestAlpha = alpha;
			bestBoxSz = boxSz;
		}
	}

	xformMat.setToIdentity();											
	xformMat.rotate(Util::rad2deg(bestAlpha), 0.0f, 0.0f, 1.0f);
	xformMat.setRow(3, QVector4D(origMidPt.x(), origMidPt.y(), 0.0f, 1.0f));
	size = bestBoxSz;
}

/**
* Get polygon oriented bounding box
**/
void Polygon2D::getLoopOBB(const QVector2D& dir, Loop2D& bboxRotLoop) const {
	Loop2D rotLoop;
	QMatrix4x4 rotMat;

	float alpha = atan2f(dir.y(), dir.x());
	rotMat.setToIdentity();
	rotMat.rotate(Util::rad2deg(-alpha), 0.0f, 0.0f, 1.0f);
	transformLoop(contour, rotLoop, rotMat);

	QVector2D minPt, maxPt;
	QVector2D size = Polygon2D::getLoopAABB(rotLoop, minPt, maxPt);

	Loop2D bboxLoop;
	bboxLoop.push_back(minPt);
	bboxLoop.push_back(QVector2D(minPt.x(), maxPt.y()));
	bboxLoop.push_back(maxPt);
	bboxLoop.push_back(QVector2D(maxPt.x(), minPt.y()));

	rotMat.setToIdentity();
	rotMat.rotate(Util::rad2deg(alpha), 0.0f, 0.0f, 1.0f);
	transformLoop(bboxLoop, bboxRotLoop, rotMat);
}

/*void Polygon2D::getMyOBB(QVector3D& size, QMatrix4x4& xformMat) {
	Polygon2D::getLoopOBB(this->contour, size, xformMat);
}*/

/*
void Polygon2D::extrudePolygon(const Loop2D& basePgon, float height, std::vector<Polygon2D>& pgonExtrusion) {	
	QVector3D zTransV(0.0f, 0.0f, height);
	int iNext;
	int pgonContourSz = basePgon.size();
	for (int i = 0; i < pgonContourSz; ++i) {
		iNext = (i+1) % pgonContourSz;

		//construct face
		Polygon2D tmpPgon1;
		Polygon2D tmpPgon2;

		tmpPgon1.contour.reserve(3); //pre allocate capacity for polygon contour
		tmpPgon1.contour.push_back(basePgon[i]);
		tmpPgon1.contour.push_back(basePgon[iNext]);
		tmpPgon1.contour.push_back(basePgon[iNext] + zTransV);

		tmpPgon2.contour.reserve(3); //pre allocate capacity for polygon contour
		tmpPgon2.contour.push_back(basePgon[i]);
		tmpPgon2.contour.push_back(basePgon[iNext] + zTransV);
		tmpPgon2.contour.push_back(basePgon[i] + zTransV);		

		//add two triangular faces to solid
		pgonExtrusion.push_back(tmpPgon1);
		pgonExtrusion.push_back(tmpPgon2);
	}
}*/

/**
 * Shortest distance from a point to a polygon
 */
float Polygon2D::distanceXYToPoint(const Loop2D& pin, const QVector3D& pt) {
	float minDist = FLT_MAX;
	float dist;
	int idxNext;

	for (size_t i = 0; i < pin.size(); ++i) {
		idxNext = (i+1) % (pin.size());
		dist = Util::pointSegmentDistanceXY(pin.at(i), pin.at(idxNext), pt);
		if (dist < minDist) {
			minDist = dist;
		}
	}
	return minDist;
}

/**
 * this function measures the minimum distance from the vertices of a contour A
 * to the edges of a contour B, i.e., it measures the distances from each vertex of A
 * to all the edges in B, and returns the minimum of such distances
 */
float Polygon2D::distanceXYfromContourAVerticesToContourB(const Loop2D& pA, const Loop2D& pB) {
	float minDist = FLT_MAX;
	float dist;

	for (size_t i = 0; i < pA.size(); ++i) {
		dist = Polygon2D::distanceXYToPoint(pB, pA.at(i));
		if (dist < minDist) {
			minDist = dist;
		}
	}
	return minDist;
}

