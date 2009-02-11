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
#include <QtDebug>
#include "layerimage.h"
#include "object.h"
//#include "timeline.h"

LayerImage::LayerImage(Object* object) : Layer(object) {
	//imageSize = desiredSize;
	//addImageAtFrame(1);
	//frameClicked = -1;
	//frameOffset = 0;
}

LayerImage::~LayerImage() {
}

int LayerImage::getIndexAtFrame(int frameNumber) {
	int index = -1;
	for(int i=0; i< framesPosition.size(); i++) {
		if(framesPosition.at(i) == frameNumber) index = i;
	}
	return index;
}

int LayerImage::getLastIndexAtFrame(int frameNumber) {
	int position  = -1;
	int index = -1;
	for(int i=0; i < framesPosition.size(); i++) {
		if(framesPosition.at(i) > position && framesPosition.at(i) <= frameNumber) {
			position = framesPosition.at(i);
			index = i;
		}
	}
	return index;
}

QImage* LayerImage::getImageAtIndex(int index) {
	return NULL; // no image -> implemented in subclasses
}

QImage* LayerImage::getImageAtFrame(int frameNumber) {
	int index = getIndexAtFrame(frameNumber);
	if(index == -1) {
		return NULL;
	} else {
		return getImageAtIndex(index);
	}
}


QImage* LayerImage::getLastImageAtFrame(int frameNumber, int increment) {
	int index = getLastIndexAtFrame(frameNumber);
	if(index == -1) {
		return NULL;
	} else {
		return getImageAtIndex(index + increment);
	}
}


bool LayerImage::addImageAtFrame(int frameNumber) {
	int index = getIndexAtFrame(frameNumber);
	if(index == -1) {
		framesPosition.append(frameNumber);
		framesOriginalPosition.append(frameNumber);
		framesSelected.append(false);
		framesFilename.append("");
		framesModified.append(false);
		bubbleSort();
		emit imageAdded(frameNumber);
		return true;
	} else {
		return false;
	}
}

void LayerImage::removeImageAtFrame(int frameNumber) {
	int index = getIndexAtFrame(frameNumber);
	if(index != -1) {
		framesPosition.removeAt(index);
		framesOriginalPosition.removeAt(index);
		framesSelected.removeAt(index);
		framesFilename.removeAt(index);
		framesModified.removeAt(index);
		emit imageRemoved(frameNumber);
	}
	bubbleSort();
}

void LayerImage::bubbleSort()  {
	// from http://fr.wikipedia.org/wiki/Tri_a_bulles
	int i   = 0; /* Indice de repetition du tri */
	int j   = 0; /* Variable de boucle */
	int MAX = framesPosition.size(); /* taille du tableau */
	/* Booleen marquant l'arret du tri si le tableau est ordonne */
	bool en_desordre = true; 
	/* Boucle de repetition du tri et le test qui arrete le tri des que le tableau est ordonne */
	for(i = 0 ; (i < MAX ) && en_desordre; i++) {
		/* Supposons le tableau ordonne */
		en_desordre = false;
		/* Verification des elements des places j et j-1 */
		for(j = 1 ; j < MAX - i ; j++) {
			/* Si les 2 elements sont mal tries */
			if(framesPosition[j] < framesPosition[j-1]) {
				/* Inversion des 2 elements */
				swap(j-1,j);
				/* Le tableau n'est toujours pas trie */
				en_desordre = true;
			}
		}
	}
}

void LayerImage::swap(int i, int j) {
	framesPosition.swap(i,j);
	framesOriginalPosition.swap(i,j);
	framesSelected.swap(i,j);
	framesFilename.swap(i,j);
	framesModified.swap(i,j);
}

void LayerImage::setModified(int frameNumber, bool trueOrFalse) {
	int index = getLastIndexAtFrame(frameNumber);
	if(index != -1) {
		framesModified[index] = trueOrFalse;
		object->modification();
	}
}

void LayerImage::deselectAllFrames() {
	for(int i=0; i < framesPosition.size(); i++) {
		framesSelected[i] = false;
	}
}

void LayerImage::moveSelectedFrames(int frameOffset) {
	for(int i=0; i < framesPosition.size(); i++) {
		if(framesSelected.at(i) && frameOffset != 0) {
			int originalFrame = framesPosition[i];
			framesPosition[i] = originalFrame + frameOffset;
			//framesModified[i] = true;
			emit imageRemoved(originalFrame); // this is to indicate to the cache that an image have been removed here
			emit imageAdded(originalFrame + frameOffset); // this is to indicate to the cache that an image have been added here
			object->modification();
		}
	}
	bubbleSort();
}


bool LayerImage::saveImages(QString path, int layerNumber) {
	qDebug() << "Save images... ";
	QDir dir(path);
	//qDebug() << dir.exists() << dir.path();
	//qDebug() << framesPosition;
	//qDebug() << framesOriginalPosition;
	
	// --- we test if all the files already exists
	for(int i=0; i < framesPosition.size(); i++) {
		QString fileName = framesFilename.at(i);
		qDebug() << "Testing if (" << i << ") " << fileName << " exists";
		bool test = dir.exists(fileName);
		if(!test) {
			qDebug() << "--- The file does not seem to exist.";
			framesModified[i] = true;
		}
	}
	// --- we rename the files for the images which have been moved (if such files exist)
	// --- we do that in two steps, with temporary names in the first step, in order to avoid conflicting names
	for(int i=0; i < framesPosition.size(); i++) {
		int frame1 = framesPosition.at(i);
		int frame0 = framesOriginalPosition.at(i);
		if(frame1 != frame0 && framesFilename.at(i) != "") {
			QString fileName0 = fileName(frame0,layerNumber);
			QString fileName1 = fileName(frame1,layerNumber);
			//qDebug() << fileName0 << fileName1;
			bool rename = dir.rename( fileName0, "tmp"+fileName0 );
			if(rename) { }
		}
	}
	for(int i=0; i < framesPosition.size(); i++) {
		int frame1 = framesPosition.at(i);
		int frame0 = framesOriginalPosition.at(i);
		if(frame1 != frame0 && framesFilename.at(i) != "") {
			QString fileName0 = fileName(frame0,layerNumber);
			QString fileName1 = fileName(frame1,layerNumber);
			bool rename = dir.rename( "tmp"+fileName0, fileName1 );
			if(rename) {
				framesOriginalPosition[i] = frame1;
				framesFilename[i] = fileName1;
				qDebug() << "Rename to " << framesFilename.at(i);
			} else { // the file doesn't exist, we probably need to create it
				qDebug() << "Could not rename to " << framesFilename.at(i);
				framesFilename[i] = "";
				framesModified[i] = true;
			}
		}
	}
	// --- we now save the files for the images which have been modified
	for(int i=0; i < framesPosition.size(); i++) {
		if(framesModified.at(i)) {
			qDebug() << "Trying to save " << framesFilename.at(i);
			saveImage(i, path, layerNumber);
		}
	}
	qDebug() << "done";
}

bool LayerImage::saveImage(int index, QString path, int layerNumber) {
	// implemented in subclasses
}

QString LayerImage::fileName(int index, int layerNumber) {
	// implemented in subclasses
	return "";
}
