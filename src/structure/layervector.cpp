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
#include "layervector.h"
#include <QtDebug>

LayerVector::LayerVector(Object* object) : LayerImage(object) {
	type = Layer::VECTOR;
	name = QString("Vector Layer");
	addImageAtFrame(1);
	//framesVector.append(new VectorImage(imageSize, QImage::Format_ARGB32_Premultiplied, object));
}

LayerVector::~LayerVector() {
  while (!framesVector.empty())
    delete framesVector.takeFirst();
  while (!framesImage.empty())
    delete framesImage.takeFirst();
}

// ------

QImage* LayerVector::getImageAtIndex(int index, QSize size, bool simplified, bool showThinLines, qreal curveOpacity, bool antialiasing, int gradients) {
	if( index < 0 || index >= framesImage.size() ) {
		return NULL;
	} else {
		VectorImage* vectorImage = getVectorImageAtIndex(index);
		QImage* image = framesImage.at(index);
		if(vectorImage->isModified() || size != image->size() ) {
			if( image->size() != size) {
				delete image;
				framesImage[index] = image = new QImage(size, QImage::Format_ARGB32_Premultiplied);
			}
			vectorImage->outputImage(image, size, myView, simplified, showThinLines, curveOpacity, antialiasing, gradients);
			vectorImage->setModified(false);
		}
		return image;
	}
}

QImage* LayerVector::getImageAtFrame(int frameNumber, QSize size, bool simplified, bool showThinLines, qreal curveOpacity, bool antialiasing, int gradients) {
	int index = getIndexAtFrame(frameNumber);
	if(index == -1) {
		return NULL;
	} else {
		return getImageAtIndex(index, size, simplified, showThinLines, curveOpacity, antialiasing, gradients);
	}
}


QImage* LayerVector::getLastImageAtFrame(int frameNumber, int increment, QSize size, bool simplified, bool showThinLines, qreal curveOpacity, bool antialiasing, int gradients) {
	int index = getLastIndexAtFrame(frameNumber);
	if(index == -1) {
		return NULL;
	} else {
		return getImageAtIndex(index + increment, size, simplified, showThinLines, curveOpacity, antialiasing, gradients);
	}
}

// ------

VectorImage* LayerVector::getVectorImageAtIndex(int index) {
	if( index < 0 || index >= framesVector.size() ) {
		return NULL;
	} else {
		return framesVector.at(index);
	}
}

VectorImage* LayerVector::getVectorImageAtFrame(int frameNumber) {
	int index = getIndexAtFrame(frameNumber);
	return getVectorImageAtIndex(index);
}

VectorImage* LayerVector::getLastVectorImageAtFrame(int frameNumber, int increment) {
	int index = getLastIndexAtFrame(frameNumber);
	return getVectorImageAtIndex(index + increment);
}

void LayerVector::setView(QMatrix view) {
	myView = view;
	setModified(true);
}

void LayerVector::setModified(bool trueOrFalse) {
	for(int i=0; i < framesVector.size(); i++) {
		framesVector[i]->setModified(trueOrFalse);
	}
}

// -----

bool LayerVector::usesColour(int index) {
	for(int i=0; i < framesVector.size(); i++) {
		if( framesVector[i]->usesColour(index) ) return true;
	}
	return false;
}

void LayerVector::removeColour(int index) {
	for(int i=0; i < framesVector.size(); i++) {
		framesVector[i]->removeColour(index);
	}
}

/*void LayerBitmap::paint(QPainter &painter, int verticalPosition, int layerHeight, int frameSize, int timeLineWidth) {
	painter.setBrush(QColor(128,128,255));
	painter.setPen(QPen(QBrush(Qt::black), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
	painter.drawRect(0, verticalPosition, timeLineWidth, layerHeight);
	for(int i=0; i < framesPosition.size(); i++) {
		if(framesSelected.at(i)) {
			painter.setBrush(QColor(10,10,10));
			painter.drawRect((framesPosition.at(i)+frameOffset-1)*frameSize, verticalPosition, frameSize, layerHeight);
		}
		else {
			painter.setBrush(QColor(75,75,75));
			painter.drawRect((framesPosition.at(i)-1)*frameSize, verticalPosition, frameSize, layerHeight);
		}
	}
	//painter.setFont(QFont("helvetica", layerHeight/2));
	//painter.drawText(QPoint(10, verticalPosition+(2*layerHeight)/3),"Undefined Layer");
}

void LayerBitmap::mousePress(int frameNumber) {
	frameClicked = frameNumber;
	int index = getIndexAtFrame(frameNumber);
	if(index == -1) {
		// deselect all
		for(int i=0; i < framesPosition.size(); i++) {
			framesSelected[i] = false;
		}
	} else {
		framesSelected[index] = true;
	}
}*/

bool LayerVector::addImageAtFrame(int frameNumber) {
	int index = getIndexAtFrame(frameNumber);
	if(index == -1) {
		//framesVector.append(new VectorImage(imageSize, QImage::Format_ARGB32_Premultiplied, object));
		framesVector.append(new VectorImage(object));
		framesImage.append(new QImage( QSize(2,2), QImage::Format_ARGB32_Premultiplied)); // very small image to begin with

		framesPosition.append(frameNumber);
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

void LayerVector::removeImageAtFrame(int frameNumber) {
	int index = getIndexAtFrame(frameNumber);
	if(index != -1 && framesPosition.size() != 1) {
		delete framesVector.at(index);
		framesVector.removeAt(index);

		delete framesImage.at(index);
		framesImage.removeAt(index);

		framesPosition.removeAt(index);
		framesSelected.removeAt(index);
		framesFilename.removeAt(index);
		framesModified.removeAt(index);
		bubbleSort();
		emit imageRemoved(frameNumber);
	}
}

void LayerVector::loadImageAtFrame(QString path, int frameNumber) {
	if(getIndexAtFrame(frameNumber) == -1) addImageAtFrame(frameNumber);
	int index = getIndexAtFrame(frameNumber);
	//image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	//framesImage[index] = new QImage(path);
}

/*void LayerVector::loadImageAtFrame(VectorImage* picture, int frameNumber) {
	if(getIndexAtFrame(frameNumber) == -1) addImageAtFrame(frameNumber);
	int index = getIndexAtFrame(frameNumber);
	//image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	framesVector[index] = picture;
}*/

void LayerVector::swap(int i, int j) {
	LayerImage::swap(i, j);
	framesVector.swap(i,j);
	framesImage.swap(i,j);
}

void LayerVector::saveImage(int index, QString path, int layerNumber) {
	// not implemented yet
}

QDomElement LayerVector::createDomElement(QDomDocument &doc) {
	QDomElement layerTag = doc.createElement("layer");
	layerTag.setAttribute("name", name);
	layerTag.setAttribute("visibility", visible);
	layerTag.setAttribute("type", type);
	for(int index=0; index < framesPosition.size() ; index++) {
		QDomElement imageTag = framesVector[index]->createDomElement(doc);
		imageTag.setAttribute("frame", framesPosition.at(index));
		layerTag.appendChild(imageTag);
	}
	return layerTag;
}

void LayerVector::loadDomElement(QDomElement element, QString filePath) {
	name = element.attribute("name");
	visible = (element.attribute("visibility") == "1");
	type = element.attribute("type").toInt();

	QDomNode imageTag = element.firstChild();
	while(!imageTag.isNull()) {
		QDomElement imageElement = imageTag.toElement();
		if(!imageElement.isNull()) {
			if(imageElement.tagName() == "image") {
				int frame = imageElement.attribute("frame").toInt();
				addImageAtFrame( frame );
				getVectorImageAtFrame( frame )->loadDomElement(imageElement);
			}
		}
		imageTag = imageTag.nextSibling();
	}
}

