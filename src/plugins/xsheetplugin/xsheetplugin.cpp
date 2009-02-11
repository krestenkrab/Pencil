/*

XSheet plugin for Pencil
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

#include "xsheetplugin.h"
#include "object.h"
#include "layercamera.h"

XsheetPlugin::XsheetPlugin()
{
	/*exportFramesDialog = NULL;
	exportFramesDialog_hBox = NULL;
	exportFramesDialog_vBox = NULL;
	exportFramesDialog_format = NULL;*/
}

QString XsheetPlugin::name() const
{
	return QString("Exposure sheet");
}

bool XsheetPlugin::exportFile(Object* object, int startFrame, int endFrame, QMatrix view, Layer* layer, QSize exportSize, QString filePath, int fps) {
	QSettings settings("Pencil","Pencil");
	//QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();		
	//if(initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled";
	//QString filePath = QFileDialog::getSaveFileName(this, tr("Export As"),initialPath);
	//if (filePath.isEmpty()) {
	//	return false;
	//} else {
		
		settings.setValue("lastExportPath", QVariant(filePath));
		
		/*if (!exportFramesDialog) createExportFramesDialog();
		//exportFramesDialog_hBox->setValue( scribbleArea->getViewRect().toRect().width() );
		//exportFramesDialog_vBox->setValue( scribbleArea->getViewRect().toRect().height() );
		
		exportFramesDialog_hBox->setValue( exportSize.width() );
		exportFramesDialog_vBox->setValue( exportSize.height() );
		exportFramesDialog->exec();
		if(exportFramesDialog->result() == QDialog::Rejected) return false;
		*/
		
		//QSize exportSize = QSize(exportFramesDialog_hBox->value(), exportFramesDialog_vBox->value());
		////QMatrix view = map( QRectF(QPointF(0,0), scribbleArea->size() ), QRectF(QPointF(0,0), exportSize) );
		//QMatrix view = map( scribbleArea->getViewRect(), QRectF(QPointF(0,0), exportSize) );
		//view = scribbleArea->getView() * view;
	
		//QByteArray exportFormat(exportFramesDialog_format->currentText().toLatin1());
		//updateMaxFrame();
		exportXsheet(object, startFrame, endFrame, view, exportSize, filePath, true, true);
		return true; 
	//}
}

void XsheetPlugin::exportXsheet(Object* object, int frameStart, int frameEnd, QMatrix view, QSize exportSize, QString filePath, bool antialiasing, int gradients) {
	QSettings settings("Pencil","Pencil");
	qreal curveOpacity = (100-settings.value("curveOpacity").toInt())/100.0; // default value is 1.0
	
	int page;
	page=0;
	for(int j = frameStart; j <= frameEnd; j=j+15) {
		QImage xImg(QSize(2300,3400), QImage::Format_ARGB32_Premultiplied);
		QPainter xPainter(&xImg);
		xPainter.fillRect(0,0,2300,3400,Qt::white);
		int y = j-1;
		for(int i=j;i<15+page*15 && i<=frameEnd;i++) {
			QRect source = QRect(  QPoint(0,0)  , exportSize );
			QRect target = QRect (  QPoint((y%3)*800+30, (y/3)*680+50-page*3400)  , QSize(640,480) );
			QMatrix thumbView = view * object->map(source, target);
			xPainter.setWorldMatrix( thumbView );
			xPainter.setClipRegion( thumbView.inverted().map( QRegion(target) ) );
			object->paintImage(xPainter, i, false, curveOpacity, antialiasing, gradients);
			xPainter.resetMatrix();
			xPainter.setClipping(false);
			xPainter.setPen( Qt::black );
			xPainter.setFont(QFont("helvetica",50));
			xPainter.drawRect( target );
			xPainter.drawText(QPoint((y%3)*800+35, (y/3)*680+65-page*3400), QString::number(i));
			y++;
		}
		
		if(filePath.endsWith(".jpg", Qt::CaseInsensitive)) {
			filePath = filePath.remove(".jpg", Qt::CaseInsensitive);
		}
		xImg.save(filePath+QString::number(page)+".jpg", "JPG", 60);
		page++;
	}	
}

/*void XsheetPlugin::createExportFramesDialog() {
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

void XsheetPlugin::createExportFramesSizeBox() {
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
}*/

Q_EXPORT_PLUGIN2(pencil_xsheetplugin, XsheetPlugin)
