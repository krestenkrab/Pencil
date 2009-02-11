/*

Pencil - Traditional Animation Software
Copyright (C) 2006-2009 Pascal Naidon

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
//#include "timeline.h"

Layer::Layer(Object* object) : QObject(0) {
	this->object = object;
	type = Layer::UNDEFINED;
	id = 0;
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



void Layer::editProperties() {
	bool ok;
	QString text = QInputDialog::getText(NULL, tr("Layer Properties"),
																				tr("Layer name:"), QLineEdit::Normal,
																				name, &ok);
	if (ok && !text.isEmpty()) {
		name = text;
		//palette->updateList();
	}
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

