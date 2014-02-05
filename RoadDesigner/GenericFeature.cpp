#include "GenericFeature.h"
#include <QFile>
#include <QDomDocument>

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
			if (node.toElement().attribute("type") == "grid") {
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
	/*
	length1.clear();
	length2.clear();

	QDomNode child = node.firstChild();
	while (!child.isNull()) {
		if (child.toElement().tagName() == "angle1") {
			angle1 = child.firstChild().nodeValue().toFloat();
		} else if (child.toElement().tagName() == "angle2") {
			angle2 = child.firstChild().nodeValue().toFloat();
		} else if (child.toElement().tagName() == "length1") {
			QDomNode child2 = child.firstChild();
			while (!child2.isNull()) {
				float len = child2.toElement().attribute("key").toFloat();
				float accm = child2.firstChild().nodeValue().toFloat();
				length1[len] = accm;

				child2 = child2.nextSibling();
			}
		} else if (child.toElement().tagName() == "length2") {
			QDomNode child2 = child.firstChild();
			while (!child2.isNull()) {
				float len = child2.toElement().attribute("key").toFloat();
				float accm = child2.firstChild().nodeValue().toFloat();
				length2[len] = accm;

				child2 = child2.nextSibling();
			}
		}

		child = child.nextSibling();
	}
	*/
}