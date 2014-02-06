#include "RadialFeature.h"
#include <QFile>
#include <QDomDocument>
#include <QTextStream>

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
	radii.clear();

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "radius") {
			radii.push_back(child.firstChild().nodeValue().toFloat());
		} else if (child.toElement().tagName() == "directions") {
			numDirections = child.firstChild().nodeValue().toInt();
		}

		child = child.nextSibling();
	}
}

/**
 * 特徴量をxmlファイルに保存する。
 */
void RadialFeature::save(QString filename) {
	QDomDocument doc;

	QDomElement root = doc.createElement("features");
	doc.appendChild(root);

	QDomElement node_feature = doc.createElement("feature");
	node_feature.setAttribute("type", "radial");
	root.appendChild(node_feature);

	// write radius node
	for (int i = 0; i < radii.size(); ++i) {
		// write radius node
		QDomElement node_radius = doc.createElement("radius");
		node_feature.appendChild(node_radius);

		QString str;
		str.setNum(radii[i]);
		QDomText node_radius_value = doc.createTextNode(str);
		node_radius.appendChild(node_radius_value);
	}

	// write numDirections node
	QDomElement node_numDirections = doc.createElement("numDirections");
	node_feature.appendChild(node_numDirections);

	QString str;
	str.setNum(numDirections);
	QDomText node_numDirections_value = doc.createTextNode(str);
	node_numDirections.appendChild(node_numDirections_value);

	// write the dom to the file
	QFile file(filename);
	file.open(QIODevice::WriteOnly);

	QTextStream out(&file);
	doc.save(out, 4);
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