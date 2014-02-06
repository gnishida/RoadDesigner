#include "RoadBox.h"
#include "MyMainWindow.h"
#include "GraphUtil.h"
#include <qlayout.h>
#include <qpushbutton.h>

RoadBox::RoadBox(MyMainWindow* mainWin, const char* filename, float roadSize) : QWidget((QWidget*)mainWin) {
	this->mainWin = mainWin;
	this->setMinimumSize(250, 250);

	QVBoxLayout* layout = new QVBoxLayout();
	this->setLayout(layout);

	view = new RoadView(mainWin, roadSize);

	view->load(filename);
	pushButtonSelect = new QPushButton(tr("Select"));

	layout->addWidget(view);
	layout->addWidget(pushButtonSelect);

	// register the event handlers
	connect(pushButtonSelect, SIGNAL(clicked()), this, SLOT(select()));
}

RoadBox::~RoadBox() {
}

void RoadBox::select() {
	mainWin->glWidget->ref_roads = GraphUtil::copyRoads(view->roads);
	GraphUtil::translate(mainWin->glWidget->ref_roads, view->offset);

	mainWin->glWidget->updateGL();
}
