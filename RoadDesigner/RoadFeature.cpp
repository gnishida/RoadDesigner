#include "RoadFeature.h"
#include <QFile>
#include <QDomDocument>
#include <QTextStream>

void RoadFeature::clear() {
	gridFeatures.clear();
	radialFeatures.clear();
	genericFeatures.clear();
}

void RoadFeature::load(QString filename) {
	QFile file(filename);

	QDomDocument doc;
	doc.setContent(&file, true);
	QDomElement root = doc.documentElement();

	QDomNode node = root.firstChild();
	while (!node.isNull()) {
		if (node.toElement().tagName() == "feature") {
			if (node.toElement().attribute("type") == "grid") {
				GridFeature gf(gridFeatures.size());
				gf.load(node);
				gridFeatures.push_back(gf);
			} else if (node.toElement().attribute("type") == "radial") {
				RadialFeature rf(radialFeatures.size());
				rf.load(node);
				radialFeatures.push_back(rf);
			} else if (node.toElement().attribute("type") == "generic") {
				GenericFeature gf(genericFeatures.size());
				gf.load(node);
				genericFeatures.push_back(gf);
			}
		}

		node = node.nextSibling();
	}
}

void RoadFeature::save(QString filename) {
	QDomDocument doc;

	QDomElement root = doc.createElement("features");
	doc.appendChild(root);

	for (int i = 0; i < gridFeatures.size(); ++i) {
		gridFeatures[i].save(doc, root);
	}
	for (int i = 0; i < radialFeatures.size(); ++i) {
		radialFeatures[i].save(doc, root);
	}
	for (int i = 0; i < genericFeatures.size(); ++i) {
		genericFeatures[i].save(doc, root);
	}

	// write the dom to the file
	QFile file(filename);
	file.open(QIODevice::WriteOnly);

	QTextStream out(&file);
	doc.save(out, 4);
}

void RoadFeature::addFeature(GridFeature& gf) {
	gridFeatures.push_back(gf);
}

void RoadFeature::addFeature(RadialFeature& rf) {
	radialFeatures.push_back(rf);
}

void RoadFeature::addFeature(GenericFeature& gf) {
	genericFeatures.push_back(gf);
}

/**
 * 抽出した特徴量を正規化する。
 * weightの正規化、centerの正規化を行う。
 */
void RoadFeature::normalize() {
	float total_weight = 0.0f;
	QVector2D total_center;

	// total weight、total centerを計算
	for (int i = 0; i < gridFeatures.size(); ++i) {
		total_weight += gridFeatures[i].weight;
		total_center += gridFeatures[i].center;
	}
	for (int i = 0; i < radialFeatures.size(); ++i) {
		total_weight += radialFeatures[i].weight;
		total_center += radialFeatures[i].center;
	}
	for (int i = 0; i < genericFeatures.size(); ++i) {
		total_weight += genericFeatures[i].weight;
		total_center += genericFeatures[i].center;
	}

	total_center /= (gridFeatures.size() + radialFeatures.size() + genericFeatures.size());

	// total weight、total centerに基づいて、weightとcenterをnormalizeする
	for (int i = 0; i < gridFeatures.size(); ++i) {
		gridFeatures[i].weight /= total_weight;
		gridFeatures[i].center -= total_center;
	}
	for (int i = 0; i < radialFeatures.size(); ++i) {
		radialFeatures[i].weight /= total_weight;
		radialFeatures[i].center -= total_center;
	}
	for (int i = 0; i < genericFeatures.size(); ++i) {
		genericFeatures[i].weight /= total_weight;
		genericFeatures[i].center -= total_center;
	}
}

