/*

Image plugin for Pencil
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

#include "imageplugin.h"
#include "object.h"
#include "layercamera.h"

ImagePlugin::ImagePlugin()
{
	exportFramesDialog = NULL;
	exportFramesDialog_hBox = NULL;
	exportFramesDialog_vBox = NULL;
	exportFramesDialog_format = NULL;
}

QString ImagePlugin::name() const
{
	return QString("Image sequence");
}

bool ImagePlugin::exportFile(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, int fps) {
	QSettings settings("Pencil","Pencil");
	//QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();		
	//if(initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled";
	//QString filePath = QFileDialog::getSaveFileName(this, tr("Export As"),initialPath);
	//if (filePath.isEmpty()) {
	//	return false;
	//} else {
		
		settings.setValue("lastExportPath", QVariant(filePath));
		
		if (!exportFramesDialog) createExportFramesDialog();
		//exportFramesDialog_hBox->setValue( scribbleArea->getViewRect().toRect().width() );
		//exportFramesDialog_vBox->setValue( scribbleArea->getViewRect().toRect().height() );
		
		exportFramesDialog_hBox->setValue( exportSize.width() );
		exportFramesDialog_vBox->setValue( exportSize.height() );
		exportFramesDialog->exec();
		if(exportFramesDialog->result() == QDialog::Rejected) return false;
		
		//QSize exportSize = QSize(exportFramesDialog_hBox->value(), exportFramesDialog_vBox->value());
		////QMatrix view = map( QRectF(QPointF(0,0), scribbleArea->size() ), QRectF(QPointF(0,0), exportSize) );
		//QMatrix view = map( scribbleArea->getViewRect(), QRectF(QPointF(0,0), exportSize) );
		//view = scribbleArea->getView() * view;
	
		QByteArray exportFormat(exportFramesDialog_format->currentText().toLatin1());
		//updateMaxFrame();
		exportFrames(object, startFrame, endFrame, view, layer, exportSize, filePath, exportFormat, -1, false, true, 2);
		return true; 
	//}
}

void ImagePlugin::exportFrames(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, const char* format, int quality, bool background, bool antialiasing, int gradients) {	
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

void ImagePlugin::createExportFramesDialog() {
	exportFramesDialog = new QDialog(NULL, Qt::Dialog);
	QGridLayout *mainLayout = new QGridLayout;
	
	QGroupBox *resolutionBox = new QGroupBox(tr("Resolution"));
	if(exportFramesDialog_hBox == NULL || exportFramesDialog_vBox == NULL) {
		createExportFramesSizeBox();
	}
	QGridLayout *resolutionLayout = new QGridLayout;
	resolutionLayout->addWidget(exportFramesDialog_hBox,0,0);
	resolutionLayout->addWidget(exportFramesDialog_vBox,0,1);
	resolutionBox->setLayout(resolutionLayout);
	
	QGroupBox *formatBox = new QGroupBox(tr("Format"));
	exportFramesDialog_format = new QComboBox();
	exportFramesDialog_format->addItem("PNG");
	exportFramesDialog_format->addItem("JPEG");
	QGridLayout *formatLayout = new QGridLayout;
	formatLayout->addWidget(exportFramesDialog_format,0,0);
	formatBox->setLayout(formatLayout);
	
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), exportFramesDialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), exportFramesDialog, SLOT(reject()));
	
	mainLayout->addWidget(resolutionBox, 0, 0);
	mainLayout->addWidget(formatBox, 1, 0);
	mainLayout->addWidget(buttonBox, 2, 0);
	exportFramesDialog->setLayout(mainLayout);
	exportFramesDialog->setWindowTitle(tr("Options"));
	exportFramesDialog->setModal(true);
}

void ImagePlugin::createExportFramesSizeBox() {
	int defaultWidth = 720; int defaultHeight = 540;
	exportFramesDialog_hBox = new QSpinBox(0);
	exportFramesDialog_hBox->setMinimum(1);
	exportFramesDialog_hBox->setMaximum(10000);
	exportFramesDialog_hBox->setValue(defaultWidth);
	exportFramesDialog_hBox->setFixedWidth(80);
	exportFramesDialog_vBox = new QSpinBox(0);
	exportFramesDialog_vBox->setMinimum(1);
	exportFramesDialog_vBox->setMaximum(10000);
	exportFramesDialog_vBox->setValue(defaultHeight);
	exportFramesDialog_vBox->setFixedWidth(80);
}

Q_EXPORT_PLUGIN2(pencil_imageplugin, ImagePlugin)
