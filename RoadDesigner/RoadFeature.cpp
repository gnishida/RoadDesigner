#include "RoadFeature.h"
#include <QFile>
#include <QDomDocument>
#include <QTextStream>

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

