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
#ifndef TIMELINE_H
#define TIMELINE_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QDockWidget>
#include <QScrollBar>
#include <QSound>
#include "toolset.h";

class Editor;

class TimeLine;

/*class TimeLineList : public QWidget
{
    Q_OBJECT
	public:
		TimeLineList(TimeLine *parent = 0, Editor *editor = 0);
		int offsetY;
		int startY, endY, startLayerNumber;
	protected:
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *event);
	private:
		TimeLine *timeLine;
		Editor* editor; // the editor for which this timeLine operates
};*/

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
	
	int getOffset();
	int getOffsetY() { return offsetY; }
	int getLayerHeight() { return layerHeight; }
	
public slots:
		
protected:
	void drawContent();
	void updateContent();
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	
public slots:
	void lengthChange(QString);
	void frameSizeChange(int);
	void fontSizeChange(int);
	void labelChange(int);
	void hScrollChange(int);
	void vScrollChange(int);
	
private:
  TimeLine *timeLine;
	Editor* editor; // the editor for which this timeLine operates
	
	QString type;
	QPixmap* cache;
	bool drawFrameNumber;
	int frameLength;
	int frameSize;
	int fontSize;
	int fps;
	bool scrubbing;
	int layerHeight;
	int offsetX, offsetY;
	int startY, endY, startLayerNumber;
	int frameOffset, layerOffset;
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
	void addKeyClick();
	void removeKeyClick();
	void newBitmapLayer();
	void newVectorLayer();
	void newSoundLayer();
	void deleteCurrentLayer();
		
public:
	TimeLine(QWidget *parent = 0, Editor *editor = 0);
	QScrollBar *hScrollBar, *vScrollBar;
	//int currentFrame;
	//int currentLayer;
	void updateLayerNumber(int number);
	void updateLayerView();
	void updateLength(int frameLength);
	
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


