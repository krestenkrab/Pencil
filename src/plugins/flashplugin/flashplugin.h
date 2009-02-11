/*

Flash plugin for Pencil
Copyright (C) 2006-2009 Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#ifndef FLASHPLUGIN_H
#define FLASHPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QImage>
#include <QSlider>

#include <interfaces.h>
#include <mingpp.h> // requires the Ming Library

class BitmapImage;
class VectorImage;

class FlashPlugin : public QObject, public ExportInterface
{
    Q_OBJECT
    Q_INTERFACES(ExportInterface)

public:
    QString name() const;
	bool exportFile(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, int fps);
    /*QImage filterImage(const QString &filter, const QImage &image,
                       QWidget *parent);*/
public:
	FlashPlugin();
	QDialog *exportFileDialog;
	QSlider* exportFileDialog_compression;
	void createExportFileDialog();
	bool writeFile(Object* object, int startFrame, int endFrame, QMatrix view, QSize exportSize, QString filePath, int fps, int compression) const;
	void convertToSWFSprite( BitmapImage* image, QMatrix view, SWFSprite* sprite ) const;
	void convertToSWFSprite( VectorImage* vectorImage, Object* object, QMatrix view, SWFSprite* sprite ) const;
	void addShape( SWFSprite* sprite, QPainterPath path, QColor fillColour, QColor borderColour, qreal width, bool fill ) const;
};

#endif
