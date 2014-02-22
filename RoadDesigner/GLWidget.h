#pragma once

#include <QString>
#include <QGLWidget>
#include <common/PolygonAreaBuilder.h>
#include <road/RoadGraph.h>
#include <road/RoadGraphRenderer.h>
#include <road/RoadArea.h>
#include <road/RoadAreaSet.h>
#include <road/feature/GridFeature.h>
#include <road/feature/RadialFeature.h>
#include "Camera.h"
#include "SimpleFlatTerrain.h"

class MainWindow;

class GLWidget : public QGLWidget {
public:
	static float MIN_Z;
	static float MAX_Z;

public:
	MainWindow* mainWin;
	Camera* camera;
	SimpleFlatTerrain terrain;
	RoadGraphRenderer* renderer;
	QPoint lastPos;
	RoadAreaSet areas;
	int selectedArea;
	PolygonAreaBuilder selectedAreaBuilder;

	// key status
	bool shiftPressed;
	bool controlPressed;
	bool altPressed;
	bool keyXPressed;

public:
	GLWidget(MainWindow *parent);
	~GLWidget();

	void drawScene();

public:
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

private:
	void mouseTo2D(int x, int y, QVector2D &result);
	void GLWidget::snap(QVector2D &pos);
};

