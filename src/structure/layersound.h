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
#ifndef LAYERSOUND_H
#define LAYERSOUND_H

#include <QImage>
#include <QSize>
#include <QList>
#include <QString>
#include <QPainter>
#include <phonon>
#include "layerimage.h"

class LayerSound : public LayerImage
{
	Q_OBJECT

  public:
	LayerSound(Object* object);
	~LayerSound();
	QDomElement createDomElement(QDomDocument &doc);
	void loadDomElement(QDomElement element, QString filePath);
	
	bool addImageAtFrame(int frameNumber);
	void removeImageAtFrame(int frameNumber);
	
	//void setModified(bool trueOrFalse) { modified = trueOrFalse; }
	
	void loadSoundAtFrame( QString filePathString, int frame );
	
	bool saveImage(int index, QString path, int layerNumber);
	void playSound(int frame);
	void stopSound();
	
  protected:
	//bool modified;
	//QList<int> startingFrame;
	//QList<QString> filePath;
	QList<QString> soundFilepath;
    QList<Phonon::MediaObject*> sound;
    QList<Phonon::AudioOutput*> outputDevices;
    // graphic representation -- could be put in another class
	void swap(int i, int j);

  private slots:
    void addTimelineKey(qint64 newTotalTime);
};

#endif

