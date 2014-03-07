#include <vector>
#include "GLWidget.h"
#include "MainWindow.h"
#include <road/GraphUtil.h>
#include <gl/GLU.h>
#include <road/generator/RoadGeneratorHelper.h>

float GLWidget::MIN_Z = 150.0f;
float GLWidget::MAX_Z = 11520.0f;
//float GLWidget::MAX_Z = 2880.0f;

GLWidget::GLWidget(MainWindow* mainWin) : QGLWidget(QGLFormat(QGL::SampleBuffers), (QWidget*)mainWin) {
	this->mainWin = mainWin;

	renderer = new RoadGraphRenderer();

	// set up the camera
	camera = new Camera();
	camera->setLookAt(0.0f, 0.0f, 0.0f);
	camera->setTranslation(0.0f, 0.0f, MAX_Z);

	areas.setZ(camera->dz);

	QString str;
	str.setNum(camera->dz);
	mainWin->ui.statusBar->showMessage(str);

	terrain.init(15000.0f, 15000.0f, 0.0f);

	// initialize the key status
	shiftPressed = false;
	controlPressed = false;
	altPressed = false;
	keyXPressed = false;

	selectedArea = -1;
	selectedVertex = NULL;
	selectedEdge = NULL;
}

GLWidget::~GLWidget() {
}

void GLWidget::drawScene() {
	terrain.generateMesh();
	renderer->render(terrain.renderables);

	// define the height for other items
	float height = (float)((int)(camera->dz * 0.012f)) * 0.15f;

	// draw the building area
	if (areaBuilder.selecting()) {
		renderer->renderPolyline(areaBuilder.polyline(), QColor(0, 0, 255), GL_LINE_STIPPLE, height);
	}

	// draw the building highway
	if (roadsBuilder.selecting()) {
		renderer->renderPolyline(roadsBuilder.polyline(), QColor(255, 0, 0), GL_LINE_STIPPLE, height);
	}

	// draw the areas
	for (int i = 0; i < areas.size(); ++i) {
		if (i == selectedArea) {
			renderer->renderArea(areas[i].area, GL_LINE_STIPPLE, QColor(0, 0, 255), height);
		} else {
			renderer->renderArea(areas[i].area, GL_LINE_STIPPLE, QColor(196, 196, 255), height);
		}

		// draw the road graph
		areas[i].roads.generateMesh();
		renderer->render(areas[i].roads.renderables);
	}

	// draw the highways
	areas.roads.generateMesh();
	renderer->render(areas.roads.renderables);

	// draw the kernel
	/*if (selectedVertex != NULL && selectedArea >= 0 && areas[selectedArea].roads.graph[selectedVertexDesc]->kernel.id != -1) {
		RoadGraph kernel_graph;
		RoadGeneratorHelper::buildGraphFromKernel(kernel_graph, areas[selectedArea].roads.graph[selectedVertexDesc]->kernel, areas[selectedArea].area.envelope().midPt());
		kernel_graph.setZ(camera->dz);
		kernel_graph.highwayHeight += 10;
		kernel_graph.avenueHeight += 10;
		kernel_graph.generateMesh();
		renderer->render(kernel_graph.renderables);
	}*/

	// draw the selected vertex
	if (selectedVertex != NULL) {
		renderer->renderPoint(selectedVertex->pt, QColor(0, 0, 255), height);
	}

	// draw the selected edge
	if (selectedEdge != NULL) {
		renderer->renderPolyline(selectedEdge->polyLine, QColor(0, 0, 255), GL_LINE_STRIP, height);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers

void GLWidget::keyPressEvent(QKeyEvent *e) {
	shiftPressed = false;
	controlPressed = false;
	altPressed = false;

	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = true;
		break;
	case Qt::Key_Control:
		controlPressed = true;
		break;
	case Qt::Key_Alt:
		altPressed = true;
		break;
	case Qt::Key_X:
		keyXPressed = true;
		break;
	case Qt::Key_Delete:
		if (selectedArea >= 0) {
			areas.remove(selectedArea);
			selectedArea = -1;
		}
	case Qt::Key_Escape:
		areaBuilder.cancel();
		
		mainWin->mode = MainWindow::MODE_AREA_SELECT;
		mainWin->ui.actionAreaSelect->setChecked(true);
		mainWin->ui.actionAreaCreate->setChecked(false);

		updateGL();

		break;
	}
}

void GLWidget::keyReleaseEvent(QKeyEvent* e) {
	if (e->isAutoRepeat()) {
		e->ignore();
		return;
	}

	switch (e->key()) {
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	case Qt::Key_Control:
		controlPressed = false;
		break;
	case Qt::Key_Alt:
		altPressed = false;
		break;
	case Qt::Key_X:
		keyXPressed = false;
		break;
	}
}

void GLWidget::mousePressEvent(QMouseEvent *e) {
	this->setFocus();

	lastPos = e->pos();
	QVector2D pos;
	mouseTo2D(e->x(), e->y(), pos);

	if (e->buttons() & Qt::LeftButton) {
		switch (mainWin->mode) {
		case MainWindow::MODE_AREA_SELECT:
			selectedVertex = NULL;
			selectedEdge = NULL;
			for (int i = 0; i < areas.size(); ++i) {
				if (areas[i].area.contains(pos)) {
					selectedArea = i;

					if (GraphUtil::getVertex(areas[i].roads, pos, 40, selectedVertexDesc)) {
						selectedVertex = areas[i].roads.graph[selectedVertexDesc];
						mainWin->propertyWidget->setRoadVertex(areas[i].roads, selectedVertexDesc, selectedVertex);
						mainWin->propertyWidget->resetRoadEdge();
					} else {
						if (GraphUtil::getEdge(areas[i].roads, pos, 40, selectedEdgeDesc)) {
							selectedEdge = areas[i].roads.graph[selectedEdgeDesc];
							mainWin->propertyWidget->resetRoadVertex();
							mainWin->propertyWidget->setRoadEdge(selectedEdge);
						}
					}

					break;
				}
			}
			break;
		case MainWindow::MODE_AREA_CREATE:
			if (!areaBuilder.selecting()) {
				snap(pos);
				areaBuilder.start(pos);
				setMouseTracking(true);
			}
		
			if (areaBuilder.selecting()) {
				snap(pos);
				areaBuilder.addPoint(pos);
			}

			selectedArea = -1;

			break;
		case MainWindow::MODE_HIGHWAY_SKETCH:
			if (!roadsBuilder.selecting()) {
				roadsBuilder.start(pos);
				setMouseTracking(true);
			}

			if (roadsBuilder.selecting()) {
				roadsBuilder.addPoint(pos);
			}

			break;
		case MainWindow::MODE_BOULEVARD_SKETCH:
			if (!roadsBuilder.selecting()) {
				roadsBuilder.start(pos);
				setMouseTracking(true);
			}

			if (roadsBuilder.selecting()) {
				roadsBuilder.addPoint(pos);
			}

			break;
		}
	}

	updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e) {
	lastPos = e->pos();

	setCursor(Qt::ArrowCursor);
	updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *e) {
	float dx = (float)(e->x() - lastPos.x());
	float dy = (float)(e->y() - lastPos.y());
	lastPos = e->pos();

	QVector2D pos;
	mouseTo2D(e->x(), e->y(), pos);

	if (e->buttons() & Qt::MidButton) {   // Shift the camera
		setCursor(Qt::ClosedHandCursor);
		camera->changeXYZTranslation(-dx * camera->dz * 0.001f, dy * camera->dz * 0.001f, 0);
	} else if (e->buttons() & Qt::RightButton) { // Zoom the camera
		setCursor(Qt::SizeVerCursor);

		camera->changeXYZTranslation(0, 0, -dy * camera->dz * 0.02f);
		if (camera->dz < MIN_Z) camera->dz = MIN_Z;
		if (camera->dz > MAX_Z) camera->dz = MAX_Z;

		QString str;
		str.setNum(camera->dz);
		mainWin->ui.statusBar->showMessage(str);

		// tell the Z coordinate to the road graph so that road graph updates rendering related variables.
		areas.setZ(camera->dz);

		lastPos = e->pos();
	} else {
		switch (mainWin->mode) {
		case MainWindow::MODE_AREA_SELECT:
			break;
		case MainWindow::MODE_AREA_CREATE:
			if (areaBuilder.selecting()) {	// Move the last point of the selected polygonal area
				snap(pos);
				areaBuilder.moveLastPoint(pos);
			}
			break;
		case MainWindow::MODE_HIGHWAY_SKETCH:
			if (roadsBuilder.selecting()) {
				roadsBuilder.moveLastPoint(pos);
			}
			break;
		case MainWindow::MODE_BOULEVARD_SKETCH:
			if (roadsBuilder.selecting()) {
				roadsBuilder.moveLastPoint(pos);
			}
			break;
		}
	}

	updateGL();
}

void GLWidget::mouseDoubleClickEvent(QMouseEvent *e) {
	setMouseTracking(false);

	switch (mainWin->mode) {
	case MainWindow::MODE_AREA_SELECT:
		break;
	case MainWindow::MODE_AREA_CREATE:
		areaBuilder.end();
		areas.add(RoadArea(areaBuilder.polygon()));
		selectedArea = areas.size() - 1;
		areas[selectedArea].roads.setZ(camera->dz);

		mainWin->mode = MainWindow::MODE_AREA_SELECT;
		mainWin->ui.actionAreaSelect->setChecked(true);
		mainWin->ui.actionAreaCreate->setChecked(false);
		break;
	case MainWindow::MODE_HIGHWAY_SKETCH:
		roadsBuilder.end();

		areas.addRoads(RoadEdge::TYPE_HIGHWAY, 2, true, roadsBuilder.polyline());

		mainWin->mode = MainWindow::MODE_AREA_SELECT;
		mainWin->ui.actionAreaSelect->setChecked(true);
		mainWin->ui.actionAreaCreate->setChecked(false);
		mainWin->ui.actionHighwaySketch->setChecked(false);
		mainWin->ui.actionBoulevardSketch->setChecked(false);
		break;
	case MainWindow::MODE_BOULEVARD_SKETCH:
		roadsBuilder.end();

		areas.addRoads(RoadEdge::TYPE_BOULEVARD, 1, false, roadsBuilder.polyline());

		mainWin->mode = MainWindow::MODE_AREA_SELECT;
		mainWin->ui.actionAreaSelect->setChecked(true);
		mainWin->ui.actionAreaCreate->setChecked(false);
		mainWin->ui.actionHighwaySketch->setChecked(false);
		mainWin->ui.actionBoulevardSketch->setChecked(false);
		break;
	}
}

void GLWidget::initializeGL() {
	//qglClearColor(QColor(233, 229, 220));
	qglClearColor(QColor(128, 128, 128));
	glClearDepth(1.0f);

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//---- lighting ----
	glDisable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	static GLfloat lightPosition[4] = { 0.0f, 0.0f, 100.0f, 0.0f };
	static GLfloat lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat lightDiffuse[4] = { 0.75f, 0.75f, 0.5f, 1.0f };

	glEnable(GL_COLOR_MATERIAL);	
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	static GLfloat materialSpecular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static GLfloat materialEmission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialEmission);
}

void GLWidget::resizeGL(int width, int height) {
	height = height ? height : 1;

	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLfloat)width/(GLfloat)height, 5.0f, 90000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera->applyCamTransform();
}

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	camera->applyCamTransform();	

	drawScene();	
}

/**
 * Convert the screen space coordinate (x, y) to the model space coordinate.
 */
void GLWidget::mouseTo2D(int x,int y, QVector2D &result) {
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	// retrieve the matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	// retrieve the projected z-buffer of the origin
	GLdouble origX, origY, origZ;
	gluProject(0, 0, 0, modelview, projection, viewport, &origX, &origY, &origZ);

	// set up the projected point
	GLfloat winX = (float)x;
	GLfloat winY = (float)viewport[3] - (float)y;
	GLfloat winZ = origZ;
	
	// unproject the image plane coordinate to the model space
	GLdouble posX, posY, posZ;
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	result.setX(posX);
	result.setY(posY);
}

/**
 * Round the position to a grid (200m x 200m)
 */
void GLWidget::snap(QVector2D &pos) {
	if (pos.x() > 0) {
		pos.setX(((int)pos.x() + 100) / 200 * 200);
	} else {
		pos.setX(((int)pos.x() - 100) / 200 * 200);
	}
	if (pos.y() > 0) {
		pos.setY(((int)pos.y() + 100) / 200 * 200);
	} else {
		pos.setY(((int)pos.y() - 100) / 200 * 200);
	}
}
