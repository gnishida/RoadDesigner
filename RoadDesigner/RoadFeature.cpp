#include "RoadFeature.h"
#include <QFile>
#include <QDomDocument>
#include <QTextStream>

void RoadFeature::clear() {
	features.clear();
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
				GridFeaturePtr gf = GridFeaturePtr(new GridFeature(features.size()));
				gf->load(node);
				features.push_back(gf);
			} else if (node.toElement().attribute("type") == "radial") {
				RadialFeaturePtr rf = RadialFeaturePtr(new RadialFeature(features.size()));
				rf->load(node);
				features.push_back(rf);
			} else if (node.toElement().attribute("type") == "generic") {
				GenericFeaturePtr gf = GenericFeaturePtr(new GenericFeature(features.size()));
				gf->load(node);
				features.push_back(gf);
			}
		}

		node = node.nextSibling();
	}
}

void RoadFeature::save(QString filename) {
	QDomDocument doc;

	QDomElement root = doc.createElement("features");
	doc.appendChild(root);

	for (int i = 0; i < features.size(); ++i) {
		features[i]->save(doc, root);
	}

	// write the dom to the file
	QFile file(filename);
	file.open(QIODevice::WriteOnly);

	QTextStream out(&file);
	doc.save(out, 4);
}

void RoadFeature::addFeature(AbstractFeaturePtr feature) {
	features.push_back(feature);
}

/**
 * 抽出した特徴量を正規化する。
 * weightの正規化、centerの正規化を行う。
 */
void RoadFeature::normalize() {
	float total_weight = 0.0f;
	QVector2D total_center;

	// total weight、total centerを計算
	for (int i = 0; i < features.size(); ++i) {
		total_weight += features[i]->weight();
		total_center += features[i]->center();
	}

	total_center /= features.size();

	// total weight、total centerに基づいて、weightとcenterをnormalizeする
	for (int i = 0; i < features.size(); ++i) {
		features[i]->setWeight(features[i]->weight() / total_weight);
		features[i]->setCenter(features[i]->center() - total_center);
	}
}

