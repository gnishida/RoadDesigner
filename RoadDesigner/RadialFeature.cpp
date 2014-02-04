#include "RadialFeature.h"
#include <QFile>
#include <QDomDocument>

/**
 * ファイルから、Radialパターンの特徴量を読込む。
 */
void RadialFeature::load(QString filename) {
	QFile file(filename);

	QDomDocument doc;
	doc.setContent(&file, true);
	QDomElement root = doc.documentElement();

	QDomNode node = root.firstChild();
	while (!node.isNull()) {
		if (node.toElement().tagName() == "feature") {
			if (node.toElement().attribute("type") == "radial") {
				load(node);
				break;
			}
		}

		node = node.nextSibling();
	}
}

/**
 * 与えられたfeatureノード配下のXML情報に基づいて、Radial特徴量を設定する。
 */
void RadialFeature::load(QDomNode& node) {
	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "radius") {
			radius = child.firstChild().nodeValue().toFloat();
		} else if (child.toElement().tagName() == "directions") {
			numDirections = child.firstChild().nodeValue().toInt();
		}

		child = child.nextSibling();
	}
}

QColor RadialFeature::color() {
	return QColor(255 - group_id * 64 % 255, 0, 0);
}

/**
 * 領域を表すポリゴンを返却する。
 */
Polygon2D RadialFeature::polygon() {
	return _polygon;
}