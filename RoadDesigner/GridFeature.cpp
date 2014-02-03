#include "GridFeature.h"
#include "Util.h"

#ifndef M_PI
#define M_PI	3.141592653
#endif

/**
 * グリッド方向の近似値を使って、グリッドの角度を仮にセットする。
 */
void GridFeature::setAngle(float angle) {
	angle1 = angle;
	angle2 = angle + M_PI * 0.5f;

	accmDirCount = 0;
	accmDir1 = QVector2D(0, 0);
	accmDir2 = QVector2D(0, 0);

	length1.clear();
	length2.clear();
	accmLenCount1 = 0;
	accmLenCount2 = 0;
}

/**
 * エッジ方向を追加しながら、グリッド方向の精度を上げる。
 * ただし、当該エッジの方向が、仮の方向から閾値よりも大きく外れている場合は、そのエッジの方向は無視する。
 * グリッドの方向に加えて、長さの情報も収集し、ヒストグラムに入れていく。
 */
void GridFeature::addEdge(const QVector2D& edge_vec, float threshold) {
	QVector2D dir = edge_vec.normalized();

	float a1 = atan2f(dir.y(), dir.x());
	float a2 = atan2f(-dir.y(), -dir.x());

	float diff1 = Util::diffAngle(angle1, a1);
	float diff2 = Util::diffAngle(angle1, a2);
	float diff3 = Util::diffAngle(angle2, a1);
	float diff4 = Util::diffAngle(angle2, a2);
	float min_diff = std::min(std::min(diff1, diff2), std::min(diff3, diff4));

	// エッジの方向が、仮の方向から閾値よりも大きく外れている場合は、そのエッジの方向は無視する。
	if (min_diff > threshold) return;

	// エッジの方向を、累積方向ベクトルに追加していく
	// また、長さも、ヒストグラムに追加していく
	if (diff1 == min_diff) {
		accmDir1 += dir;
		accmDir2 += QVector2D(-dir.y(), dir.x());
		length1[(int)(edge_vec.length() / 20) * 20] += 1;
		accmLenCount1++;
	} else if (diff2 == min_diff) {
		accmDir1 -= dir;
		accmDir2 -= QVector2D(-dir.y(), dir.x());
		length1[(int)(edge_vec.length() / 20) * 20] += 1;
		accmLenCount1++;
	} else if (diff3 == min_diff) {
		accmDir1 += QVector2D(dir.y(), -dir.x());
		accmDir2 += dir;
		length2[(int)(edge_vec.length() / 20) * 20] += 1;
		accmLenCount2++;
	} else {
		accmDir1 -= QVector2D(dir.y(), -dir.x());
		accmDir2 -= dir;
		length2[(int)(edge_vec.length() / 20) * 20] += 1;
		accmLenCount2++;
	}

	accmDirCount++;
}

/**
 * エッジの追加が終わったら、この関数を呼んで、グリッド方向をより正確な値に更新する。
 * また、グリッドの長さヒストグラムのnormalizeする。
 */
void GridFeature::computeFeature() {
	accmDir1 /= (float)accmDirCount;
	accmDir2 /= (float)accmDirCount;

	angle1 = atan2f(accmDir1.y(), accmDir1.x());
	angle2 = atan2f(accmDir2.y(), accmDir2.x());

	// グリッドの長さをnormalizeする
	for (QMap<float, float>::iterator it = length1.begin(); it != length1.end(); ++it) {
		length1[it.key()] /= (float)accmLenCount1;
	}
	for (QMap<float, float>::iterator it = length2.begin(); it != length2.end(); ++it) {
		length2[it.key()] /= (float)accmLenCount2;
	}

	// グリッドの長さの累積確率分布を生成
	float apd = 0.0f;
	for (QMap<float, float>::iterator it = length1.begin(); it != length1.end(); ++it) {
		apd += length1[it.key()];
		length1[it.key()] = apd;
	}
	apd = 0.0f;
	for (QMap<float, float>::iterator it = length2.begin(); it != length2.end(); ++it) {
		apd += length2[it.key()];
		length2[it.key()] = apd;
	}

	// 累積方向ベクトルをリセットする
	accmDirCount = 0;
	accmDir1 = QVector2D(0, 0);
	accmDir2 = QVector2D(0, 0);

	// 累積長カウンタをリセットする
	accmLenCount1 = 0;
	accmLenCount2 = 0;

	// デバッグ情報を出力
	std::cout << "Grid Feature:" << std::endl;
	std::cout << "  angle 1: " << angle1 * 180.0f / M_PI << std::endl;
	std::cout << "  angle 2: " << angle2 * 180.0f / M_PI << std::endl;
	std::cout << "  length 1:" << std::endl;
	for (QMap<float, float>::iterator it = length1.begin(); it != length1.end(); ++it) {
		std::cout << "      " << it.key() << ": " << length1[it.key()] << std::endl;
	}
	std::cout << "  length 2:" << std::endl;
	for (QMap<float, float>::iterator it = length2.begin(); it != length2.end(); ++it) {
		std::cout << "      " << it.key() << ": " << length2[it.key()] << std::endl;
	}
}

/**
 * 与えられたエッジの方向が、グリッド方向に近いかどうかチェックする。
 *
 * @param threshold			与えられたエッジの方向とグリッド方向の差が、このしきい値以下であれば、trueを返却する
 */
bool GridFeature::isClose(const QVector2D& dir, float threshold) {
	float a1 = atan2f(dir.y(), dir.x());
	float a2 = atan2f(-dir.y(), -dir.x());

	if (Util::diffAngle(angle1, a1) <= threshold || Util::diffAngle(angle1, a2) <= threshold || Util::diffAngle(angle2, a1) <= threshold || Util::diffAngle(angle2, a2) <= threshold) return true;
	else return false;
}

QColor GridFeature::color() {
	return QColor(0, 0, 255 - group_id * 64 % 255);
}

/**
 * 領域を表すポリゴンを返却する。
 */
Polygon2D GridFeature::polygon() {
	return _polygon;
}