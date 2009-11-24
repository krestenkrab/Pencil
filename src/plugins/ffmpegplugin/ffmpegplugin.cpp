/*

QuickTime plugin for Pencil
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

#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "ffmpegplugin.h"
#include "object.h"
#include "layercamera.h"

FFmpegPlugin::FFmpegPlugin()
{
}

QString FFmpegPlugin::name() const
{
	return QString("Video (FFmpeg)");
}

bool FFmpegPlugin::exportFile(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, int fps) {
	QSettings settings("Pencil","Pencil");
	settings.setValue("lastExportPath", QVariant(filePath));
	exportFFmpeg(object, startFrame, endFrame, view, layer, exportSize, filePath, fps);
	return true; 
}

void FFmpegPlugin::exportFFmpeg(Object* object, int startFrame, int endFrame, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, int fps) {
	if(!filePath.endsWith(".mov", Qt::CaseInsensitive)) {
		filePath = filePath + ".mov";
	}
	qDebug() << "-------AVI------";
	// --------- Export all the temporary frames ----------
	QDir::temp().mkdir("pencil");
	QString tempPath = QDir::temp().absolutePath()+"/pencil/";
	QProgressDialog progress("Exporting movie...", "Abort", 0, 100, NULL);
	progress.setWindowModality(Qt::WindowModal);
	progress.show();

	QDir dir2(filePath);
	if (QFile::exists(QDir::current().currentPath() +"/plugins/ffmpeg.exe") == true) {
		if (QFile::exists(filePath) == true) { dir2.remove(filePath); }

		exportFrames(object, startFrame, endFrame, view, currentLayer, exportSize, tempPath+"tmp", "png", 100, true, true, 2);
		// --------- Quicktime assemble call ----------
		QDir sampledir;
		qDebug() << "testmic:" << sampledir.filePath(filePath);
		QProcess ffmpeg;

		qDebug() << "Trying to export AVI";
		//ffmpeg.start("./plugins/ffmpeg.exe -formats");
		//ffmpeg.waitForFinished();
		ffmpeg.start("./plugins/ffmpeg.exe -i " + tempPath + "tmp%03d.png -v 0 -r " + QString::number(fps) + " -y \"" + filePath + "\"");
		if (ffmpeg.waitForStarted() == true) {
			if (ffmpeg.waitForFinished() == true) {
				/*
					qDebug() << ffmpeg.readAllStandardOutput();
					qDebug() << ffmpeg.readAllStandardError();

					qDebug() << "dbg:" << QDir::current().currentPath() +"/plugins/";
					qDebug() << ":" << tempPath + "tmp%03d.png";
					qDebug() << ":\"" + filePath + "\"";
				 */
				qDebug() << "AVI export done.";
			} else {
				qDebug() << "ERROR: FFmpeg did not finish executing.";
			}
		} else {
			qDebug() << "ERROR: Could not execute FFmpeg.";
		}
		// --------- Clean up temp directory ---------
		QDir dir(tempPath);
		QStringList filtername("*.*");
		QStringList entries = dir.entryList(filtername,QDir::Files,QDir::Type);
		for(int i=0;i<entries.size();i++)
			dir.remove(entries[i]);
	} else {
		qDebug() << "Please place ffmpeg.exe in plugins directory";
	}
	qDebug() << "-----";
}

void FFmpegPlugin::exportFrames(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, const char* format, int quality, bool background, bool antialiasing, int gradients) {	
	QSettings settings("Pencil","Pencil");
	qreal curveOpacity = (100-settings.value("curveOpacity").toInt())/100.0; // default value is 1.0
	
	QString extension = "";
	QString formatStr = format;
	if( formatStr == "PNG" || formatStr == "png") {
		format = "PNG";  extension = ".png";
	}
	if( formatStr == "JPG" || formatStr == "jpg" || formatStr == "JPEG") {
		format = "JPG";  extension = ".jpg";
		background = true; // JPG doesn't support transparency so we have to include the background
	}
	if(filePath.endsWith(extension, Qt::CaseInsensitive)) {
		filePath = filePath.remove(extension, Qt::CaseInsensitive);
	}
	//qDebug() << "format =" << format << "extension = " << extension;
		
	qDebug() << "Exporting frames from " << startFrame << "to" << endFrame << "at size " << exportSize;
	for(int currentFrame = startFrame; currentFrame <= endFrame ; currentFrame++) {
		QImage tempImage(exportSize, QImage::Format_ARGB32_Premultiplied);
		QPainter painter(&tempImage);
		
		if(layer->type == Layer::CAMERA) {
			QRect viewRect = ((LayerCamera*)layer)->getViewRect();
			QMatrix mapView = Object::map( viewRect, QRectF(QPointF(0,0), exportSize) );
			mapView = ((LayerCamera*)layer)->getViewAtFrame(currentFrame) * mapView;
			painter.setWorldMatrix(mapView);
		} else {
			painter.setWorldMatrix(view);
		}
		object->paintImage(painter, currentFrame, background, curveOpacity, antialiasing, gradients);
		
		QString frameNumberString = QString::number(currentFrame);
		while( frameNumberString.length() < 3) frameNumberString.prepend("0");
		
		tempImage.save(filePath+frameNumberString+extension, format, quality);
	}
}

Q_EXPORT_PLUGIN2(pencil_ffmpegplugin, FFmpegPlugin)
