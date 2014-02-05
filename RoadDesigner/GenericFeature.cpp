#include "GenericFeature.h"
#include "Util.h"
#include <QFile>
#include <QDomDocument>

#ifndef M_PI
#define M_PI	3.141592653589793238
#endif

std::vector<float> GenericFeature::getAngles(int num) const {
	std::vector<float> angles;

	float angle = Util::uniform_rand(0, M_PI * 2.0f);
	float angle_step = M_PI * 2.0f / num;

	for (int i = 0; i < num; ++i) {
		angles.push_back(angle + Util::uniform_rand(-0.2f, 0.2f));

		angle += angle_step;		
	}

	return angles;
}

/**
 * 指定された数のエッジ方向を生成する。ただし、dirで指定された方向には既にエッジがあるので、この方向は避ける。
 */
std::vector<float> GenericFeature::getAngles(int num, const QVector2D &dir) const {
	std::vector<float> angles;

	float angle = atan2f(dir.y(), dir.x());
	float angle_step = M_PI * 2.0f / (num + 1);

	for (int i = 0; i < num; ++i) {
		angle += angle_step;		

		angles.push_back(angle + Util::uniform_rand(-0.2f, 0.2f));
	}

	return angles;
}

/**
 * 指定された方向に垂直な２方向を生成する。
 */
std::vector<float> GenericFeature::getPerpendicularAngles(const QVector2D &dir) const {
	std::vector<float> angles;

	float angle = atan2f(dir.x(), -dir.y());
	float angle_step = M_PI;

	for (int i = 0; i < 2; ++i) {
		angles.push_back(angle + Util::uniform_rand(-0.2f, 0.2f));

		angle += angle_step;		
	}

	return angles;
}

/**
 * 指定された数のエッジ長を生成する。
 */
std::vector<float> GenericFeature::getLengths(int roadType, int num) const {
	std::vector<float> ret;

	for (int i = 0; i < num; ++i) {
		ret.push_back(generateLength(roadType, Util::uniform_rand()));
	}

	return ret;
}

/** 
 * 与えられたuniform random numberに基づいて、エッジの長さを生成する。
 */
float GenericFeature::generateLength(int roadType, float uniform_random_number) const {
	if (roadType == 1) {
		for (QMap<float, float>::iterator it = streetLengths.begin(); it != streetLengths.end(); ++it) {
			if (uniform_random_number <= streetLengths[it.key()]) return it.key();
		}
	} else {
		for (QMap<float, float>::iterator it = avenueLengths.begin(); it != avenueLengths.end(); ++it) {
			if (uniform_random_number <= avenueLengths[it.key()]) return it.key();
		}
	}

	return 0.0f;
}

int GenericFeature::getDirections(int roadType) const {
	float uniform_random_number = Util::uniform_rand();

	for (QMap<int, float>::iterator it = avenueNumDirections.begin(); it != avenueNumDirections.end(); ++it) {
		if (uniform_random_number <= avenueNumDirections[it.key()]) return it.key();
	}

	return 0;
}

/**
 * ファイルから、グリッドの特徴量を読込む。
 */
void GenericFeature::load(QString filename) {
	QFile file(filename);

	QDomDocument doc;
	doc.setContent(&file, true);
	QDomElement root = doc.documentElement();

	QDomNode node = root.firstChild();
	while (!node.isNull()) {
		if (node.toElement().tagName() == "feature") {
			if (node.toElement().attribute("type") == "generic") {
				load(node);
				break;
			}
		}

		node = node.nextSibling();
	}
}

/**
 * 与えられたfeatureノード配下のXML情報に基づいて、グリッド特徴量を設定する。
 */
void GenericFeature::load(QDomNode& node) {
	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "avenue") {
			loadAvenue(child);
		} else if (child.toElement().tagName() == "street") {
			loadStreet(child);
		}

		child = child.nextSibling();
	}
}

/**
 * 与えられたfeatureノード配下のXML情報に基づいて、Avenueの特徴量を設定する。
 */
void GenericFeature::loadAvenue(QDomNode& node) {
	avenueLengths.clear();
	avenueNumDirections.clear();

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "length") {
			QDomNode child2 = child.firstChild();
			while (!child2.isNull()) {
				float len = child2.toElement().attribute("key").toFloat();
				float accm = child2.firstChild().nodeValue().toFloat();
				avenueLengths[len] = accm;

				child2 = child2.nextSibling();
			}
		} else if (child.toElement().tagName() == "numDirections") {
			QDomNode child2 = child.firstChild();
			while (!child2.isNull()) {
				float num = child2.toElement().attribute("key").toFloat();
				float accm = child2.firstChild().nodeValue().toFloat();
				avenueNumDirections[num] = accm;

				child2 = child2.nextSibling();
			}
		}

		child = child.nextSibling();
	}
}

/**
 * 与えられたfeatureノード配下のXML情報に基づいて、Local streetの特徴量を設定する。
 */
void GenericFeature::loadStreet(QDomNode& node) {
	streetLengths.clear();
	streetNumDirections.clear();

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "length") {
			QDomNode child2 = child.firstChild();
			while (!child2.isNull()) {
				float len = child2.toElement().attribute("key").toFloat();
				float accm = child2.firstChild().nodeValue().toFloat();
				streetLengths[len] = accm;

				child2 = child2.nextSibling();
			}
		} else if (child.toElement().tagName() == "numDirections") {
			QDomNode child2 = child.firstChild();
			while (!child2.isNull()) {
				float num = child2.toElement().attribute("key").toFloat();
				float accm = child2.firstChild().nodeValue().toFloat();
				streetNumDirections[num] = accm;

				child2 = child2.nextSibling();
			}
		}

		child = child.nextSibling();
	}
}
