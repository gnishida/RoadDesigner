#pragma once

#include "RoadView.h"
#include "ui_RoadBox.h"
#include <qwidget.h>
#include <qpushbutton.h>

class MyMainWindow;

class RoadBox : public QWidget {
Q_OBJECT

public:
	Ui::RoadBox ui;
	MyMainWindow* mainWin;
	RoadView* view;
	QPushButton* pushButtonSelect;

public:
	RoadBox(MyMainWindow* mainWin, const char* filename, float roadSize);
	~RoadBox();

public slots:
	void select();
};

