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
#include <QtDebug>
#include "layer.h"
#include "object.h"
#include "timeline.h"

Layer::Layer(Object* object) : QObject(0) {
	this->object = object;
	type = Layer::UNDEFINED;
	name = QString("Undefined Layer");
	visible = true;
}

Layer::~Layer() {}

//QImage* Layer::getImageAtFrame(int frameNumber) {
//	qDebug() << "get image from undefined layer!";
//	return NULL; // null by default 
//}

//QImage* Layer::getLastImageAtFrame(int frameNumber) {
//	qDebug() << "get last image from undefined layer!";
//	return NULL; // null by default 
//}

QDomElement Layer::createDomElement(QDomDocument &doc) {
	QDomElement layerTag = doc.createElement("layer");
	layerTag.setAttribute("name", name);
	layerTag.setAttribute("visibility", visible);
	layerTag.setAttribute("type", type);
	return layerTag;
}

void Layer::loadDomElement(QDomElement element) {
	name = element.attribute("name");
	visible = (element.attribute("visibility") == "1");
	type = element.attribute("type").toInt();
}

void Layer::paintTrack(QPainter &painter, TimeLineCells *cells, int x, int y, int width, int height, bool selected, int frameSize) {
	painter.setBrush(Qt::lightGray);
	painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
	painter.drawRect(x, y, width, height); // empty rectangle  by default
	//painter.setFont(QFont("helvetica", height/2));
	//painter.drawText(QPoint(10, y+(2*height)/3), name);
	//if(selected) {
	//	painter.setBrush(QColor(0,0,0,80));
	//	painter.drawRect(x, y-1, width, height);
	//}
	if(selected) {
			QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
			linearGrad.setColorAt(0, QColor(255,255,255,128) );
			linearGrad.setColorAt(0.40, QColor(255,255,255,0) );
			linearGrad.setColorAt(0.60, QColor(0,0,0,0) );
			linearGrad.setColorAt(1, QColor(0,0,0,64) );
			painter.setBrush( linearGrad );
			painter.setPen(QPen(QBrush(QColor(70,70,70)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
			painter.drawRect(x, y-1, width, height);
	}
}

void Layer::paintLabel(QPainter &painter, TimeLineCells *cells, int x, int y, int width, int height, bool selected, int allLayers) {
	painter.setBrush(Qt::lightGray);
	painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
	painter.drawRect(x, y-1, width, height); // empty rectangle  by default
	painter.setFont(QFont("helvetica", height/2));
	painter.setPen(Qt::black);
	painter.drawText(QPoint(20, y+(2*height)/3), name);
	if(visible) {
		if(allLayers==0)  painter.setBrush(Qt::NoBrush);
		if(allLayers==1)   painter.setBrush(Qt::darkGray);
		if((allLayers==2) || selected)  painter.setBrush(Qt::black);
	} else {
		painter.setBrush(Qt::NoBrush);
	}
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.drawEllipse(x+6, y+4, 9, 9);
	painter.setRenderHint(QPainter::Antialiasing, false);
	/*if(selected) {
		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(0,0,0,80));
		painter.drawRect(x, y-1, width, height);
	}*/
	if(selected) {
		paintSelection(painter, x, y, width, height);
	}
}

void Layer::paintSelection(QPainter &painter, int x, int y, int width, int height) {
			QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
			//linearGrad.setColorAt(0, QColor(255,255,255,128) );
			//linearGrad.setColorAt(0.40, QColor(255,255,255,0) );
			//linearGrad.setColorAt(0.60, QColor(0,0,0,0) );
			//linearGrad.setColorAt(1, QColor(0,0,0,64) );
			linearGrad.setColorAt(0, QColor(255,255,255,128) );
			linearGrad.setColorAt(0.10, QColor(255,255,255,64) );
			linearGrad.setColorAt(0.20, QColor(0,0,0,32) );
			linearGrad.setColorAt(0.40, QColor(0,0,0,0) );
			linearGrad.setColorAt(0.41, QColor(255,255,255,0) );
			linearGrad.setColorAt(1, QColor(255,255,255,128) );
			painter.setBrush( linearGrad );
			painter.setPen(QPen(QBrush(QColor(70,70,70)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
			painter.drawRect(x, y-1, width, height);
}

void Layer::mousePress(QMouseEvent *event, int frameNumber) {
}

void Layer::mouseDoubleClick(QMouseEvent *event, int frameNumber) {
}

void Layer::mouseMove(QMouseEvent *event, int frameNumber) {
}

void Layer::mouseRelease(QMouseEvent *event, int frameNumber) {
}


//void Layer::addImageAtFrame(int frameNumber) {
	// nothing by default
//}

/*Layer::Layer(QColor theColour, QString theName) {
	colour = theColour;
	name = theName;
}*/

/*bool Layer::operator==(Layer colourRef1) {
	if( (colour == colourRef1.colour) && (name == colourRef1.name) ) {
		return true;
	} else {
		return false;
	}
}

bool Layer::operator!=(Layer colourRef1) {
	if( (colour != colourRef1.colour) || (name != colourRef1.name) ) {
		return true;
	} else {
		return false;
	}
}*/

