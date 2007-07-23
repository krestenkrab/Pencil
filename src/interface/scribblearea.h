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
#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QGLWidget>
#include <QFrame>
#include "vectorimage.h"
#include "bitmapimage.h"
#include "colourref.h"

class Editor;
class Layer;

class Properties
{
	public:
		qreal width;
		qreal feather;
		qreal opacity;
		QColor colour;
		int colourNumber;
		bool pressure;
		bool invisibility;
};

class VectorSelection
{
	public:
	QList<VertexRef> vertex;
	QList<int> curve;
	//QList<int> area;
	void clear();
	void add(int curveNumber);
	void add(QList<int> curveNumbers);
	void add(VertexRef point);
	void add(QList<VertexRef> points);
};

/*struct Buffer {
	QList<QPoint> points;
	QList<QColor> colours;
};*/

//class ScribbleArea : public QGLWidget
class ScribbleArea : public QWidget
{
    Q_OBJECT

public:
	ScribbleArea(QWidget *parent = 0, Editor* editor = 0);
	
  void next(const int &i);
	
	void setColour(const int);
	void setColour(const QColor);
	void resetColours();
  
	void deleteSelection();
	void setSelection(QRectF rect, bool);
	void displaySelectionProperties();
	QRectF getSelection() { return mySelection; }
	bool somethingSelected;
	bool readCanvasFromCache;
	
	bool isModified() const { return modified; }
	
	static QBrush getBackgroundBrush(QString);
	//QColor penColour() const { return myPenColour; }
	//double pencilWidth() const { return myPencilWidth; }
	//double penWidth() const { return myPenWidth; }
	//double brushWidth() const { return myBrushWidth; }
	bool thinLines() const { return showThinLines; }
	int allLayers() const { return showAllLayers; }
	QMatrix getView();
	QRectF getViewRect();
	QPointF getCentralPoint();
	
	void updateFrame();
	void updateFrame(int frame);
	void updateAllFrames();
	void updateAllVectorLayersAtCurrentFrame();
	void updateAllVectorLayersAt(int frame);
	void updateAllVectorLayers();
	
signals:
	void modification();
	void modification(int);
	void thinLinesChanged(bool);
	void outlinesChanged(bool);
	//void showAllLayersChanged(bool);
	
public slots:
	void clearImage();
	void calculateSelectionRect();
	void calculateSelectionTransformation();
	void paintTransformedSelection();
	void setModified(int layerNumber, int frameNumber);
	
	void selectAll();
	void deselectAll();

	void onionPrevSlot();
	void onionNextSlot();
	
	//void pressureSlot(int);
	//void invisibleSlot(int);
	
	void pencilOn();
	void eraserOn();	
	void selectOn();	
	void moveOn();
	void handOn();
	void resetView();
	void penOn();
	void polylineOn();
	void bucketOn();
	void eyedropperOn();
	void colouringOn();
	void smudgeOn();
	
	void setWidth(const qreal);
	void setFeather(const qreal);
	void setOpacity(const qreal);
	void setPressure(const bool);
	void setInvisibility(const bool);
	
	void setCurveOpacity(int);
	void setCurveSmoothing(int);
	void setHighResPosition(int);
	void setAntialiasing(int);
	void setGradients(int);
	void setBackground(int);
	void setBackgroundBrush(QString);
	void setShadows(int);
	void setToolCursors(int);
	void setStyle(int);
	void toggleThinLines();
	void toggleOutlines();
	void toggleMirror();
	void toggleShowAllLayers();
	
	void escape();
	//void undo();
	//void redo();
	//void copy();
	//void paste();
	
protected:
	void tabletEvent(QTabletEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void recentre();
	void setView();
	void setView(QMatrix);
	
private:
	void updateCanvas(int frame, QRect rect);
	void setGaussianGradient(QGradient &gradient, QColor coulour, qreal opacity);
	void drawBrush(QPointF thePoint, qreal brushWidth, QColor fillColour, qreal opacity);
	void drawLineTo(const QPointF &endPixel, const QPointF &endPoint);
	void drawEyedropperPreview(const QColor colour);
	void drawPolyline();
	void endPolyline();
	void updateCursor();
	
	void floodFill(VectorImage* vectorImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance);
	void floodFillError(int errorType);
	
	enum myToolModes { PENCIL, ERASER, SELECT, MOVE, EDIT, HAND, SMUDGE, PEN, POLYLINE, BUCKET, EYEDROPPER, COLOURING };
	enum myMoveModes { MIDDLE, TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT };
	myToolModes toolMode;
	myMoveModes moveMode;
	
	void switchTool();
	
	Editor* editor;
	
	bool tabletEraser;
	bool modified;
	bool simplified;
	
	bool showThinLines;
	int showAllLayers;
	bool usePressure, makeInvisible;
	bool highResPosition;
	bool antialiasing;
	bool shadows;
	bool toolCursors;
	int gradients;
	qreal curveOpacity;
	qreal curveSmoothing;
	bool onionPrev, onionNext;
	
	Properties pencil;
	Properties pen;
	Properties brush;
	Properties eraser;
	
	qreal currentWidth;
	QColor currentColour;
	/*qreal myPencilWidth, currentPencilWidth;
	qreal myPenWidth, currentPenWidth;
	qreal myBrushWidth, currentBrushWidth;*/
	//QColor myPenColour, myFillColour;
	//int penColourNumber, fillColourNumber;
	
	QBrush backgroundBrush;
	BitmapImage* bufferImg; // used to pre-draw vector modifications
	//Buffer buffer; // used to pre-draw bitmap modifications, such as lines, brushes, etc.
	QPixmap* eyedropperCursor;
	
	bool mouseInUse;
	QList<QPointF> mousePoints; // copy of points clicked using polyline tool
	QList<QPointF> mousePath; // copy of points drawn using pencil, pen, eraser, etc, tools
	QList<qreal> mousePressure;
	QPointF lastPixel, currentPixel;
	QPointF lastPoint, currentPoint;
	QPointF lastBrushPoint;
	//QBrush brush; // the current brush
	
	qreal tol;
	QList<int> closestCurves;
	QList<VertexRef> closestVertices;
	QPointF offset;
	
	VectorSelection vectorSelection;
	//bool selectionChanged;
	QMatrix selectionTransformation;
	QRectF mySelection, myTransformedSelection, myTempTransformedSelection;
	
	bool tabletInUse;
	qreal tabletPressure;
	QPointF tabletPosition;
	
	QMatrix myView, myTempView, centralView, transMatrix;
	
	QPixmap canvas;
	
	// debug
	QRectF debugRect;
};

#endif


