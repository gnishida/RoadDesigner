#include <QtGui/QApplication>
#include <common/Polygon2D.h>
#include <common/VoronoiUtil.h>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
