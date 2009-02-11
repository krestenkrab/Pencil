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
#ifndef LAYER_H
#define LAYER_H

#include <QImage>
#include <QString>
#include <QPainter>
#include <QtGui>
#include <QtXml>

class Object;

class Layer : public QObject
{
	Q_OBJECT

	public:
		Layer(Object* object);
		virtual ~Layer();
		//Layer(QColor theColour, QString theName);
		//bool operator==(ColourRef colourRef1);
		//bool operator!=(ColourRef colourRef1); 
		
		//virtual QImage* getImageAtFrame(int frameNumber);
		//virtual QImage* getLastImageAtFrame(int frameNumber);
		//virtual void addImageAtFrame(int frameNumber);
		
		Object* object;
		int type;
		bool visible;
		int id;
		QString name;
		
		void switchVisibility() { visible = !visible;}
		virtual int getMaxFrame() { return -1;}
		
		virtual QDomElement createDomElement(QDomDocument &doc); // constructs an dom/xml representation of the layer for the document doc
		virtual void loadDomElement(QDomElement element); // construct a layer from a dom/xml representation
		
		virtual void editProperties();
		
	public:
		enum types { UNDEFINED, BITMAP, VECTOR, MOVIE, SOUND, CAMERA };
};

#endif
