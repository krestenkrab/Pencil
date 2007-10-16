/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include <QtGui>
#include <math.h>
#include "scribblearea.h"
#include "beziercurve.h"
#include "editor.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layercamera.h"

#include "bitmapimage.h"

void VectorSelection::clear() {
	vertex.clear();
	curve.clear();
}

void VectorSelection::add(int curveNumber) {
	curve << curveNumber;
}

void VectorSelection::add(QList<int> list) {
	if(list.size() > 0) add(list[0]);
	/*for(int i=0; i<list.size(); i++) {
		add(list[i]);
	}*/
}

void VectorSelection::add(VertexRef point) {
	vertex << point;
	add(point.curveNumber);
}

void VectorSelection::add(QList<VertexRef> list) {
	if(list.size() > 0) add(list[0]);
	/*for(int i=0; i<list.size(); i++) {
		add(list[i]);
	}*/
}

ScribbleArea::ScribbleArea(QWidget *parent, Editor* editor)
    : QWidget(parent)
//ScribbleArea::ScribbleArea(QWidget *parent, Editor* editor)
//    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{	
	this->editor = editor;
	QSettings settings("Pencil","Pencil");
	
	pencil.width = settings.value("pencilWidth").toDouble();
	if (pencil.width == 0) { pencil.width = 1; settings.setValue("pencilWidth", pencil.width); }
	pen.width = settings.value("penWidth").toDouble();
	if (pen.width == 0) { pen.width = 2; settings.setValue("penWidth", pen.width); }
	brush.width = settings.value("brushWidth").toDouble();
	if (brush.width == 0) { brush.width = 48; settings.setValue("brushWidth", brush.width); }
	eraser.width = settings.value("eraserWidth").toDouble();
	if (eraser.width == 0) { eraser.width = 24; settings.setValue("brushWidth", brush.width); }
	currentWidth = pencil.width;
	
	pencil.colour = Qt::black;
	pen.colour = Qt::black;
	brush.colour = Qt::blue;
	currentColour = pencil.colour;
	
	pencil.colourNumber = 0;
	pen.colourNumber = 0;
	brush.colourNumber = 1;
	
	pencil.feather = 0;
	pen.feather = 0;
	brush.feather = 0.5;
	eraser.feather = 0;
	
	pencil.opacity = 0.8;
	pen.opacity = 1;
	brush.opacity = 0.5;
	eraser.opacity = 0.5;
	
	pencil.pressure = 1;
	pen.pressure = 1;
	brush.pressure = 1;
	eraser.pressure = 1;
	
	pencil.invisibility = 1;
	pen.invisibility = 0;
	brush.invisibility = -1;
	eraser.invisibility = -1;
	
	curveOpacity = (100-settings.value("curveOpacity").toInt())/100.0; // default value is 1.0
	int curveSmoothingLevel = settings.value("curveSmoothing").toInt();
	if(curveSmoothingLevel == 0) { curveSmoothingLevel = 20; settings.setValue("curveSmoothing", curveSmoothingLevel); } // default
	curveSmoothing = curveSmoothingLevel/20.0; // default value is 1.0
	
	highResPosition = false; // default is false (because it does not work on Windows)
	if( settings.value("highResPosition").toString() == "true") highResPosition = true;
	antialiasing = true; // default value is true (because it's prettier)
	if( settings.value("antialiasing").toString() == "false") antialiasing = false;
	shadows = true; // default value is true (because it's prettier)
	if( settings.value("shadows").toString() == "false") shadows = false;
	toolCursors = true; // default value is true
	if( settings.value("toolCursors").toString() == "false") toolCursors = false;
	gradients = 2;
	if( settings.value("gradients").toString() != "") gradients = settings.value("gradients").toInt();;
	
	toolMode = PENCIL;
	tabletEraser=false;
	tabletInUse=false;
	tabletPressure=1.0;
	setAttribute(Qt::WA_StaticContents); // ?
	modified = false;
	simplified = false;
	usePressure = true;
	makeInvisible = false;
	somethingSelected = false;
	setCursor(Qt::CrossCursor);
	onionPrev = true;
	onionNext = false;
	showThinLines = false;
	showAllLayers = 1;
	myView = QMatrix(); // identity matrix
	myTempView = QMatrix();
	transMatrix = QMatrix();
	centralView = QMatrix();
	
	QString background = settings.value("background").toString();
	if(background == "") background = "white";
	setBackgroundBrush(background);
	bufferImg = new BitmapImage(NULL);
	eyedropperCursor = NULL;
	
	QRect newSelection(QPoint(0,0), QSize(0,0));
	mySelection =  newSelection;
	myTransformedSelection = newSelection;
	myTempTransformedSelection = newSelection;
	offset.setX(0);
	offset.setY(0);
	selectionTransformation.reset();
	
	tol = 7.0;
	
	readCanvasFromCache = true;
	mouseInUse = false;
	setMouseTracking(true); // reacts to mouse move events, even if the button is not pressed
	
	debugRect = QRectF(0,0,0,0);
	
	setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding) );
	QPixmapCache::setCacheLimit(30*2*1024);
	setAttribute(Qt::WA_OpaquePaintEvent, false);
	setAttribute(Qt::WA_NoSystemBackground, true);
}

void ScribbleArea::setColour(const int i)
{
	if(toolMode == PENCIL) {
		pencil.colourNumber = i;
		pencil.colour = editor->object->getColour(i).colour;
	}
	if(toolMode == PEN || toolMode == POLYLINE) {
		pen.colourNumber = i;
		pen.colour = editor->object->getColour(i).colour;
	}
	if(toolMode == BUCKET || toolMode == COLOURING || toolMode == EYEDROPPER) {
		brush.colourNumber = i;
		brush.colour = editor->object->getColour(i).colour;
	}
	updateCursor();
	updateFrame();
}

void ScribbleArea::setColour(const QColor colour)
{
	if(toolMode == PENCIL || toolMode == EYEDROPPER) {
		pencil.colour = colour;
	}
	if(toolMode == PEN || toolMode == EYEDROPPER) {
		pen.colour = colour;
	}
	if(toolMode == COLOURING || toolMode == BUCKET || toolMode == EYEDROPPER) {
		brush.colour = colour;
	}
	currentColour = colour;
	updateCursor();
}

void ScribbleArea::resetColours()
{
	pen.colourNumber = 0;
	pencil.colourNumber = 0;
	brush.colourNumber = 1;
}

void ScribbleArea::setWidth(const qreal newWidth)
{
	QSettings settings("Pencil","Pencil");
	if(toolMode == PENCIL) {
		pencil.width = newWidth;
		settings.setValue("pencilWidth", newWidth);
	}
	if(toolMode == ERASER) {
		eraser.width = newWidth;
		settings.setValue("eraserWidth", newWidth);
	}
	if(toolMode == PEN || toolMode == POLYLINE) {
		pen.width = newWidth;
		settings.setValue("penWidth", newWidth);
	}
	if(toolMode == COLOURING) {
		brush.width = newWidth;
		settings.setValue("brushWidth", newWidth);
	}
	currentWidth = newWidth;
	updateAllFrames();
	updateCursor();
}

void ScribbleArea::setFeather(const qreal newFeather)
{
	QSettings settings("Pencil","Pencil");
	if(toolMode == PENCIL) {
		pencil.feather = newFeather;
		settings.setValue("pencilOpacity", newFeather);
	}
	if(toolMode == PEN || toolMode == POLYLINE) {
		pen.feather = newFeather;
		settings.setValue("penOpacity", newFeather);
	}
	if(toolMode == COLOURING) {
		brush.feather = newFeather;
		settings.setValue("brushOpacity", newFeather);
	}
	//currentWidth = newWidth;
	updateAllFrames();
	updateCursor();
}

void ScribbleArea::setOpacity(const qreal newOpacity)
{
	QSettings settings("Pencil","Pencil");
	if(toolMode == PENCIL) {
		pencil.opacity = newOpacity;
		settings.setValue("pencilOpacity", newOpacity);
	}
	if(toolMode == PEN || toolMode == POLYLINE) {
		pen.opacity = newOpacity;
		settings.setValue("penOpacity", newOpacity);
	}
	if(toolMode == COLOURING) {
		brush.opacity = newOpacity;
		settings.setValue("brushOpacity", newOpacity);
	}
	//currentWidth = newWidth;
	updateAllFrames();
}

void ScribbleArea::setInvisibility(const bool invisibility)
{
	QSettings settings("Pencil","Pencil");
	if(toolMode == PENCIL) {
		pencil.invisibility = invisibility;
		settings.setValue("pencilOpacity", invisibility);
	}
	if(toolMode == PEN || toolMode == POLYLINE) {
		pen.invisibility = invisibility;
		settings.setValue("penOpacity", invisibility);
	}
	makeInvisible = invisibility;
	updateAllFrames();
}

void ScribbleArea::setPressure(const bool pressure)
{
	QSettings settings("Pencil","Pencil");
	if(toolMode == PENCIL) {
		pencil.pressure = pressure;
		settings.setValue("pencilOpacity", pressure);
	}
	if(toolMode == PEN || toolMode == POLYLINE) {
		pen.pressure = pressure;
		settings.setValue("penOpacity", pressure);
	}
	if(toolMode == COLOURING) {
		brush.pressure = pressure;
		settings.setValue("brushOpacity", pressure);
	}
	usePressure = pressure;
	updateAllFrames();
}

void ScribbleArea::setCurveOpacity(int newOpacity)
{
	curveOpacity = newOpacity/100.0;
	QSettings settings("Pencil","Pencil");
	settings.setValue("curveOpacity", 100-newOpacity);
	updateAllVectorLayers();
}

void ScribbleArea::setCurveSmoothing(int newSmoothingLevel)
{
	curveSmoothing = newSmoothingLevel/20.0;
	QSettings settings("Pencil","Pencil");
	settings.setValue("curveSmoothing", newSmoothingLevel);
}

void ScribbleArea::setHighResPosition(int x)
{
	QSettings settings("Pencil","Pencil");
	if (x==0) { highResPosition=false; settings.setValue("highResPosition","false"); }
	else { highResPosition=true; settings.setValue("highResPosition","true"); }
}

void ScribbleArea::setAntialiasing(int x)
{
	QSettings settings("Pencil","Pencil");
	if (x==0) { antialiasing=false; settings.setValue("antialiasing","false"); }
	else { antialiasing=true; settings.setValue("antialiasing","true"); }
	updateAllVectorLayers();
}

void ScribbleArea::setGradients(int x)
{
	//if(x==0) { gradients = x; } else { gradients = x; }
	QSettings settings("Pencil","Pencil");
	if(x >= 0) {
		gradients = x;
		settings.setValue("gradients", gradients);
	} else {
		gradients = settings.value("gradients").toInt();
	}
	//if (x==0) { antialiasing=false; settings.setValue("antialiasing","false"); }
	//else { antialiasing=true; settings.setValue("antialiasing","true"); }
	updateAllVectorLayers();
}

void ScribbleArea::setShadows(int x)
{
	QSettings settings("Pencil","Pencil");
	if (x==0) { shadows=false; settings.setValue("shadows","false"); }
	else { shadows=true; settings.setValue("shadows","true"); }
	update();
}

void ScribbleArea::setToolCursors(int x)
{
	QSettings settings("Pencil","Pencil");
	if (x==0) { toolCursors=false; settings.setValue("toolCursors","false"); }
	else { toolCursors=true; settings.setValue("toolCursors","true"); }
}

void ScribbleArea::setStyle(int x)
{
	QSettings settings("Pencil","Pencil");
	if (x==0) { settings.setValue("style","default"); }
	else { settings.setValue("style","aqua"); }
	update();
}

void ScribbleArea::setBackground(int number)
{
	if(number == 1) setBackgroundBrush("checkerboard");
	if(number == 2) setBackgroundBrush("white");
	if(number == 3) setBackgroundBrush("grey");
	if(number == 4) setBackgroundBrush("dots");
	if(number == 5) setBackgroundBrush("weave");
	updateAllFrames();
}

void ScribbleArea::setBackgroundBrush(QString brushName)
{
	QSettings settings("Pencil","Pencil");
	settings.setValue("background", brushName);
	backgroundBrush = getBackgroundBrush(brushName);
}

QBrush ScribbleArea::getBackgroundBrush(QString brushName)
{
	QBrush brush = QBrush(Qt::white);
	if(brushName == "white") {
		brush = QBrush(Qt::white);
	}
	if(brushName == "grey") {
		brush = QBrush(Qt::lightGray);
	}
	if(brushName == "checkerboard") {
		QPixmap pattern(16,16);
		pattern.fill( QColor(255,255,255) );
		QPainter painter(&pattern);
		painter.fillRect( QRect(0,0,8,8), QColor(220,220,220) );
		painter.fillRect( QRect(8,8,8,8), QColor(220,220,220) );
		painter.end();
		brush.setTexture(pattern);
	}
	if(brushName == "dots") {
		QPixmap pattern(":background/dots.png");
		brush.setTexture(pattern);
	}
	if(brushName == "weave") {
		QPixmap pattern(":background/weave.jpg");
		brush.setTexture(pattern);
	}
	return brush;
}

void ScribbleArea::updateFrame() {
	updateFrame(editor->currentFrame);
}

void ScribbleArea::updateFrame(int frame) {
	//qDebug() << "updateFrame";
	//paintCanvas(frame);
	setView();
	int frameNumber = editor->getLastFrameAtFrame( frame );
	QPixmapCache::remove("frame"+QString::number(frameNumber));
	readCanvasFromCache = true;
	update();
}

void ScribbleArea::updateAllFrames() {
	//qDebug() << "updateAllFrames";
	//frameList.clear();
	setView();
	QPixmapCache::clear();
	readCanvasFromCache = true;
	update();
}

void ScribbleArea::updateAllVectorLayersAtCurrentFrame() {
	updateAllVectorLayersAt(editor->currentFrame);
}

void ScribbleArea::updateAllVectorLayersAt(int frameNumber) {
	for(int i=0; i< editor->object->getLayerCount(); i++) {
		Layer* layer = editor->object->getLayer(i);
		if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(frameNumber, 0)->setModified(true);
	}
	updateFrame(editor->currentFrame);
}


void ScribbleArea::updateAllVectorLayers() {
	for(int i=0; i< editor->object->getLayerCount(); i++) {
		Layer* layer = editor->object->getLayer(i);
		if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->setModified(true);
	}
	updateAllFrames();
}

void ScribbleArea::setModified(int layerNumber, int frameNumber) {
	Layer* layer = editor->object->getLayer(layerNumber);
	//if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(frameNumber, 0)->setModified(true);
	if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->setModified(frameNumber, true);
	if(layer->type == Layer::BITMAP) ((LayerImage*)layer)->setModified(frameNumber, true);
	emit modification(layerNumber);
	//updateFrame(frame);
	updateAllFrames();
}

void ScribbleArea::escape() {
	deselectAll();
}

void ScribbleArea::keyPressEvent( QKeyEvent *event ) {
	switch (event->key()) {
	  case Qt::Key_Right:
		if(somethingSelected) { myTempTransformedSelection.translate(1,0); myTransformedSelection = myTempTransformedSelection; calculateSelectionTransformation(); update(); } else { editor->scrubForward(); event->ignore(); }
		break;
	case Qt::Key_Left:
		if(somethingSelected) { myTempTransformedSelection.translate(-1,0);  myTransformedSelection = myTempTransformedSelection; calculateSelectionTransformation(); update(); } else { editor->scrubBackward(); event->ignore(); }
		break;
	case Qt::Key_Up:
		if(somethingSelected) { myTempTransformedSelection.translate(0,-1); myTransformedSelection = myTempTransformedSelection; calculateSelectionTransformation(); update(); } else { editor->previousLayer(); event->ignore(); }
		break;
	case Qt::Key_Down:
		if(somethingSelected) { myTempTransformedSelection.translate(0,1); myTransformedSelection = myTempTransformedSelection; calculateSelectionTransformation(); update(); } else { editor->nextLayer(); event->ignore(); }
		break;
	case Qt::Key_Return:
		if(somethingSelected) { paintTransformedSelection(); deselectAll(); } else {
			if(toolMode == POLYLINE) {
				endPolyline();
			} else {
				event->ignore();
			}
		}
		break;
	case Qt::Key_Escape:
		if(somethingSelected || toolMode == POLYLINE) { escape(); }
		break;
	case Qt::Key_Backspace:
		if(somethingSelected) deleteSelection();
		break;
	case Qt::Key_F1:
		simplified = true;
		emit outlinesChanged(simplified);
		gradients = 0;
		updateAllVectorLayersAtCurrentFrame();
		break;
	case Qt::Key_F2:
		gradients = 1;
		updateAllVectorLayersAtCurrentFrame();
		break;
	case Qt::Key_F3:
		gradients = 2;
		updateAllVectorLayersAtCurrentFrame();
		break;
	default:
		event->ignore(); 
		//QWidget::keyPressEvent(e);
	}
}

void ScribbleArea::keyReleaseEvent( QKeyEvent *event ) {
	switch (event->key()) {
	case Qt::Key_F1:
		simplified = false;
		emit outlinesChanged(simplified);
		setGradients(-1);
		updateAllVectorLayersAtCurrentFrame();
		break;
	case Qt::Key_F2:
		setGradients(-1);
		updateAllVectorLayersAtCurrentFrame();
		break;
	case Qt::Key_F3:
		setGradients(-1);
		updateAllVectorLayersAtCurrentFrame();
		break;
	default:
		event->ignore(); 
		//QWidget::keyPressEvent(e);
	}
}

void ScribbleArea::tabletEvent(QTabletEvent *event)
{
	//qDebug() << "Device" << event->device() << "Pointer type" << event->pointerType();
	tabletInUse = true;
	tabletPosition = event->hiResGlobalPos();
	tabletPressure = event->pressure();
	if(event->pointerType() == QTabletEvent::Cursor) tabletPressure = 0.5*tabletPressure; // "normal" pressure is chosen to be 0.5 instead of 1.0
	mousePressure.append(tabletPressure);
	if(toolMode==ScribbleArea::ERASER) {
		//myPenWidth = static_cast<int>(10.0*tabletPressure);
		currentWidth = (eraser.width*tabletPressure);
	}
	if(toolMode==ScribbleArea::PENCIL) {
		currentColour = pencil.colour;
		if(usePressure) {currentColour.setAlphaF(pencil.colour.alphaF()*tabletPressure); } else { currentColour.setAlphaF(pencil.colour.alphaF()); }
		currentWidth = pencil.width;
	}
	if(toolMode==ScribbleArea::PEN) {
		currentColour = pen.colour;
		currentColour.setAlphaF(pen.colour.alphaF());
		if(usePressure) { currentWidth = 2.0*pen.width*tabletPressure; } else { currentWidth = pen.width; }
	}
	if(event->pointerType() == QTabletEvent::Eraser) {
		if(tabletEraser == false) eraserOn();
		tabletEraser = true;
	} else {
		if(tabletEraser == true) pencilOn();
		tabletEraser = false;
	}
	event->ignore(); // indicates that the tablet event is not accepted yet, so that it is propagated as a mouse event)
}

void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
	mouseInUse = true;
	
	Layer* layer = editor->getCurrentLayer();
	// ---- checks ------
	if(layer==NULL) return;
	if(!layer->visible && toolMode != HAND && (event->button() != Qt::RightButton)) {
		QMessageBox::warning(this, tr("Warning"),
				tr("You are drawing on a hidden layer! Please select another layer (or make the current layer visible)."),
				QMessageBox::Ok,
				QMessageBox::Ok);
		mouseInUse = false;
		return;
	}
	if(layer->type == Layer::VECTOR) {
		VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
		if (vectorImage == NULL) return;
		if(toolMode == ScribbleArea::PENCIL) editor->selectColour(pencil.colourNumber);
		if(toolMode == ScribbleArea::PEN) editor->selectColour(pen.colourNumber);
		if(toolMode == ScribbleArea::COLOURING || toolMode == ScribbleArea::BUCKET) editor->selectColour(brush.colourNumber);
	}
	if(layer->type == Layer::BITMAP) {
		BitmapImage* bitmapImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
		if (bitmapImage == NULL) return;
	}
	// --- end checks ----
	
	while(!mousePath.isEmpty()) mousePath.removeAt(0); // empty the mousePath
	while(!mousePressure.isEmpty()) mousePressure.removeAt(0); // empty the mousePressure
	if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {  // if the user is pressing the left or right button
		if(tabletInUse && highResPosition) { lastPixel = QPointF(event->pos()) + tabletPosition - QPointF(event->globalPos()); } else { lastPixel = QPointF(event->pos()); }
		bool invertible = true;
		lastPoint = myTempView.inverted(&invertible).map(QPointF(lastPixel));
		lastBrushPoint = lastPoint;
	}
	
	if (event->button() == Qt::LeftButton) { 
		if(toolMode == ScribbleArea::PENCIL || toolMode == ScribbleArea::ERASER || toolMode == ScribbleArea::PEN || toolMode == ScribbleArea::BUCKET || toolMode == ScribbleArea::COLOURING) {
			
			editor->backup();
			
			if(layer->type == Layer::VECTOR) {
				if(toolMode == ScribbleArea::PENCIL && !showThinLines) toggleThinLines();
			}
			mousePath.append(lastPoint);
		}
		// ----------------------------------------------------------------------
		if(toolMode == COLOURING) {
			if(layer->type == Layer::BITMAP) {
				qreal opacity = 1.0;
				qreal brushWidth = brush.width;
				if(tabletInUse) opacity = tabletPressure;
				if(usePressure) brushWidth = brush.width*tabletPressure;
				drawBrush( lastPoint, brushWidth, brush.colour, opacity);
				int rad = qRound(brushWidth / 2) + 3;
				update(myTempView.mapRect(QRect(lastPoint.toPoint(), lastPoint.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad)));
			}
		}		
		// ----------------------------------------------------------------------
		if(toolMode == POLYLINE && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR) ) {
			if(mousePoints.size() == 0) editor->backup();
			if(layer->type == Layer::VECTOR) {
				((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->deselectAll();
				if(makeInvisible && !showThinLines) toggleThinLines();
			}
			mousePoints << lastPoint;
			//qDebug() << "--------- " << mousePoints;
		}
		// ----------------------------------------------------------------------
		if(toolMode == ScribbleArea::SELECT && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)) {
			if(layer->type == Layer::VECTOR) {
				((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->deselectAll();
			}
			moveMode = ScribbleArea::MIDDLE;
			if( somethingSelected ) {  // there is something selected
				if( BezierCurve::mLength(lastPoint - myTransformedSelection.topLeft()) < 6) moveMode = ScribbleArea::TOPLEFT;
				if( BezierCurve::mLength(lastPoint - myTransformedSelection.topRight()) < 6) moveMode = ScribbleArea::TOPRIGHT;
				if( BezierCurve::mLength(lastPoint - myTransformedSelection.bottomLeft()) < 6) moveMode = ScribbleArea::BOTTOMLEFT;
				if( BezierCurve::mLength(lastPoint - myTransformedSelection.bottomRight()) < 6) moveMode = ScribbleArea::BOTTOMRIGHT;
				if( moveMode == ScribbleArea::MIDDLE ) { paintTransformedSelection(); deselectAll(); } // the user did not click on one of the corners
			} else { // there is nothing selected
				mySelection.setTopLeft( lastPoint );
				mySelection.setBottomRight( lastPoint );
				setSelection(mySelection, true);
			}
			update();
		}
		// ----------------------------------------------------------------------
		if(toolMode == ScribbleArea::EDIT) {
			if(layer->type == Layer::VECTOR) {
				closestCurves = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->getCurvesCloseTo(currentPoint, tol/myTempView.m11());
				closestVertices = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->getVerticesCloseTo(currentPoint, tol/myTempView.m11());
				if( closestVertices.size() > 0 || closestCurves.size() > 0 ) {  // the user clicks near a vertex or a curve
					editor->backup();
					VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
					if(event->modifiers() != Qt::ShiftModifier && !vectorImage->isSelected(closestVertices)) { paintTransformedSelection(); deselectAll(); }
					vectorImage->setSelected(closestVertices, true);
					vectorSelection.add(closestCurves);
					vectorSelection.add(closestVertices);
					/*calculateSelectionRect();
					moveMode == ScribbleArea::MIDDLE;
					if( somethingSelected ) {  // there is an area selection
						if( BezierCurve::mLength(lastPoint - myTransformedSelection.topLeft()) < 6) moveMode = ScribbleArea::TOPLEFT;
						if( BezierCurve::mLength(lastPoint - myTransformedSelection.topRight()) < 6) moveMode = ScribbleArea::TOPRIGHT;
						if( BezierCurve::mLength(lastPoint - myTransformedSelection.bottomLeft()) < 6) moveMode = ScribbleArea::BOTTOMLEFT;
						if( BezierCurve::mLength(lastPoint - myTransformedSelection.bottomRight()) < 6) moveMode = ScribbleArea::BOTTOMRIGHT;
					}*/
					update();
				} else {
					deselectAll();
				}
			}
		}
		// ----------------------------------------------------------------------
		if(toolMode == ScribbleArea::MOVE && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)) {
			editor->backup();
			moveMode = ScribbleArea::MIDDLE;
			if( somethingSelected ) {  // there is an area selection
				if( BezierCurve::mLength(lastPoint - myTransformedSelection.topLeft()) < 6) moveMode = ScribbleArea::TOPLEFT;
				if( BezierCurve::mLength(lastPoint - myTransformedSelection.topRight()) < 6) moveMode = ScribbleArea::TOPRIGHT;
				if( BezierCurve::mLength(lastPoint - myTransformedSelection.bottomLeft()) < 6) moveMode = ScribbleArea::BOTTOMLEFT;
				if( BezierCurve::mLength(lastPoint - myTransformedSelection.bottomRight()) < 6) moveMode = ScribbleArea::BOTTOMRIGHT;
			}
			// ---
			if(moveMode == ScribbleArea::MIDDLE) {
				if(layer->type == Layer::BITMAP) {
					if(!(myTransformedSelection.contains(lastPoint))) {  // click is outside the transformed selection with the MOVE tool
						paintTransformedSelection(); deselectAll();
					}
				}
				if(layer->type == Layer::VECTOR) {
					VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
					if(closestCurves.size() > 0) { // the user clicks near a curve
						//	editor->backup();
						if(!vectorImage->isSelected(closestCurves)) {
							paintTransformedSelection();
							if(event->modifiers() != Qt::ShiftModifier) { deselectAll(); }
							vectorImage->setSelected(closestCurves, true);
							setSelection( vectorImage->getSelectionRect(), true );
							update();
						}
					} else {
						int areaNumber = vectorImage->getLastAreaNumber(lastPoint);
						if(areaNumber != -1) { // the user clicks on an area
							if(!vectorImage->isAreaSelected(areaNumber)) {
								if(event->modifiers() != Qt::ShiftModifier) { deselectAll(); }
								vectorImage->setAreaSelected(areaNumber, true);
								//setSelection( vectorImage->getSelectionRect() );
								setSelection( QRectF(0,0,0,0), true );
								update();
							}
						}	else { // the user doesn't click near a curve or an area
							if(!(myTransformedSelection.contains(lastPoint))) {  // click is outside the transformed selection with the MOVE tool
								paintTransformedSelection(); deselectAll();
							}
						}
					}
				}
			}
			// ---
						
		}
	}
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event)
{
	Layer* layer = editor->getCurrentLayer();
	// ---- checks ------
	if(layer==NULL) return;
	if(layer->type == Layer::VECTOR) {
		VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
		if (vectorImage == NULL) return;
	}
	if(layer->type == Layer::BITMAP) {
		BitmapImage* bitmapImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
		if (bitmapImage == NULL) return;
	}
	// ---- end checks ------
	
	if(tabletInUse  && highResPosition) { currentPixel = QPointF(event->pos()) + tabletPosition - QPointF(event->globalPos()); } else { currentPixel = event->pos(); }
	bool invertible = true;
	currentPoint = myTempView.inverted(&invertible).map(QPointF(currentPixel));
	if(event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton) { // the user is also pressing the mouse (dragging)
		offset = currentPoint - lastPoint;
		mousePath.append(currentPoint);
	}
	
	// ----------------------------------------------------------------------
	if(layer->type == Layer::BITMAP || layer->type == Layer::VECTOR) {
		if ((event->buttons() & Qt::LeftButton) && (toolMode == ScribbleArea::PENCIL || toolMode == ScribbleArea::ERASER || toolMode == ScribbleArea::PEN || toolMode == ScribbleArea::COLOURING))
			drawLineTo(currentPixel, currentPoint);
		if(toolMode == ScribbleArea::POLYLINE)
			drawPolyline();
	}
	// ----------------------------------------------------------------------
	if (toolMode == ScribbleArea::SELECT && (event->buttons() & Qt::LeftButton) && somethingSelected && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)) {
		if(moveMode == ScribbleArea::MIDDLE) mySelection.setBottomRight(currentPoint);
		if(moveMode == ScribbleArea::TOPLEFT) mySelection.setTopLeft(currentPoint);
		if(moveMode == ScribbleArea::TOPRIGHT) mySelection.setTopRight(currentPoint);
		if(moveMode == ScribbleArea::BOTTOMLEFT) mySelection.setBottomLeft(currentPoint);
		if(moveMode == ScribbleArea::BOTTOMRIGHT) mySelection.setBottomRight(currentPoint);
		myTransformedSelection = mySelection.adjusted(0,0,0,0);
		myTempTransformedSelection = mySelection.adjusted(0,0,0,0);
		if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->select(mySelection);
		update();
	}
	// ----------------------------------------------------------------------
	if (toolMode == ScribbleArea::ERASER && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)) {
		if(event->buttons() & Qt::LeftButton) { // the user is also pressing the mouse (dragging)
			if(layer->type == Layer::VECTOR) {
				qreal radius = (eraser.width/2)/myTempView.m11();
				QList<VertexRef> nearbyVertices = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->getVerticesCloseTo(currentPoint, radius);
				for(int i=0; i< nearbyVertices.size(); i++) {
					((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->setSelected(nearbyVertices.at(i), true);
				}
				//update();
			}
		}
	}
	// ----------------------------------------------------------------------
	if (toolMode == ScribbleArea::EDIT && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)) {
		if(event->buttons() & Qt::LeftButton) { // the user is also pressing the mouse (dragging) {
			if( layer->type == Layer::VECTOR) {
				if( event->modifiers() != Qt::ShiftModifier) { // (and the user doesn't press shift)
					// transforms the selection
					selectionTransformation = QMatrix().translate(offset.x(), offset.y());
					((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->setSelectionTransformation(selectionTransformation);
					/*if(moveMode == ScribbleArea::MIDDLE) {
						if(QLineF(lastPixel,currentPixel).length()>4) myTempTransformedSelection = myTransformedSelection.translated(offset);
					}
					if(moveMode == ScribbleArea::TOPLEFT) myTempTransformedSelection = myTransformedSelection.adjusted(offset.x(), offset.y(), 0, 0);
					if(moveMode == ScribbleArea::TOPRIGHT) myTempTransformedSelection = myTransformedSelection.adjusted(0, offset.y(), offset.x(), 0);
					if(moveMode == ScribbleArea::BOTTOMLEFT) myTempTransformedSelection = myTransformedSelection.adjusted(offset.x(), 0, 0, offset.y());
					if(moveMode == ScribbleArea::BOTTOMRIGHT) myTempTransformedSelection = myTransformedSelection.adjusted(0, 0, offset.x(), offset.y());
					calculateSelectionTransformation();*/
					//update();
				}
			}
		} else { // the user is moving the mouse without pressing it
			if(layer->type == Layer::VECTOR)  {
				closestVertices = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->getVerticesCloseTo(currentPoint, tol/myTempView.m11());
			}
			//update();
		}
		update();
	}
	// ----------------------------------------------------------------------
	if (toolMode == ScribbleArea::MOVE  && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)) {
		if(event->buttons() & Qt::LeftButton) { // the user is also pressing the mouse (dragging)
			if( somethingSelected) {  // there is something selected
				if( event->modifiers() != Qt::ShiftModifier) { // (and the user doesn't press shift)
					// transforms the selection
					if(moveMode == ScribbleArea::MIDDLE) {
						if(QLineF(lastPixel,currentPixel).length()>4) myTempTransformedSelection = myTransformedSelection.translated(offset);
					}
					if(moveMode == ScribbleArea::TOPLEFT) myTempTransformedSelection = myTransformedSelection.adjusted(offset.x(), offset.y(), 0, 0);
					if(moveMode == ScribbleArea::TOPRIGHT) myTempTransformedSelection = myTransformedSelection.adjusted(0, offset.y(), offset.x(), 0);
					if(moveMode == ScribbleArea::BOTTOMLEFT) myTempTransformedSelection = myTransformedSelection.adjusted(offset.x(), 0, 0, offset.y());
					if(moveMode == ScribbleArea::BOTTOMRIGHT) myTempTransformedSelection = myTransformedSelection.adjusted(0, 0, offset.x(), offset.y());
					calculateSelectionTransformation();
					update();
				}
			} else { // there is nothing selected
				//selectionTransformation = selectionTransformation.translate(offset.x(), offset.y());
				//if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->setSelectionTransformation(selectionTransformation);
				
				//VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
				//setSelection( vectorImage->getSelectionRect() );
				
				// we switch to the select tool
				editor->getToolSet()->changeSelectButton();
				selectOn();
				moveMode = ScribbleArea::MIDDLE;
				mySelection.setTopLeft( lastPoint );
				mySelection.setBottomRight( lastPoint );
				setSelection(mySelection, true);
			}
		} else { // the user is moving the mouse without pressing it
			if(layer->type == Layer::VECTOR)  {
				closestCurves = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->getCurvesCloseTo(currentPoint, tol/myTempView.m11());
			}
			update();
		}
	}
	// ----------------------------------------------------------------------
	if (  toolMode == ScribbleArea::HAND && (event->buttons() != Qt::NoButton || event->buttons() & Qt::RightButton) ) {
		if(event->modifiers() & Qt::ControlModifier || event->modifiers() & Qt::AltModifier || event->buttons() & Qt::RightButton) {
			QPoint centralPixel(width()/2, height()/2);
			if(lastPixel.x() != centralPixel.x()) {
				qreal scale = 1.0;
				qreal cosine = 1.0;
				qreal sine = 0.0;
				if( event->modifiers() & Qt::AltModifier) { // rotation
					QPointF V1 = lastPixel-centralPixel;
					QPointF V2 = currentPixel-centralPixel;
					cosine = ( V1.x()*V2.x() + V1.y()*V2.y()  )/(BezierCurve::eLength(V1)*BezierCurve::eLength(V2));
					sine = ( -V1.x()*V2.y() + V1.y()*V2.x()  )/(BezierCurve::eLength(V1)*BezierCurve::eLength(V2));
					
				}
				if( event->modifiers() & Qt::ControlModifier || event->buttons() & Qt::RightButton) { // scale
					scale = exp( 0.01*( currentPixel.y()-lastPixel.y() ) );
				}
				transMatrix = QMatrix(
					scale*cosine, -scale*sine,
					scale*sine,  scale*cosine,
					0.0,
					0.0
				);
			}
		} else { // translation
			transMatrix.setMatrix(1.0,0.0,0.0,1.0, currentPixel.x()-lastPixel.x(), currentPixel.y()-lastPixel.y());
		}
		update();
	}
	// ----------------------------------------------------------------------
	if( toolMode == EYEDROPPER ) {
		if(layer->type == Layer::BITMAP) {
			BitmapImage* targetImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
			if( targetImage->contains(currentPoint) ) {
				QColor pickedColour = targetImage->pixel(currentPoint.x(), currentPoint.y());
				if(pickedColour.alpha() != 0) drawEyedropperPreview(pickedColour);
			}
		}
		if(layer->type == Layer::VECTOR) {
			VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
			int colourNumber = vectorImage->getColourNumber(currentPoint);
			if( colourNumber != -1) {
				drawEyedropperPreview( editor->object->getColour(colourNumber).colour );
			}
		}
	}

}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{	
	mouseInUse = false;
	
	Layer* layer = editor->getCurrentLayer();
	// ---- checks ------
	if(layer==NULL) return;
	if(layer->type == Layer::VECTOR) {
		VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
		if (vectorImage == NULL) return;
	}
	if(layer->type == Layer::BITMAP) {
		BitmapImage* bitmapImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
		if (bitmapImage == NULL) return;
	}
	// ---- end checks ------

	//currentWidth = myPenWidth;
	if(layer->type == Layer::BITMAP || layer->type == Layer::VECTOR) {
		if ((event->button() == Qt::LeftButton) && (toolMode == ScribbleArea::PENCIL || toolMode == ScribbleArea::ERASER || toolMode == ScribbleArea::PEN)) {
			drawLineTo(currentPixel, currentPoint);
		}
	}
		
	if(event->button() == Qt::LeftButton) {
		// ======================================================================
		if(layer->type == Layer::BITMAP) {
			// ----------------------------------------------------------------------
			if( toolMode == BUCKET) {
				BitmapImage* sourceImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
				Layer* targetLayer = layer; // by default
				int layerNumber = editor->currentLayer; // by default
				if( editor->currentLayer > 0) {
					Layer* layer2 = editor->getCurrentLayer(-1);
					if(layer2->type == Layer::BITMAP) {
						targetLayer = layer2;
						layerNumber = layerNumber - 1;
					}
				}
				BitmapImage* targetImage = ((LayerBitmap*)targetLayer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
				BitmapImage::floodFill( sourceImage, targetImage, lastPoint.toPoint(), qRgba(0,0,0,0), brush.colour.rgba(), 10*10);
				setModified(layerNumber, editor->currentFrame);
			}
			// ----------------------------------------------------------------------
			if( toolMode == EYEDROPPER) {
				BitmapImage* targetImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
				QColor pickedColour = targetImage->pixel(lastPoint.x(), lastPoint.y());
				if(pickedColour.alpha() != 0) editor->setColour( pickedColour );
			}
			// ----------------------------------------------------------------------
			if( toolMode == PENCIL || toolMode == PEN || toolMode == ERASER || toolMode == COLOURING ) {
				// Clear the temporary pixel path
				BitmapImage* targetImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
				if(toolMode == ERASER) {
					targetImage->paste(bufferImg, QPainter::CompositionMode_DestinationOut);
				} else {
					targetImage->paste(bufferImg);
				}
				QRect rect = myTempView.mapRect(bufferImg->boundaries);
				bufferImg->clear();
				
				//setModified(layer, editor->currentFrame);
				((LayerImage*)layer)->setModified(editor->currentFrame, true);
				emit modification();
				QPixmapCache::remove("frame"+QString::number(editor->currentFrame));
				readCanvasFromCache = false;
				updateCanvas(editor->currentFrame, rect);
				update(rect);
			}
		}
		
		// ======================================================================
		if(layer->type == Layer::VECTOR) {
			// ----------------------------------------------------------------------
			if( toolMode == BUCKET) {
				VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
				if(event->modifiers() == Qt::AltModifier) {
					vectorImage->removeArea(lastPoint);
				} else {
					floodFill(vectorImage, lastPixel.toPoint(), qRgba(0,0,0,0), qRgb(200,200,200), 100*100);
				}
				setModified(editor->currentLayer, editor->currentFrame);
			}
			// ----------------------------------------------------------------------
			if( toolMode == EYEDROPPER) {
				VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
				int colourNumber = vectorImage->getColourNumber(lastPoint);
				if( colourNumber != -1) {
					editor->selectColour( colourNumber );
				}
			}
			// ----------------------------------------------------------------------
			if( toolMode == COLOURING) {
				// Clear the temporary pixel path
				bufferImg->clear();
				((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->colour(mousePath, brush.colourNumber);
				//((LayerImage*)layer)->setModified(editor->currentFrame, true);
				//update();
				setModified(editor->currentLayer, editor->currentFrame);
			}
			// ----------------------------------------------------------------------
			if( (toolMode == ScribbleArea::PENCIL || toolMode == ScribbleArea::PEN) && mousePath.size() > -1 ) {
				// Clear the temporary pixel path
				bufferImg->clear();
				qreal tol = curveSmoothing/qAbs( myView.m11() );
				BezierCurve curve(mousePath, mousePressure, tol);
				if(toolMode == PEN) {
					curve.setWidth(pen.width);
					curve.setFeather(0);
					curve.setInvisibility(false);
					curve.setVariableWidth(usePressure);
					curve.setColourNumber(pen.colourNumber);
				} else {
					curve.setWidth(0);
					curve.setFeather(0);
					curve.setInvisibility(true);
					curve.setVariableWidth(false);
					curve.setColourNumber(pencil.colourNumber);
				}
				VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
				
				//curve.setSelected(true);
				//qDebug() << "this curve has " << curve.getVertexSize() << "vertices";
				
				vectorImage->addCurve(curve, qAbs(myView.m11()) );
				
				//if(layer->type == Layer::BITMAP || layer->type == Layer::VECTOR) ((LayerImage*)layer)->setModified(editor->currentFrame, true);
				//update();
				setModified(editor->currentLayer, editor->currentFrame);
			}
			// ----------------------------------------------------------------------
			if( toolMode == ScribbleArea::ERASER ) {
				VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
				// Clear the area containing the last point
				//vectorImage->removeArea(lastPoint);
				// Clear the temporary pixel path
				bufferImg->clear();
				vectorImage->deleteSelectedPoints();
				//update();
				setModified(editor->currentLayer, editor->currentFrame);
			}
		}
	}
	
	// ====================== for all kinds of layers =======================
	// ----------------------------------------------------------------------
	if ((toolMode == ScribbleArea::EDIT) && (event->button() == Qt::LeftButton)) {
		if(layer->type == Layer::VECTOR) {
			VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
			vectorImage->applySelectionTransformation();
			selectionTransformation.reset();
			for(int k=0; k<vectorSelection.curve.size(); k++) {
				int curveNumber = vectorSelection.curve.at(k);
				vectorImage->curve[curveNumber].smoothCurve();
			}
			setModified(editor->currentLayer, editor->currentFrame);
		}
	}
	if ((toolMode == ScribbleArea::MOVE) && (event->button() == Qt::LeftButton)  && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)) {
		offset.setX(0);
		offset.setY(0);
		calculateSelectionTransformation();
		//if(layer->type == Layer::VECTOR) {
			//	closestCurves = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->getCurvesCloseTo(currentPoint, tol/myView.m11());
			//	closestVertices = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->getVerticesCloseTo(currentPoint, tol/myView.m11());
		//}	
		//if(somethingSelected && layer->type == Layer::VECTOR) {
			//editor->backup();
			//((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->applySelectionTransformation();
		//}
		//if( (mySelection.width() > 1) || (mySelection.height() > 1) ) {
			myTransformedSelection = myTempTransformedSelection;
		//	update();
		//}
		//if(layer->type == Layer::BITMAP || layer->type == Layer::VECTOR) ((LayerImage*)layer)->setModified(editor->currentFrame, true);
		
		if(layer->type == Layer::VECTOR) {
			if(somethingSelected) {
				//VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
				//-- we should recalculate the intersections, once the curved are moved!
				//vectorImage->reinsertSelection();
			}
		}

		setModified(editor->currentLayer, editor->currentFrame);
	}
	// ----------------------------------------------------------------------
	if( toolMode == ScribbleArea::HAND || (event->button() == Qt::RightButton) ) {
		bufferImg->clear();
		if(layer->type == Layer::CAMERA) {
			LayerCamera* layerCamera = (LayerCamera*)layer;
			QMatrix view = layerCamera->getViewAtFrame(editor->currentFrame);
			layerCamera->loadImageAtFrame(editor->currentFrame, view * transMatrix);
			//Camera* camera = ((LayerCamera*)layer)->getLastCameraAtFrame(editor->currentFrame, 0);
			//camera->view = camera->view * transMatrix;
		} else {
			myView =  myView * transMatrix;
		}
		transMatrix.reset();
		updateAllVectorLayers();
		//update();
	}
	// ----------------------------------------------------------------------
	if( toolMode == ScribbleArea::SELECT && (event->button() == Qt::LeftButton)  && (layer->type == Layer::BITMAP || layer->type == Layer::VECTOR)) {
		if(layer->type == Layer::VECTOR) {
			if(somethingSelected) {
				editor->getToolSet()->changeMoveButton();
				moveOn();
				VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
				setSelection( vectorImage->getSelectionRect(), true );
				if(mySelection.size() == QSizeF(0,0)) somethingSelected = false;
			}
		}
		updateFrame();
	}
	// ----------------------------------------------------------------------
	//update();
	tabletInUse = false;
}

void ScribbleArea::mouseDoubleClickEvent(QMouseEvent *event)
{
	if( toolMode == ScribbleArea::HAND || event->button() == Qt::RightButton ) {
		resetView();
	} else {
		if( toolMode == ScribbleArea::POLYLINE && BezierCurve::eLength(lastPixel.toPoint() - event->pos()) < 2.0 ) {
			endPolyline();
		} else {
			mousePressEvent(event);
		}
	}
}

void ScribbleArea::paintEvent(QPaintEvent* event)
{
	//qDebug() << "paint event!" << QDateTime::currentDateTime() << event->rect(); //readCanvasFromCache << mouseInUse << editor->currentFrame;
	QPainter painter(this);
	
	// draws the background (if necessary)
	if(mouseInUse && toolMode == HAND) {
			painter.setWorldMatrix(myTempView);
			painter.setWorldMatrixEnabled(true);
			painter.setPen(Qt::NoPen);
			painter.setBrush(backgroundBrush);
			painter.drawRect(  (myTempView).inverted().mapRect( QRect(-2,-2, width()+3, height()+3) )  ); // this is necessary to have the background move with the view
	}
	
	// process the canvas (or not)
	if(!mouseInUse && readCanvasFromCache) {
		// --- we retrieve the canvas from the cache; we create it if it doesn't exist
		int frameNumber = editor->getLastFrameAtFrame( editor->currentFrame );
		if(!QPixmapCache::find("frame"+QString::number(frameNumber), canvas)) {
			updateCanvas(editor->currentFrame, event->rect());
			QPixmapCache::insert("frame"+QString::number(frameNumber), canvas);
		}
	}
	if(toolMode == MOVE) {
		Layer* layer = editor->getCurrentLayer();
		if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->setModified(true);
		updateCanvas(editor->currentFrame, event->rect());
	}
	// paints the canvas
	painter.setWorldMatrixEnabled(true);
	painter.setWorldMatrix(  centralView.inverted() * transMatrix * centralView  );
	painter.drawPixmap( QPoint(0,0), canvas );
	
	Layer* layer = editor->getCurrentLayer();
	if(!layer) return;
	
	if(!editor->playing) {  // we don't need to display the following when the animation is playing
		painter.setWorldMatrix(myTempView);
		
		if(layer->type == Layer::VECTOR) {
			VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
			
			if(toolMode == EDIT || toolMode == HAND) {
				//bufferImg->clear();
				painter.save();
				painter.setWorldMatrixEnabled(false);
				painter.setRenderHint(QPainter::Antialiasing, false);
				// ----- paints the edited elements
				QPen pen2(Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin);
				painter.setPen( pen2 );
				QColor colour;
				// ------------ vertices of the edited curves
				colour = QColor(200,200,200);
				painter.setBrush( colour );
				for(int k=0; k<vectorSelection.curve.size(); k++) {
					int curveNumber = vectorSelection.curve.at(k);
					//QPainterPath path = vectorImage->curve[curveNumber].getStrokedPath();
					//bufferImg->drawPath( myTempView.map(path), pen2, colour, QPainter::CompositionMode_SourceOver, false);
					for(int vertexNumber=-1; vertexNumber<vectorImage->getCurveSize(curveNumber); vertexNumber++) {
						QPointF vertexPoint = vectorImage->getVertex(curveNumber, vertexNumber);
						QRectF rectangle = QRectF( (myView*transMatrix*centralView).map(vertexPoint)-QPointF(3.0,3.0), QSizeF(7,7) ); 
						if(rect().contains( (myView*transMatrix*centralView).map(vertexPoint).toPoint())) {
							
							painter.drawRect( rectangle.toRect() );
							//bufferImg->drawRect( rectangle.toRect(), pen2, colour, QPainter::CompositionMode_SourceOver, false);
						}
					}
					
				}
				// ------------ selected vertices of the edited curves
				colour = QColor(100,100,255);
				painter.setBrush( colour );
				for(int k=0; k<vectorSelection.vertex.size(); k++) {
					VertexRef vertexRef = vectorSelection.vertex.at(k);
					QPointF vertexPoint = vectorImage->getVertex(vertexRef);
					QRectF rectangle0 = QRectF( (myView*transMatrix*centralView).map(vertexPoint)-QPointF(3.0,3.0), QSizeF(7,7) ); 
					painter.drawRect( rectangle0.toRect() );
					//bufferImg->drawRect( rectangle0, pen2, colour, QPainter::CompositionMode_SourceOver, false);
					
					/* --- draws the control points -- maybe editable in a future version (although not recommended)
					QPointF c1Point = vectorImage->getC1(vertexRef.nextVertex());
					QPointF c2Point = vectorImage->getC2(vertexRef);
					QRectF rectangle1 = QRectF( myTempView.map(c1Point)-QPointF(3.0,3.0), QSize(7,7) ); 
					QRectF rectangle2 = QRectF( myTempView.map(c2Point)-QPointF(3.0,3.0), QSize(7,7) ); 
					bufferImg->drawLine( myTempView.map(vertexPoint), myTempView.map(c1Point), colour, QPainter::CompositionMode_SourceOver, antialiasing);
					bufferImg->drawLine( myTempView.map(vertexPoint), myTempView.map(c2Point), colour, QPainter::CompositionMode_SourceOver, antialiasing);
					bufferImg->drawRect( rectangle0, pen2, colour, QPainter::CompositionMode_SourceOver, false);
					bufferImg->drawEllipse( rectangle1, pen2, Qt::white, QPainter::CompositionMode_SourceOver, false);
					bufferImg->drawEllipse( rectangle2, pen2, Qt::white, QPainter::CompositionMode_SourceOver, false);*/
				}
				// ----- paints the closest vertices
				colour = QColor(255,0,0);
				painter.setBrush( colour );
				if( vectorSelection.curve.size() > 0 ) {
					for(int k=0; k<closestVertices.size(); k++) {
						VertexRef vertexRef = closestVertices.at(k);
						QPointF vertexPoint = vectorImage->getVertex(vertexRef);
						//if( vectorImage->isSelected(vertexRef) ) vertexPoint = selectionTransformation.map( vertexPoint );
						QRectF rectangle = QRectF( (myView*transMatrix*centralView).map(vertexPoint)-QPointF(3.0,3.0), QSizeF(7,7) );
						painter.drawRect( rectangle.toRect() );
						//bufferImg->drawRect( rectangle.toRect(), pen2, colour, QPainter::CompositionMode_SourceOver, false);
					}
				}
				painter.restore();
			}

			if(toolMode == MOVE) {
				// ----- paints the closest curves
				bufferImg->clear();
				QPen pen2(Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin);
				QColor colour = QColor(100,100,255);
				for(int k=0; k<closestCurves.size(); k++) {
					qreal scale = myTempView.det();
					BezierCurve myCurve = vectorImage->curve[closestCurves[k]];
					if(myCurve.isPartlySelected()) myCurve.transform( selectionTransformation );
					QPainterPath path = myCurve.getStrokedPath(1.2/scale, false);
					bufferImg->drawPath( (myView*transMatrix*centralView).map(path), pen2, colour, QPainter::CompositionMode_SourceOver, antialiasing);
				}
			}

		}
		
		// paints the  buffer image
		if(editor->getCurrentLayer() != NULL) {
			painter.setOpacity(1.0);
			if(editor->getCurrentLayer()->type == Layer::BITMAP) painter.setWorldMatrixEnabled(true);
			if(editor->getCurrentLayer()->type == Layer::VECTOR) painter.setWorldMatrixEnabled(false);
			bufferImg->paintImage(painter);
		}
		
		// paints the selection outline
		if( somethingSelected && myTempTransformedSelection.isValid() ) {
			// outline of the transformed selection
			painter.setWorldMatrixEnabled(false);
			painter.setOpacity(1.0);
			QPolygon tempRect = (myView*transMatrix*centralView).mapToPolygon( myTempTransformedSelection.normalized().toRect() );
			
			Layer* layer = editor->getCurrentLayer();
			if(layer != NULL) {
				if(layer->type == Layer::BITMAP) {
					painter.setBrush(Qt::NoBrush);
					painter.setPen(Qt::DashLine);
				}
				if(layer->type == Layer::VECTOR) {
					painter.setBrush(QColor(0,0,0,20));
					painter.setPen(Qt::gray);
				}
				painter.drawPolygon( tempRect );
				
				if(layer->type != Layer::VECTOR || toolMode != SELECT) {
					painter.setPen(Qt::SolidLine);
					painter.setBrush(QBrush(Qt::gray));
					painter.drawRect( tempRect.point(0).x()-3, tempRect.point(0).y()-3, 6, 6 );
					painter.drawRect( tempRect.point(1).x()-3, tempRect.point(1).y()-3, 6, 6 );
					painter.drawRect( tempRect.point(2).x()-3, tempRect.point(2).y()-3, 6, 6 );
					painter.drawRect( tempRect.point(3).x()-3, tempRect.point(3).y()-3, 6, 6 );
				}
			}
		}
		 
	}
	// clips to the frame of the camera
	if(layer->type == Layer::CAMERA) {
		QRect rect = ((LayerCamera*)layer)->getViewRect();
		rect.translate( width()/2, height()/2 );
		painter.setWorldMatrixEnabled(false);
		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(0,0,0,160));
		painter.drawRect( QRect(0, 0, width(), (height() - rect.height())/2) );
		painter.drawRect( QRect(0, rect.bottom(), width(), (height() - rect.height())/2) );
		painter.drawRect( QRect(0, rect.top(), (width() - rect.width())/2, rect.height()-1) );
		painter.drawRect( QRect((width() + rect.width())/2, rect.top(), (width() - rect.width())/2, rect.height()-1) );
		painter.setPen(Qt::black);
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(rect);
	}
	// outlines the frame of the viewport
	painter.setWorldMatrixEnabled(false);
	painter.setPen( QPen(Qt::gray, 2) );
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(QRect(0,0, width(), height()));
	// shadow
	if(shadows && !editor->playing && (!mouseInUse || toolMode == HAND)) {
		int radius1 = 12;
		int radius2 = 8;
		QLinearGradient shadow = QLinearGradient( 0, 0, 0, radius1);
		setGaussianGradient(shadow, Qt::black, 0.15);
		painter.setPen(Qt::NoPen);
		painter.setBrush(shadow);
		painter.drawRect(QRect(0,0, width(), radius1));
		shadow.setFinalStop(radius1, 0);
		painter.setBrush(shadow);
		painter.drawRect(QRect(0,0, radius1, height()));
		shadow.setStart(0, height());
		shadow.setFinalStop(0, height()-radius2);
		painter.setBrush(shadow);
		painter.drawRect(QRect(0,height()-radius2, width(), height()));
		shadow.setStart(width(), 0);
		shadow.setFinalStop(width()-radius2, 0);
		painter.setBrush(shadow);
		painter.drawRect(QRect(width()-radius2,0, width(), height()));
	}
	event->accept();
}

void ScribbleArea::updateCanvas(int frame, QRect rect)
{	
	//qDebug() << "paint canvas!" << QDateTime::currentDateTime();
	// merge the different layers into the ScribbleArea
	QPainter painter(&canvas);
	if(myTempView.det() == 1.0) {
		painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
	} else {
		painter.setRenderHint(QPainter::SmoothPixmapTransform, antialiasing);
	}
	painter.setClipRect(rect);
	painter.setClipping(true);
	painter.fillRect(rect, Qt::red);
	setView();
	painter.setWorldMatrix(myTempView);
	painter.setWorldMatrixEnabled(true);
		
	// background
	painter.setPen(Qt::NoPen);
	painter.setBrush(backgroundBrush);
	painter.drawRect(  myTempView.inverted().mapRect( QRect(-2,-2, width()+3, height()+3) )  ); // this is necessary to have the background move with the view
	
	// grid
	bool drawGrid = false;
	if(drawGrid) {
		painter.setOpacity(1.0);
		painter.setPen(Qt::gray);
		painter.setBrush(Qt::NoBrush);
		// What kind of grid do we want?
		//painter.drawRect(QRect(0,0, mySize.width(), mySize.height()));
		//painter.drawLine( QPoint(0,mySize.height()/2), QPoint(mySize.width(), mySize.height()/2) );
		//painter.drawLine( QPoint(mySize.width()/3, 0), QPoint(mySize.width()/3, mySize.height()) );
		//painter.drawLine( QPoint(mySize.width()*2/3, 0), QPoint(mySize.width()*2/3, mySize.height()) );
	}
	
	Object* object = editor->object;
	qreal opacity;
	for(int i=0; i < object->getLayerCount(); i++) {
		opacity = 1.0;
		if(i != editor->currentLayer && (showAllLayers == 1)) { opacity = 0.4; }
		if(editor->getCurrentLayer()->type == Layer::CAMERA) opacity = 1.0;
		Layer* layer = (object->getLayer(i));
		if(layer->visible && (showAllLayers>0 || i == editor->currentLayer)) {
			// paints the bitmap images
			if(layer->type == Layer::BITMAP) {
				LayerBitmap* layerBitmap = (LayerBitmap*)layer;
				BitmapImage* bitmapImage = layerBitmap->getLastBitmapImageAtFrame(frame, 0);
				if(bitmapImage != NULL) {
					painter.setWorldMatrixEnabled(true);
					
					// previous frame (onion skin)
					BitmapImage* previousImage = layerBitmap->getLastBitmapImageAtFrame(frame, -1);
					if(previousImage != NULL && onionPrev) { painter.setOpacity(opacity*0.5); previousImage->paintImage(painter); }
					//painter.drawImage(previousImage->topLeft(), *(previousImage->image) );
					
					// next frame (onion skin)
					BitmapImage* nextImage = layerBitmap->getLastBitmapImageAtFrame(frame, 1);
					if(nextImage != NULL && onionNext) { painter.setOpacity(opacity*0.5); nextImage->paintImage(painter); }
					
					// current frame
					painter.setOpacity(opacity);
					if(i==editor->currentLayer && somethingSelected && (myTempTransformedSelection != mySelection) ) {
						// hole in the original selection -- might support arbitrary shapes in the future
						painter.setClipping(true);
						QRegion clip = QRegion(mySelection.toRect());
						QRegion totalImage = QRegion( myTempView.inverted().mapRect( QRect(-2,-2, width()+3, height()+3) ) );
						QRegion ImageWithHole = totalImage-=clip;
						painter.setClipRegion(ImageWithHole, Qt::ReplaceClip);
						//painter.drawImage(bitmapImage->topLeft(), *(bitmapImage->image) );
						bitmapImage->paintImage(painter);
						painter.setClipping(false);
						// transforms the bitmap selection
						bool smoothTransform = false;
						if(myTempTransformedSelection.width() != mySelection.width() || myTempTransformedSelection.height() != mySelection.height() ) smoothTransform = true;
						BitmapImage selectionClip = bitmapImage->copy(mySelection.toRect());
						selectionClip.transform(myTempTransformedSelection, smoothTransform);
						selectionClip.paintImage(painter);
						//painter.drawImage(selectionClip.topLeft(), *(selectionClip.image));
					} else {
						//painter.drawImage(bitmapImage->topLeft(), *(bitmapImage->image) );
						bitmapImage->paintImage(painter);
					}
					//painter.setPen(Qt::red);
					//painter.setBrush(Qt::NoBrush);
					//painter.drawRect(bitmapImage->boundaries);
				}
			}
			// paints the vector images
			if(layer->type == Layer::VECTOR) {
				LayerVector* layerVector = (LayerVector*)layer;
				VectorImage* vectorImage = layerVector->getLastVectorImageAtFrame(frame, 0);
				if( somethingSelected ) {
					// transforms the vector selection
					//calculateSelectionTransformation();
					vectorImage->setSelectionTransformation(selectionTransformation);
					//vectorImage->setTransformedSelection(myTempTransformedSelection);
				} 
				QImage* image = layerVector->getLastImageAtFrame(frame, 0, size(), simplified, showThinLines, curveOpacity, antialiasing, gradients);
				if(image != NULL) {
					painter.setWorldMatrixEnabled(false);
					
					// previous frame (onion skin)
					QImage* previousImage = layerVector->getLastImageAtFrame(frame, -1, size(), simplified, showThinLines, curveOpacity, antialiasing, gradients);
					if(previousImage != NULL && onionPrev) { painter.setOpacity(opacity*0.5); painter.drawImage(QPoint(0, 0), *previousImage ); }
					
					// next frame (onion skin)
					QImage* nextImage = layerVector->getLastImageAtFrame(frame, 1, size(), simplified, showThinLines, curveOpacity, antialiasing, gradients);
					if(nextImage != NULL && onionNext) { painter.setOpacity(opacity*0.5); painter.drawImage(QPoint(0, 0), *nextImage ); }
					
					// current frame
					painter.setOpacity(opacity);
					painter.drawImage(QPoint(0, 0), *image);
				}
			}
		}
	}
	painter.end();
}

void ScribbleArea::setGaussianGradient(QGradient &gradient, QColor colour, qreal opacity) {
	int r = colour.red();
	int g = colour.green();
	int b = colour.blue();
	qreal a = colour.alphaF();
			gradient.setColorAt(0.0, QColor(r, g, b, qRound(a*255*opacity)) );
			gradient.setColorAt(0.1, QColor(r, g, b, qRound(a*245*opacity)) );
			gradient.setColorAt(0.2, QColor(r, g, b, qRound(a*217*opacity)) );
			gradient.setColorAt(0.3, QColor(r, g, b, qRound(a*178*opacity)) );
			gradient.setColorAt(0.4, QColor(r, g, b, qRound(a*134*opacity)) );
			gradient.setColorAt(0.5, QColor(r, g, b, qRound(a*94*opacity)) );
			gradient.setColorAt(0.6, QColor(r, g, b, qRound(a*60*opacity)) );
			gradient.setColorAt(0.7, QColor(r, g, b, qRound(a*36*opacity)) );
			gradient.setColorAt(0.8, QColor(r, g, b, qRound(a*20*opacity)) );
			gradient.setColorAt(0.9, QColor(r, g, b, qRound(a*10*opacity)) );
			gradient.setColorAt(1.0, QColor(r, g, b, 0) );
}

void ScribbleArea::drawBrush(QPointF thePoint, qreal brushWidth, QColor fillColour, qreal opacity) {
	QRadialGradient radialGrad(thePoint, 0.5*brushWidth);
	setGaussianGradient(radialGrad, fillColour, opacity);
	
	//radialGrad.setCenter( thePoint );
	//radialGrad.setFocalPoint( thePoint );
				
	QRectF rectangle(thePoint.x()-0.5*brushWidth, thePoint.y()-0.5*brushWidth, brushWidth, brushWidth);
	BitmapImage* tempBitmapImage = new BitmapImage(NULL);
	tempBitmapImage->drawRect( rectangle, Qt::NoPen, radialGrad, QPainter::CompositionMode_Source, antialiasing);
	bufferImg->paste(tempBitmapImage);
	delete tempBitmapImage;
}

void ScribbleArea::drawLineTo(const QPointF &endPixel, const QPointF &endPoint)
{	
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	
	if(layer->type == Layer::BITMAP) {
		//int index = ((LayerImage*)layer)->getLastIndexAtFrame(editor->currentFrame);
		//if(index == -1) return;
		//BitmapImage* bitmapImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
		//if(bitmapImage == NULL) { qDebug() << "NULL image pointer!" << editor->currentLayer << editor->currentFrame;  return; }
		
		if (toolMode == ScribbleArea::ERASER) {
			QPen pen2 = QPen ( QBrush(QColor(255,255,255,255)), currentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
			bufferImg->drawLine(lastPoint, endPoint, pen2, QPainter::CompositionMode_SourceOver, antialiasing);
			int rad = qRound(currentWidth / 2) + 2;
			update(myTempView.mapRect(QRect(lastPoint.toPoint(), endPoint.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad)));
		}
		if(toolMode == ScribbleArea::PENCIL) {
			QPen pen2 = QPen ( QBrush(currentColour), currentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
			bufferImg->drawLine(lastPoint, endPoint, pen2, QPainter::CompositionMode_SourceOver, antialiasing);
			int rad = qRound(currentWidth / 2) + 3;
			update(myTempView.mapRect(QRect(lastPoint.toPoint(), endPoint.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad)));
		}
		if(toolMode == ScribbleArea::PEN) {
			QPen pen2 = QPen ( QBrush(pen.colour), currentWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
			bufferImg->drawLine(lastPoint, endPoint, pen2, QPainter::CompositionMode_SourceOver, antialiasing);
			int rad = qRound(currentWidth / 2) + 3;
			update(myTempView.mapRect(QRect(lastPoint.toPoint(), endPoint.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad)));
		}
		if(toolMode == ScribbleArea::COLOURING) {
			qreal opacity = 1.0;
			qreal brushWidth = brush.width;
			if(tabletInUse) opacity = tabletPressure;
			if(usePressure) brushWidth = brush.width*tabletPressure;
			
			qreal distance = 4*QLineF(endPoint, lastBrushPoint).length();
			int steps = qRound( distance )/brushWidth;
			
			for(int i=0; i<steps; i++) {
				QPointF thePoint = lastBrushPoint + (i+1)*(brushWidth)*(endPoint -lastBrushPoint)/distance;
				drawBrush( thePoint, brushWidth, brush.colour, opacity);
				if(i==steps-1) lastBrushPoint = thePoint;
			}
			
			int rad = qRound(brushWidth / 2) + 3;
			update(myTempView.mapRect(QRect(lastPoint.toPoint(), endPoint.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad)));
		}
	}
	if(layer->type == Layer::VECTOR) {
		if (toolMode == ScribbleArea::ERASER) {
			bufferImg->drawLine(lastPixel, currentPixel, QPen(Qt::white, currentWidth, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin), QPainter::CompositionMode_SourceOver, antialiasing);
			int rad = qRound(  (currentWidth/2 + 2)*qAbs( myTempView.m11() )  );
			update(QRect(lastPixel.toPoint(), endPixel.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad));
		}
		if(toolMode == ScribbleArea::PENCIL) {
			bufferImg->drawLine(lastPixel, currentPixel, QPen(currentColour, 1, Qt::DotLine, Qt::RoundCap,Qt::RoundJoin), QPainter::CompositionMode_SourceOver, antialiasing);
			int rad = qRound(  ( currentWidth/2 + 2)*qAbs( myTempView.m11() )  );
			update(QRect(lastPixel.toPoint(), endPixel.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad));
		}
		if(toolMode == ScribbleArea::PEN) {
			bufferImg->drawLine(lastPixel, currentPixel, QPen(pen.colour, currentWidth*myTempView.m11(), Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin), QPainter::CompositionMode_SourceOver, antialiasing);
			int rad = qRound(  (currentWidth/2 + 2)* (qAbs(myTempView.m11())+qAbs(myTempView.m22())) );
			update(QRect(lastPixel.toPoint(), endPixel.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad));
		}
		if(toolMode == ScribbleArea::COLOURING) {
			bufferImg->drawLine(lastPixel, currentPixel, QPen(Qt::gray, 1, Qt::DashLine, Qt::RoundCap,Qt::RoundJoin), QPainter::CompositionMode_SourceOver, antialiasing);
			int rad = qRound(   (currentWidth/2 + 2)*qAbs( myTempView.m11() )   );
			update(QRect(lastPixel.toPoint(), endPixel.toPoint()).normalized().adjusted(-rad, -rad, +rad, +rad));
		}
	}
	
	//emit modification();
	
	lastPixel = endPixel;
	lastPoint = endPoint;
}

void ScribbleArea::drawEyedropperPreview(const QColor colour) {
	QPainter painter( eyedropperCursor );
	painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
	painter.setBrush(colour);
	painter.drawRect( 10, 10, 20, 20);
	painter.end();
	setCursor( QCursor(*eyedropperCursor, 5, 5) );
}

void ScribbleArea::drawPolyline() {
	if(toolMode == POLYLINE) {
		if(mousePoints.size()>0) {
			QPen pen2(pen.colour, pen.width, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin);
			QPainterPath tempPath = BezierCurve(mousePoints).getSimplePath();
			tempPath.lineTo(currentPoint);
			QRect updateRect = myTempView.mapRect(tempPath.boundingRect().toRect()).adjusted(-10,-10,10,10);
			if(editor->getCurrentLayer()->type == Layer::VECTOR) {
				tempPath = myTempView.map( tempPath );
				if(makeInvisible) { pen2.setWidth(0); pen2.setStyle(Qt::DotLine);} else pen2.setWidth(pen.width*myTempView.m11());
			}
			bufferImg->clear();
			bufferImg->drawPath( tempPath, pen2, Qt::NoBrush, QPainter::CompositionMode_SourceOver, antialiasing);
			update(  updateRect  );
			//update( QRect(lastPixel.toPoint(), currentPixel.toPoint()).normalized() );
			//bufferImg->drawRect(tempPath.boundingRect().toRect());
			//update( QRect(lastPixel.toPoint()-QPoint(10,10), lastPixel.toPoint()+QPoint(10,10)) );
			//update();
		}
	}
}

void ScribbleArea::endPolyline()
{
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) {
		BezierCurve curve = BezierCurve(mousePoints);
		if(makeInvisible) curve.setWidth(0); else curve.setWidth(pen.width);
		curve.setColourNumber( pen.colourNumber );
		curve.setVariableWidth(false);
		curve.setInvisibility(makeInvisible);
		//curve.setSelected(true);
		((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->addCurve(curve, qAbs(myTempView.m11()) );
	}
	if(layer->type == Layer::BITMAP) {
		drawPolyline();
		BitmapImage* bitmapImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
		bitmapImage->paste(bufferImg);
	}
	bufferImg->clear();
	while(!mousePoints.isEmpty()) mousePoints.removeAt(0); // empty the mousePoints
	setModified(editor->currentLayer, editor->currentFrame);
}


void ScribbleArea::resizeEvent(QResizeEvent *event)
{
	//resize( size() );
	QWidget::resizeEvent(event);
	canvas = QPixmap(size());
	recentre();
	updateAllFrames();
}

void ScribbleArea::recentre() {
	centralView = QMatrix(1,0,0,1, 0.5*width(), 0.5*height());
	setView();
	QPixmapCache::clear();
	update();
}

void ScribbleArea::setView() {
	setView(getView());
}

void ScribbleArea::setView(QMatrix view) {
	for(int i=0; i < editor->object->getLayerCount() ; i++) {
		Layer* layer = editor->object->getLayer(i);
		if(layer->type == Layer::VECTOR) {
			((LayerVector*)layer)->setView(view * centralView);
		}
	}
	myTempView = view * centralView;
}

void ScribbleArea::resetView() {
	editor->resetMirror();
	myView.reset();
	myTempView = myView * centralView;
	recentre();
}

QMatrix ScribbleArea::getView() {
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return QMatrix(); // error
	if(layer->type == Layer::CAMERA) {
		return ((LayerCamera*)layer)->getViewAtFrame(editor->currentFrame);
		qDebug() << "viewCamera" << ((LayerCamera*)layer)->getViewAtFrame(editor->currentFrame);
	} else {
		return myView;
	}
}

QRectF ScribbleArea::getViewRect() {
	QRectF rect =  QRectF( -width()/2, -height()/2, width(), height() );
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return rect;
	if(layer->type == Layer::CAMERA) {
		return ((LayerCamera*)layer)->getViewRect();
	} else {
		return rect;
	}
}

QPointF ScribbleArea::getCentralPoint() {
	return myTempView.inverted().map( QPoint(width()/2, height()/2) );
}

void ScribbleArea::calculateSelectionRect() {
	selectionTransformation.reset();
	Layer* layer = editor->getCurrentLayer();
	if(layer==NULL) return;
	if(layer->type == Layer::VECTOR) {
		VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
		vectorImage->calculateSelectionRect();
		setSelection( vectorImage->getSelectionRect(), true );
	}
}

void ScribbleArea::calculateSelectionTransformation() {
	qreal c1x, c1y , c2x, c2y, scaleX, scaleY;
	c1x = 0.5*(myTempTransformedSelection.left()+myTempTransformedSelection.right());
	c1y = 0.5*(myTempTransformedSelection.top()+myTempTransformedSelection.bottom());
	c2x = 0.5*(mySelection.left()+mySelection.right());
	c2y = 0.5*(mySelection.top()+mySelection.bottom());
	if(mySelection.width() == 0) { scaleX = 1.0; } else { scaleX = myTempTransformedSelection.width()/mySelection.width(); }
	if(mySelection.height() == 0) { scaleY = 1.0; } else { scaleY = myTempTransformedSelection.height()/mySelection.height(); }
	selectionTransformation.reset();
	selectionTransformation.translate(c1x,c1y);
	selectionTransformation.scale(scaleX, scaleY);
	selectionTransformation.translate(-c2x,-c2y);
	//modification();
}

void ScribbleArea::paintTransformedSelection() {
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	
	if(somethingSelected) {  // there is something selected
		if(layer->type == Layer::BITMAP && (myTransformedSelection != mySelection) ) {
			//backup();
			BitmapImage* bitmapImage = ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0);
			if(bitmapImage == NULL) { qDebug() << "NULL image pointer!" << editor->currentLayer << editor->currentFrame;  return; }
			
			bool smoothTransform = false;
			if(myTransformedSelection.width() != mySelection.width() || myTransformedSelection.height() != mySelection.height() ) smoothTransform = true;
			BitmapImage selectionClip = bitmapImage->copy(mySelection.toRect());
			selectionClip.transform( myTransformedSelection, smoothTransform );
			bitmapImage->clear(mySelection.toRect());
			bitmapImage->paste(&selectionClip);
		}
		if(layer->type == Layer::VECTOR) {
			// vector transformation
			LayerVector* layerVector = (LayerVector*)layer;
			VectorImage* vectorImage = layerVector->getLastVectorImageAtFrame(editor->currentFrame, 0);
			vectorImage->applySelectionTransformation();
			selectionTransformation.reset();
		}
		//deselectAll();
		//emit modification();
		setModified(editor->currentLayer, editor->currentFrame);
	}
}

void ScribbleArea::setSelection(QRectF rect, bool trueOrFalse) {
	mySelection = rect;
	myTransformedSelection = rect;
	myTempTransformedSelection = rect;
	somethingSelected = trueOrFalse;
	displaySelectionProperties();
}

void ScribbleArea::displaySelectionProperties() {
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) {
		LayerVector* layerVector = (LayerVector*)layer;
		VectorImage* vectorImage = layerVector->getLastVectorImageAtFrame(editor->currentFrame, 0);
		//vectorImage->applySelectionTransformation();
		if(toolMode == MOVE) {
			//if(closestCurves.size()>0) {
			int selectedCurve = vectorImage->getFirstSelectedCurve();
			if(selectedCurve != -1) {
				editor->setWidth(vectorImage->curve[selectedCurve].getWidth());
				editor->setFeather(vectorImage->curve[selectedCurve].getFeather());
				editor->setOpacity(0);
				editor->setInvisibility(vectorImage->curve[selectedCurve].isInvisible());
				editor->setPressure(vectorImage->curve[selectedCurve].getVariableWidth());
				editor->selectColour(vectorImage->curve[selectedCurve].getColourNumber());
			}
			
			int selectedArea = vectorImage->getFirstSelectedArea();
			if(selectedArea != -1) {
				editor->selectColour(vectorImage->area[selectedArea].colourNumber);
				//editor->setFeather(vectorImage->area[selectedArea].getFeather());
			}
		}
	}
}

void ScribbleArea::selectAll() {
	offset.setX(0); offset.setY(0);
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::BITMAP) {
		setSelection( myTempView.inverted().mapRect( QRect(-2,-2, width()+3, height()+3) ), true ); // TO BE IMPROVED
	}
	if(layer->type == Layer::VECTOR) {
		VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0);
		vectorImage->selectAll();
		setSelection( vectorImage->getSelectionRect(), true );
	}
	updateFrame();
}

void ScribbleArea::deselectAll() {
	offset.setX(0); offset.setY(0);
	selectionTransformation.reset();
	mySelection.setRect(10,10,20,20);
	myTransformedSelection.setRect(10,10,20,20);
	myTempTransformedSelection.setRect(10,10,20,20);
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) {
		((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->deselectAll(); 
		if(toolMode == MOVE) {
			editor->setWidth(-1);
			editor->setOpacity(-1);
			editor->setInvisibility(-1);
			editor->setPressure(-1);
		}
	}
	somethingSelected = false;
	bufferImg->clear();
	vectorSelection.clear();
	while(!mousePoints.isEmpty()) mousePoints.removeAt(0); // empty the mousePoints
	updateFrame();
}

void ScribbleArea::onionNextSlot() {
	onionNext = !onionNext;
	updateAllFrames();
}

void ScribbleArea::onionPrevSlot() {
	onionPrev = !onionPrev;
	updateAllFrames();
}




void ScribbleArea::floodFill(VectorImage* vectorImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance) {
	bool invertible;
	QPointF initialPoint = myTempView.inverted(&invertible).map(QPointF(point));
	// Step 1: peforms a standard (pixel-based) flood fill, and finds the vertices on the contour of the filled area
	qreal tol = 8.0/qAbs(myTempView.m11()); // tolerance for finding vertices along the contour of the flood-filled area
	qreal tol2 = 1.5/qAbs(myTempView.m11()); // tolerance for connecting contour vertices from different curves // should be small as close points of different curves are supposed to coincide
	QList<QPoint> queue; // queue all the pixels of the filled area (as they are found)
	QList<QPoint> contourPixels; // list of the pixels near the contour of the filled area
	int j, k; bool condition;
	//vectorImage->update(true, showThinLines); // update the vector image with simplified curves (all width=1)
	QImage* targetImage = new QImage(size(), QImage::Format_ARGB32_Premultiplied);
	vectorImage->outputImage(targetImage, size(), myTempView, true, showThinLines, 1.0, true, false); // the target image is the vector image with simplified curves (all width=1)
	//QImage* replaceImage = &bufferImg;
	QImage* replaceImage = new QImage( size(), QImage::Format_ARGB32_Premultiplied);
	QList<VertexRef> points = vectorImage->getAllVertices(); // refs of all the points
	QList<VertexRef> boxPoints; // refs of points inside the bounding box
	QList<VertexRef> contourPoints; // refs of points near the contour pixels
	QList<VertexRef> vertices; 
	if(  BitmapImage::rgbDistance(targetImage->pixel(point.x(), point.y()), targetColour) > tolerance ) return;
	queue.append( point );
	int boxLeft = point.x();
	int boxRight = point.x();
	int boxTop = point.y();
	int boxBottom = point.y();
	// ----- flood fill and remember the contour pixels -> contourPixels
	// ----- from the standard flood fill algorithm
	// ----- http://en.wikipedia.org/wiki/Flood_fill
	j = -1; k = 1;
	for(int i=0; i< queue.size(); i++ ) {
		point = queue.at(i);
		if(  replaceImage->pixel(point.x(), point.y()) != replacementColour  && BitmapImage::rgbDistance(targetImage->pixel(point.x(), point.y()), targetColour) < tolerance ) {
			//image.setPixel( point.x(), point.y(), replacementColour);
			j = -1; condition =  (point.x() + j > 0);
			while( replaceImage->pixel(point.x()+j, point.y()) != replacementColour  && BitmapImage::rgbDistance(targetImage->pixel( point.x()+j, point.y() ), targetColour) < tolerance && condition) { 
				j = j - 1;
				condition =  (point.x() + j > 0);
			}
			if(!condition) { floodFillError(1); return; }
			if( BitmapImage::rgbDistance(targetImage->pixel( point.x()+j, point.y() ), targetColour) >= tolerance ) {  // bumps into the contour
				contourPixels.append( point+QPoint(j,0) );
			}
			
			k = 1; condition = ( point.x() + k < targetImage->width()-1);
			while( replaceImage->pixel(point.x()+k, point.y()) != replacementColour  && BitmapImage::rgbDistance(targetImage->pixel( point.x()+k, point.y() ), targetColour) < tolerance && condition) {
				k = k + 1;
				condition = ( point.x() + k < targetImage->width()-1);
			}
			if(!condition) { floodFillError(1); return; }
			if( BitmapImage::rgbDistance(targetImage->pixel( point.x()+k, point.y() ), targetColour) >= tolerance ) {  // bumps into the contour
				contourPixels.append( point + QPoint(k,0) );
			}
			if(point.x()+k > boxRight) boxRight = point.x()+k;
			if(point.x()+j < boxLeft) boxLeft = point.x()+j;
			for(int x = j+1; x < k; x++) {
				replaceImage->setPixel( point.x()+x, point.y(), replacementColour);
				if(point.y() - 1 > 0 && queue.size() < targetImage->height() * targetImage->width() ) {
					if( replaceImage->pixel(point.x()+x, point.y()-1) != replacementColour) {
						if(BitmapImage::rgbDistance(targetImage->pixel( point.x()+x, point.y() - 1), targetColour) < tolerance) {
							queue.append( point + QPoint(x,-1) );
							if(point.y()-1< boxBottom) boxBottom = point.y()-1;
						}
						else { // bumps into the contour
							contourPixels.append(point+QPoint(x,-1));
						}
					}
				} else { floodFillError(1); return; }
				if(point.y() + 1 < targetImage->height() && queue.size() < targetImage->height() * targetImage->width() ) {
					if( replaceImage->pixel(point.x()+x, point.y()+1) != replacementColour) {
						if(BitmapImage::rgbDistance(targetImage->pixel( point.x()+x, point.y() + 1), targetColour) < tolerance) {
							queue.append( point + QPoint(x, 1) );
							if(point.y()+1> boxTop) boxTop = point.y()+1;
						} 
						else { // bumps into the contour
							contourPixels.append(point+QPoint(x,1));
						}
					}
				} else { floodFillError(1); return; }
			}
		}
	}
	// --- finds the bounding box of the filled area, and all the points contained inside (+ 1*tol)  -> boxPoints
	QPointF mBoxTopRight = myTempView.inverted(&invertible).map(    QPointF(qMax(boxLeft,boxRight)+1*tol, qMax(boxTop,boxBottom)+1*tol)  );
	QPointF mBoxBottomLeft = myTempView.inverted(&invertible).map( QPointF(qMin(boxLeft,boxRight)-1*tol, qMin(boxTop,boxBottom)-1*tol) );
	QRectF boundingBox = QRectF( mBoxBottomLeft.x()-1, mBoxBottomLeft.y()-1, qAbs(mBoxBottomLeft.x()-mBoxTopRight.x())+2, qAbs(mBoxBottomLeft.y()-mBoxTopRight.y())+2 );
	debugRect = QRectF(0,0,0,0);
	debugRect = boundingBox;
	for(int l=0; l<points.size(); l++) {
		QPointF mPoint = vectorImage->getVertex(points.at(l));
		if(   boundingBox.contains( mPoint )   ) {
			// -----
			//vectorImage->setSelected(points.at(l), true);
			boxPoints.append( points.at(l) );
		}
	}
	// ---- finds the points near the contourPixels -> contourPoints
	for(int i=0; i< contourPixels.size(); i++ ) {
		QPointF mPoint = myTempView.inverted(&invertible).map(  QPointF( contourPixels.at(i) )   );
		vertices = vectorImage->getAndRemoveVerticesCloseTo(mPoint, tol, &boxPoints);
		//contourPoints << vertices;
		for(int m=0; m<vertices.size(); m++) {  // for each ?
			contourPoints.append( vertices.at(m) );
		}
	}
	// ---- points of sharp peaks may be missing in contourPoints ---> we correct for that
	for(int i=0; i< contourPoints.size(); i++) {
		VertexRef theNextVertex = contourPoints[i].nextVertex();
		if( ! contourPoints.contains(theNextVertex) ) { // if the next vertex is not in the list of contour points
			if( contourPoints.contains( theNextVertex.nextVertex() ) ) { // but the next-next vertex is...
				contourPoints.append( theNextVertex );
				//qDebug() << "----- found SHARP point (type 1a) ------";
			}
			QList<VertexRef> closePoints = vectorImage->getVerticesCloseTo( theNextVertex, tol2 );
			for( int j=0; j<closePoints.size(); j++ ) {
				if( closePoints[j] != theNextVertex ) { // ...or a point connected to the next vertex is
					if( contourPoints.contains(closePoints[j].nextVertex()) || contourPoints.contains(closePoints[j].prevVertex()) ) {
						contourPoints.append( theNextVertex );
						contourPoints.append( closePoints[j] );
						//qDebug() << "----- found SHARP point (type 2a) ------";
					}
				}
			}
		}
		VertexRef thePreviousVertex = contourPoints[i].prevVertex();
		if( ! contourPoints.contains(thePreviousVertex) ) { // if the previous vertex is not in the list of contour points
			if( contourPoints.contains( thePreviousVertex.prevVertex() ) ) { // but the prev-prev vertex is...
				contourPoints.append( thePreviousVertex );
				//qDebug() << "----- found SHARP point (type 1b) ------";
			}
			QList<VertexRef> closePoints = vectorImage->getVerticesCloseTo( thePreviousVertex, tol2 );
			for( int j=0; j<closePoints.size(); j++ ) {
				if( closePoints[j] != thePreviousVertex ) { // ...or a point connected to the previous vertex is
					if( contourPoints.contains(closePoints[j].nextVertex()) || contourPoints.contains(closePoints[j].prevVertex()) ) {
						contourPoints.append( thePreviousVertex );
						contourPoints.append( closePoints[j] );
						//qDebug() << "----- found SHARP point (type 2b) ------";
					}
				}
			}
		}
	}
	
	// 1 --- stop here (for debugging purpose)
	/*qDebug() << "CONTOUR POINTS:";
	for(int i=0; i < contourPoints.size(); i++) {
		qDebug() << "(" << contourPoints.at(i).curveNumber << "," << contourPoints.at(i).vertexNumber << ")";
	}*/
	// -----
	vectorImage->setSelected( contourPoints, true);
	update();
	//replaceImage->fill(qRgba(0,0,0,0));
	//QMessageBox::warning(this, tr("My Application"), tr("all the contour points"), QMessageBox::Ok, QMessageBox::Ok);
	//vectorImage->deselectAll();
	//return;
	// 2 --- or continue
	
	// Step 2: finds closed paths among the selected vertices: we start from a vertex and build a tree of connected vertices
	//while(contourPoints.size() > 0) {
	QList<VertexRef> tree;
	QList<int> fatherNode; // given the index in tree (of a vertex), return the index (in tree) of its father vertex; this will define the tree structure
	QList<int> leaves; // list of indices in tree which correspond to end of branches (leaves)
	
	// Step 2.1: build tree
	int rootIndex = -1; bool rootIndexFound = false;
	while(!rootIndexFound && rootIndex < contourPoints.size()-1) {
		rootIndex++;
		if( vectorImage->getVerticesCloseTo( vectorImage->getVertex(contourPoints.at(rootIndex)), tol2, &contourPoints).size() > 1) {
			// this point is connected!
			rootIndexFound = true;
		}
	}
	if(!rootIndexFound) { floodFillError(3); return; }
	tree << contourPoints.at(rootIndex);
	fatherNode.append(-1);
	//leaves << 0;
	contourPoints.removeAt(rootIndex);
	VertexRef vertex0 = tree.at(0);
	//qDebug() << "ROOT = " << 	vertex0.curveNumber << "," << vertex0.vertexNumber;
	j=0;
	bool success = false;
	int counter = 0;
	while(!success && j>-1 && counter<1000) {
		counter++;
		//qDebug() << "------";
		VertexRef vertex  = tree.at(j);
		//qDebug() << j << "/" << tree.size() << "   " << vertex.curveNumber << "," << vertex.vertexNumber << "->" << fatherNode.at(j);
		int index1 = contourPoints.indexOf(vertex.nextVertex());
		if( index1 != -1) {
			//qDebug() << "next vertex";
			tree.append(vertex.nextVertex());
			fatherNode.append(j);
			contourPoints.removeAt(index1);
			j = tree.size()-1;
		} else {
			int index2 = contourPoints.indexOf(vertex.prevVertex());
			if( index2 != -1 ) {
				// qDebug() << "previous vertex";
				tree.append(vertex.prevVertex());
				fatherNode.append(j);
				contourPoints.removeAt(index2);
				j = tree.size()-1;
			} else {
				QList<VertexRef> pointsNearby = vectorImage->getVerticesCloseTo( vectorImage->getVertex(vertex), tol2, &contourPoints);
				if(pointsNearby.size() > 0) {
					//qDebug() << "close vertex";
					tree << pointsNearby.at(0);
					fatherNode.append(j);
					contourPoints.removeAt( contourPoints.indexOf(pointsNearby.at(0)) );
					j = tree.size()-1;
				} else {
					qreal dist = vectorImage->getDistance(vertex, vertex0);
					//qDebug() << "is it a leave ? " << j << "dist = " << dist << "-" << tol2;
					if(  ((vertex.curveNumber == vertex0.curveNumber) && (qAbs(vertex.vertexNumber-vertex0.vertexNumber)==1))  ||  (dist < tol2) ) {
						// we found a leaf close to the root of the tree - does the closed path contain the initial point?
						QList<VertexRef> closedPath;
						int pathIndex = j;
						if(dist > 0) closedPath.prepend(vertex0);
						closedPath.prepend(tree.at(pathIndex));
						while( (pathIndex = fatherNode.at(pathIndex)) != -1) {
							closedPath.prepend(tree.at(pathIndex));
						}
						BezierArea newArea = BezierArea( closedPath, brush.colourNumber );
						vectorImage->updateArea(newArea);
						if( newArea.path.contains(initialPoint) ) {
							vectorImage->addArea( newArea );
							//qDebug() << "Yes!";
							success = true;
						} else {
							//qDebug() << "No! almost";
							j = fatherNode.at(j);
						}
					} else {
						//qDebug() << "No!";
						leaves << j;
						j = fatherNode.at(j);
					}
				}
			}
		}
		
		//
	}
	
	if(!success) { floodFillError(2); return; }
	//qDebug() << "failure!" << contourPoints.size();
	replaceImage->fill(qRgba(0,0,0,0));
	deselectAll();
	
	// -- debug --- (display tree)
	/*for(int indexm=0; indexm < tree.size(); indexm++) {
		qDebug() << indexm  << ") " << tree.at(indexm).curveNumber << "," << tree.at(indexm).vertexNumber << " -> " << fatherNode.at(indexm);
		//if(leaves.contains(indexm)) vectorImage->setSelected( tree.at(indexm), true);
		vectorImage->setSelected( tree.at(indexm), true);
		update();
		//sleep( 1 );
		QMessageBox::warning(this, tr("My Application"), tr("all the tree points"), QMessageBox::Ok, QMessageBox::Ok);
	}*/
	delete targetImage;
	update();
}

void ScribbleArea::floodFillError(int errorType) {
	QString message, error;
	if(errorType == 1) message = "There is a gap in your drawing (or maybe you have zoomed too much).";
	if(errorType == 2 || errorType == 3) message = "Sorry! This doesn't always work."
			"Please try again (zoom a bit, click at another location... )<br>"
			"if it doesn't work, zoom a bit and check that your paths are connected by pressing F1.).";
	
	if(errorType == 1) error = "Out of bound.";
	if(errorType == 2) error = "Could not find a closed path.";
	if(errorType == 3) error = "Could not find the root index.";
	QMessageBox::warning(this, tr("Flood fill error"), message+"<br><br>Error: "+error, QMessageBox::Ok, QMessageBox::Ok);
	bufferImg->clear();
	deselectAll();
}


void ScribbleArea::switchTool() {
	if(toolMode == ScribbleArea::MOVE) { paintTransformedSelection(); deselectAll(); }
	if(toolMode == ScribbleArea::POLYLINE) escape();
}

void ScribbleArea::updateCursor() {
	if(toolMode == PENCIL) {
		if(toolCursors) {
			QCursor cursor(QPixmap(":icons/pencil2.png"),0,16);
			setCursor(cursor);
		} else {
			setCursor(Qt::CrossCursor);
		}
	}
	if(toolMode == PEN) {
		if(toolCursors) {
			QCursor cursor(QPixmap(":icons/pen.png"),7,0);
			setCursor(cursor);
		} else {
			setCursor(Qt::CrossCursor);
		}
	}
	if(toolMode == ERASER) {
		QPixmap pixmap(eraser.width,eraser.width);
		pixmap.fill( QColor(255,255,255,0) );
		QPainter painter(&pixmap);
		painter.setPen( QColor(0,0,0,190) );
		painter.setBrush( QColor(255,255,255,100) );
		painter.drawLine( QPointF(eraser.width/2-2,eraser.width/2), QPointF(eraser.width/2+2,eraser.width/2) );
		painter.drawLine( QPointF(eraser.width/2,eraser.width/2-2), QPointF(eraser.width/2,eraser.width/2+2) );
		painter.setRenderHints(QPainter::Antialiasing, true);
		painter.setPen( QColor(0,0,0,100) );
		painter.drawEllipse( QRectF(1,1,eraser.width-2,eraser.width-2) );
		painter.end();
		setCursor(pixmap); //setCursor(Qt::CrossCursor);
	}
	if(toolMode == BUCKET) {
		if(toolCursors) {
			QPixmap pixmap(":icons/bucketTool.png");
			QPainter painter(&pixmap);
			painter.setPen( brush.colour );
			painter.drawLine( QPoint(5,16), QPoint(5,18) );
			painter.end();
			QCursor cursor(pixmap,4,20);
			setCursor(cursor);
		} else {
			setCursor(Qt::CrossCursor);
		}
	}
	if(toolMode == EYEDROPPER) {
		if(eyedropperCursor == NULL) {
			eyedropperCursor = new QPixmap(32,32);
			eyedropperCursor->fill(Qt::white);
			QPixmap* mask = new QPixmap(32,32);
			mask->fill(Qt::color0);
			QPainter painter(eyedropperCursor);
			painter.drawLine(5,0,5,10);
			painter.drawLine(0,5,10,5);
			painter.end();
			painter.begin(mask);
			painter.setBrush(Qt::color1);
			painter.setPen(Qt::color1);
			painter.drawLine(5,0,5,10);
			painter.drawLine(0,5,10,5);
			painter.drawRect(10,10,20,20);
			painter.end();
			eyedropperCursor->setMask(*mask);
		}
		setCursor( QCursor(*eyedropperCursor, 5, 5) );
	}
	if(toolMode == SELECT) {
		Layer* layer = editor->getCurrentLayer();
		if(layer->type == Layer::VECTOR) setCursor(Qt::ArrowCursor);
		if(layer->type == Layer::BITMAP) setCursor(Qt::CrossCursor);
	}
	if(toolMode == MOVE) {
		setCursor(Qt::ArrowCursor);
	}
	if(toolMode == HAND) {
		QPixmap pixmap(":icons/hand.png");
		setCursor(pixmap); //setCursor(Qt::PointingHandCursor);
	}
	if(toolMode == POLYLINE) {
		setCursor(Qt::CrossCursor);
	}
	if(toolMode == COLOURING) {
		Layer* layer = editor->getCurrentLayer();
		if(layer->type == Layer::VECTOR) {
			setCursor(Qt::CrossCursor);
		}
		if(layer->type == Layer::BITMAP) {
			qreal width = brush.width*0.66;
			QPixmap pixmap(width,width);
			pixmap.fill( QColor(255,255,255,0) );
			QPainter painter(&pixmap);
			painter.setPen( QColor(0,0,0,190) );
			painter.setBrush( Qt::NoBrush );
			painter.drawLine( QPointF(width/2-2,width/2), QPointF(width/2+2,width/2) );
			painter.drawLine( QPointF(width/2,width/2-2), QPointF(width/2,width/2+2) );
			painter.setRenderHints(QPainter::Antialiasing, true);
			painter.setPen( QColor(0,0,0,100) );
			painter.drawEllipse( QRectF(1,1,width-2,width-2) );
			painter.end();
			setCursor(pixmap);
		}
	}
	if(toolMode == EDIT) {
		QCursor cursor(QPixmap(":icons/smudge.png"),3,16);
		setCursor(cursor); //setCursor(Qt::ArrowCursor);
	}
}

void ScribbleArea::pencilOn() {
	switchTool();
	toolMode = ScribbleArea::PENCIL;
	// --- change properties ---
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) editor->selectColour(pencil.colourNumber);
	if(layer->type == Layer::BITMAP) editor->setColour(pencil.colour);
	editor->setWidth(pencil.width);
	editor->setFeather(pencil.feather);
	editor->setFeather(-1); // by definition the pencil has no feather
	editor->setOpacity(pencil.opacity);
	editor->setPressure(pencil.pressure);
	editor->setInvisibility(pencil.invisibility);
	editor->setInvisibility(-1); // by definition the pencil is invisible in vector mode
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::penOn() {
	switchTool();
	toolMode = ScribbleArea::PEN;
	// --- change properties ---
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) editor->selectColour(pen.colourNumber);
	if(layer->type == Layer::BITMAP) editor->setColour(pen.colour);
	if(pen.width<0) pen.width = 1.0; editor->setWidth(pen.width);
	if(pen.feather<0) pen.feather = 0.0; editor->setFeather(pen.feather);
	if(pen.opacity<0) pen.opacity = 1.0; editor->setOpacity(pen.opacity);
	editor->setPressure(pen.pressure);
	editor->setInvisibility(pen.invisibility);
	editor->setInvisibility(-1); // by definition the pen is visible in vector mode
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::eraserOn() {
	switchTool();
	toolMode = ScribbleArea::ERASER;
	// --- change properties ---
	editor->setWidth(eraser.width);
	editor->setFeather(eraser.feather);
	editor->setOpacity(eraser.opacity);
	editor->setPressure(eraser.pressure);
	editor->setInvisibility(1);
	editor->setInvisibility(-1);
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::selectOn() {
	switchTool();
	toolMode = ScribbleArea::SELECT;
	// --- change properties ---
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	editor->setWidth(-1);
	editor->setFeather(-1);
	editor->setOpacity(-1);
	editor->setPressure(-1);
	editor->setInvisibility(-1);
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::moveOn() {
	toolMode = ScribbleArea::MOVE;
	// --- change properties ---
	editor->setWidth(-1);
	editor->setFeather(-1);
	editor->setOpacity(-1);
	editor->setPressure(-1);
	editor->setInvisibility(-1);
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::handOn() {
	if(toolMode == ScribbleArea::HAND) resetView();
	toolMode = ScribbleArea::HAND;
	// --- change properties ---
	editor->setWidth(-1);
	editor->setFeather(-1);
	editor->setOpacity(-1);
	editor->setPressure(-1);
	editor->setInvisibility(-1);
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::polylineOn() {
	switchTool();
	toolMode = ScribbleArea::POLYLINE;
	// --- change properties ---
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) editor->selectColour(pen.colourNumber);
	if(layer->type == Layer::BITMAP) editor->setColour(pen.colour);
	editor->setWidth(pen.width);
	editor->setFeather(pen.feather);
	editor->setOpacity(pen.opacity);
	editor->setPressure(pen.pressure);
	editor->setInvisibility(pen.invisibility);
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::bucketOn() {
	switchTool();
	toolMode = ScribbleArea::BUCKET;
	// --- change properties ---
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) editor->selectColour(brush.colourNumber);
	if(layer->type == Layer::BITMAP) editor->setColour(brush.colour);
	editor->setWidth(-1);
	editor->setFeather(brush.feather);
	editor->setOpacity(1); // the bucket has full opacity (but one should be able to use it with translucent colours)
	editor->setOpacity(-1);
	editor->setPressure(-1);
	editor->setInvisibility(-1);
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::eyedropperOn() {
	switchTool();
	toolMode = ScribbleArea::EYEDROPPER;
	// --- change properties ---
	editor->setWidth(-1);
	editor->setFeather(-1);
	editor->setOpacity(-1);
	editor->setPressure(-1);
	editor->setInvisibility(-1);
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::colouringOn() {
	switchTool();
	toolMode = ScribbleArea::COLOURING;
	// --- change properties ---
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) editor->selectColour(brush.colourNumber);
	if(layer->type == Layer::BITMAP) editor->setColour(brush.colour);
	editor->setWidth(brush.width);
	editor->setFeather(brush.feather);
	editor->setOpacity(brush.opacity);
	editor->setPressure(brush.pressure);
	editor->setInvisibility(-1);
	// --- change cursor ---
	updateCursor();
}

void ScribbleArea::smudgeOn() {
	switchTool();
	toolMode = EDIT;
	// --- change properties ---
	editor->setWidth(-1);
	editor->setFeather(-1);
	editor->setOpacity(-1);
	editor->setPressure(-1);
	editor->setInvisibility(-1);
	// --- change cursor ---
	updateCursor();
}



void ScribbleArea::deleteSelection() {
	if( somethingSelected ) {  // there is something selected
		editor->backup();
		Layer* layer = editor->getCurrentLayer();
		if(layer == NULL) return;
		closestCurves.clear();
		if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->deleteSelection();
		if(layer->type == Layer::BITMAP) ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0)->clear(mySelection);
		updateAllFrames();
	}
}

void ScribbleArea::clearImage() {
	editor->backup();
	Layer* layer = editor->getCurrentLayer();
	if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(editor->currentFrame, 0)->clear();
	if(layer->type == Layer::BITMAP) ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(editor->currentFrame, 0)->clear();
	//emit modification();
	//update();
	setModified(editor->currentLayer, editor->currentFrame);
}
	
void ScribbleArea::toggleThinLines() {
	showThinLines = !showThinLines;
	emit thinLinesChanged(showThinLines);
	setView(myView);
	updateAllFrames();
}

void ScribbleArea::toggleOutlines() {
	simplified = !simplified;
	emit outlinesChanged(simplified);
	setView(myView);
	updateAllFrames();
}

void ScribbleArea::toggleMirror() {
	myView =  myView * QMatrix(-1, 0, 0, 1, 0, 0);
	myTempView = myView * centralView;
	setView(myView);
	updateAllFrames();
}

void ScribbleArea::toggleShowAllLayers() {
	showAllLayers++;
	if(showAllLayers==3) showAllLayers = 0;
	//emit showAllLayersChanged(showAllLayers);
	setView(myView);
	updateAllFrames();
}

