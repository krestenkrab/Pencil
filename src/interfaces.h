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

#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtPlugin>

class QImage;
class QPainter;
class QWidget;
class QPainterPath;
class QPoint;
class QRect;
class QStringList;

class QMatrix;
class QSize;
class QString;

class Object;
class Layer;

class ExportInterface
{
public:
	virtual ~ExportInterface() {}
	virtual QString name() const = 0;
	virtual bool exportFile(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, int fps) = 0;
};

Q_DECLARE_INTERFACE(ExportInterface,
										"org.pencil.plugins.ExportInterface/1.0")

#endif
