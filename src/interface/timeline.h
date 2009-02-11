/*

Pencil - Traditional Animation Software
Copyright (C) 2005 Patrick Corrieri
Copyright (C) 2006-2009 Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef TIMELINE_H
#define TIMELINE_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QDockWidget>
#include <QScrollBar>
#include <QSound>
#include "toolset.h"

class Editor;
class Layer;
class LayerImage;
class TimeLine;

class TimeLineCells : public QWidget
{
    Q_OBJECT

public:
	TimeLineCells(TimeLine *parent = 0, Editor *editor = 0, QString type = "");
	//int currentFrame;
	//int currentLayer;
	int getFps() { return fps; }
	int getLayerNumber(int y);
	int getLayerY(int layerNumber);
	int getFrameNumber(int x);
	int getFrameX(int frameNumber);
	int getMouseMoveY() { return mouseMoveY; }
	int getOffsetY() { return offsetY; }
	int getLayerHeight() { return layerHeight; }

signals:
	void mouseMovedY(int);

public slots:
	void updateContent();
	void updateFrame(int frameNumber);
	void lengthChange(QString);
	void frameSizeChange(int);
	void fontSizeChange(int);
	void scrubChange(int);
	void labelChange(int);
	void hScrollChange(int);
	void vScrollChange(int);
	void setMouseMoveY(int x) { mouseMoveY = x;}

protected:
	void drawContent();
	
		// graphic representation and interface reaction -- was originallly in the layer classes
		
		void paintSelection(QPainter &painter, int x, int y, int height, int width);
		void paintLabel(QPainter &painter, Layer *layer, int x, int y, int height, int width, bool selected, int allLayers);
		void paintTrack(QPainter &painter, Layer *layer, int x, int y, int width, int height, bool selected, int frameSize);
		void paintImages(QPainter &painter, LayerImage *layer, int x, int y, int width, int height, bool selected, int frameSize);
		void mousePress(QMouseEvent *event, int frameNumber, Layer* layer);
		void mouseMove(QMouseEvent *event, int frameNumber, Layer* layer);
		void mouseRelease(QMouseEvent *event, int frameNumber, Layer* layer);
		void mouseDoubleClick(QMouseEvent *event, int frameNumber, Layer* layer);
		
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);

private:
  TimeLine *timeLine;
	Editor* editor; // the editor for which this timeLine operates

	QString type;
	QPixmap* cache;
	bool drawFrameNumber;
	bool shortScrub;
	int frameLength;
	int frameSize;
	int fontSize;
	int fps;
	bool scrubbing;
	int layerHeight;
	int offsetX, offsetY;
	int startY, endY, startLayerNumber;
	int mouseMoveY;
	int frameOffset, layerOffset;
	int frameClicked, selectionOffset;
};

class TimeLine : public QDockWidget //DockPalette
{
	Q_OBJECT

signals:
	void modification();
	void lengthChange(QString);
	void frameSizeChange(int);
	void fontSizeChange(int);
	void labelChange(int);
	void scrubChange(int);
	void addKeyClick();
	void removeKeyClick();
	void newBitmapLayer();
	void newVectorLayer();
	void newSoundLayer();
	void newCameraLayer();
	void deleteCurrentLayer();

	void playClick();
	void loopClick();
	void soundClick();
	void fpsClick(int);
	void onionPrevClick();
	void onionNextClick();
	

public slots:
	void updateFrame(int frameNumber);
	void updateLayerNumber(int number);
	void updateLayerView();
	void updateLength(int frameLength);
	void updateContent();

public:
	TimeLine(QWidget *parent = 0, Editor *editor = 0);
	QScrollBar *hScrollBar, *vScrollBar;
	//int currentFrame;
	//int currentLayer;
	bool scrubbing;

protected:
	void resizeEvent(QResizeEvent *event);
	//void mousePressEvent(QMouseEvent *event);
	//void mouseMoveEvent(QMouseEvent *event);
	//void mouseReleaseEvent(QMouseEvent *event);
	//void mouseDoubleClickEvent(QMouseEvent *event);

private:
	Editor* editor; // the editor for which this timeLine operates
	//TimeLineCells* list;
	TimeLineCells* cells;
	TimeLineCells* list;
	int numberOfLayers;

};

#endif
