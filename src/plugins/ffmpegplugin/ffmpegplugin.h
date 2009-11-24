/*

QuickTime Plugin for Pencil
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

#ifndef FFMPEGPLUGIN_H
#define FFMPEGPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QImage>
#include <QSpinBox>
#include <QComboBox>

#include <interfaces.h>

class BitmapImage;
class VectorImage;

class FFmpegPlugin : public QObject, public ExportInterface
{
    Q_OBJECT
    Q_INTERFACES(ExportInterface)

public:
	
	FFmpegPlugin();
    QString name() const;
	bool exportFile(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, int fps);
	
public:
	void exportFFmpeg(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, int fps);
	void exportFrames(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, const char* format, int quality, bool background, bool antialiasing, int gradients);
};

#endif
