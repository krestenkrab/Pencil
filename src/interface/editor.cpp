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
#include <QtGui>
#include <QBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QTableWidget>
#include <QListWidget>
#include <QMainWindow>
#include <QTimer>

#include "editor.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "layersound.h"



Editor::Editor(QMainWindow* parent)
{
	mainWindow = parent;
	
	object = NULL; // the editor is initialized with no object
	savedName = "";
	altpress=false;
	backupIndex = -1;
	clipboardBitmapOk = false;
	clipboardVectorOk = false;
	
	QSettings settings("Pencil","Pencil");
	
	fps = settings.value("fps").toInt();
	if (fps==0) { fps=12; settings.setValue("fps", 12); }
	maxFrame = 1;
	timer = new QTimer(this);
	timer->setInterval(1000/fps);
	connect(timer, SIGNAL(timeout()), this, SLOT(playNextFrame()));
	playing = false;
	looping = false;
	
	frameList << 1;
	currentFrame = 1;
	currentLayer = 0;
	
	QHBoxLayout *lay = new QHBoxLayout();
	QVBoxLayout *framelay = new QVBoxLayout();
	
	scribbleArea = new ScribbleArea(this, this);
	timeLine = new TimeLine(this, this);
	
	toolSet = new ToolSet();
	palette = new Palette(this);
	preferences = new Preferences();
	exportFramesDialog = NULL; // will be created when needed
	exportMovieDialog = NULL;
	exportFlashDialog = NULL;
	newDocumentDialog = NULL;
	exportFramesDialog_hBox = NULL;
	exportFramesDialog_vBox = NULL;
	exportFramesDialog_format = NULL;
	exportFlashDialog_compression = NULL;
	
	// FOCUS POLICY
	scribbleArea->setFocusPolicy(Qt::StrongFocus);
	timeLine->setFocusPolicy(Qt::NoFocus);
	toolSet->setFocusPolicy(Qt::NoFocus);
	palette->setFocusPolicy(Qt::NoFocus);

	// CONNECTIONS
	connect(toolSet, SIGNAL(addClick()), this, SLOT(addKey()));
	connect(toolSet, SIGNAL(rmClick()), this, SLOT(removeKey()));
	connect(toolSet, SIGNAL(playClick()), this, SLOT(play()));
	connect(toolSet, SIGNAL(fpsClick(int)), this, SLOT(changeFps(int)));
	connect(toolSet, SIGNAL(loopClick()), this, SLOT(setLoop()));
	
	connect(toolSet, SIGNAL(pencilClick()), scribbleArea, SLOT(pencilOn()));
	connect(toolSet, SIGNAL(eraserClick()), scribbleArea, SLOT(eraserOn()));	
	connect(toolSet, SIGNAL(selectClick()), scribbleArea, SLOT(selectOn()));
	connect(toolSet, SIGNAL(moveClick()), scribbleArea, SLOT(moveOn()));	
	connect(toolSet, SIGNAL(handClick()), scribbleArea, SLOT(handOn()));
	connect(toolSet, SIGNAL(penClick()), scribbleArea, SLOT(penOn()));	
	connect(toolSet, SIGNAL(polylineClick()), scribbleArea, SLOT(polylineOn()));	
	connect(toolSet, SIGNAL(bucketClick()), scribbleArea, SLOT(bucketOn()));
	connect(toolSet, SIGNAL(eyedropperClick()), scribbleArea, SLOT(eyedropperOn()));
	connect(toolSet, SIGNAL(colouringClick()), scribbleArea, SLOT(colouringOn()));
	
	connect(toolSet, SIGNAL(pressureClick(int)), this, SLOT(applyPressure(int)));
	connect(toolSet, SIGNAL(invisibleClick(int)), this, SLOT(applyInvisibility(int)));
	
	connect(toolSet, SIGNAL(widthClick(qreal)), this, SLOT(applyWidth(qreal)));
	connect(toolSet, SIGNAL(featherClick(qreal)), this, SLOT(applyFeather(qreal)));
	connect(toolSet, SIGNAL(opacityClick(qreal)), this, SLOT(applyOpacity(qreal)));
	
	connect(toolSet, SIGNAL(colourClick()), this, SLOT(showPalette()));
	
	connect(toolSet,SIGNAL(clearClick()), scribbleArea, SLOT(clearImage()));
	connect(toolSet, SIGNAL(thinLinesClick()), scribbleArea, SLOT(toggleThinLines()));
	connect(toolSet, SIGNAL(outlinesClick()), scribbleArea, SLOT(toggleOutlines()));
	connect(scribbleArea, SIGNAL(thinLinesChanged(bool)), toolSet, SLOT(changeThinLinesButton(bool)));
	connect(scribbleArea, SIGNAL(outlinesChanged(bool)), toolSet, SLOT(changeOutlinesButton(bool)));
	connect(toolSet, SIGNAL(mirrorClick()), this, SLOT(toggleMirror()));
	connect(toolSet, SIGNAL(prevClick()), scribbleArea, SLOT(onionPrevSlot()));
	connect(toolSet, SIGNAL(nextClick()), scribbleArea, SLOT(onionNextSlot()));
	
	connect(this, SIGNAL(selectAll()), scribbleArea, SLOT(selectAll()));
	
	connect(scribbleArea, SIGNAL(modification()), this, SLOT(modification()));
	connect(timeLine, SIGNAL(modification()), this, SLOT(modification()));
	
	connect(timeLine, SIGNAL(addKeyClick()), this, SLOT(addKey()));
	connect(timeLine, SIGNAL(removeKeyClick()), this, SLOT(removeKey()));
	
	connect(timeLine, SIGNAL(newBitmapLayer()), this, SLOT(newBitmapLayer()));
	connect(timeLine, SIGNAL(newVectorLayer()), this, SLOT(newVectorLayer()));
	connect(timeLine, SIGNAL(newSoundLayer()), this, SLOT(newSoundLayer()));
	connect(timeLine, SIGNAL(deleteCurrentLayer()), this, SLOT(deleteCurrentLayer()));
	
	connect(preferences, SIGNAL(curveOpacityChange(int)), scribbleArea, SLOT(setCurveOpacity(int)));
	connect(preferences, SIGNAL(curveSmoothingChange(int)), scribbleArea, SLOT(setCurveSmoothing(int)));
	connect(preferences, SIGNAL(highResPositionChange(int)), scribbleArea, SLOT(setHighResPosition(int)));
	connect(preferences, SIGNAL(antialiasingChange(int)), scribbleArea, SLOT(setAntialiasing(int)));
	connect(preferences, SIGNAL(gradientsChange(int)), scribbleArea, SLOT(setGradients(int)));
	connect(preferences, SIGNAL(backgroundChange(int)), scribbleArea, SLOT(setBackground(int)));
	connect(preferences, SIGNAL(shadowsChange(int)), scribbleArea, SLOT(setShadows(int)));
	connect(preferences, SIGNAL(styleChange(int)), scribbleArea, SLOT(setStyle(int)));
	
	connect(preferences, SIGNAL(lengthSizeChange(QString)), timeLine, SIGNAL(lengthChange(QString)));
	connect(preferences, SIGNAL(fontSizeChange(int)), timeLine, SIGNAL(fontSizeChange(int)));
	connect(preferences, SIGNAL(frameSizeChange(int)), timeLine, SIGNAL(frameSizeChange(int)));
	connect(preferences, SIGNAL(labelChange(int)), timeLine, SIGNAL(labelChange(int)));
	
	connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardChanged()) );
	
	framelay->addWidget(scribbleArea);
	lay->addWidget(toolSet);
	lay->addLayout(framelay);
	
	framelay->setMargin(0);
	framelay->setSpacing(0);
	lay->setMargin(0);
	lay->setSpacing(0);
	
	setLayout(lay);
	
	qDebug() << "Hello";
	qDebug() << QLibraryInfo::location(QLibraryInfo::PluginsPath);
	qDebug() << QLibraryInfo::location(QLibraryInfo::BinariesPath);
	qDebug() << QLibraryInfo::location(QLibraryInfo::LibrariesPath);
	
	setAcceptDrops(true);
}

Editor::~Editor() {
	// a lot more probably needs to be cleaned here...
	if (object) delete object;
	clearBackup();
}

void Editor::dragEnterEvent(QDragEnterEvent *event)
{
	//if (event->mimeData()->hasFormat("text/plain"))
	event->acceptProposedAction();
}
 
void Editor::dropEvent(QDropEvent *event)
{
	if( event->mimeData()->hasUrls() ) {
		for(int i=0; i < event->mimeData()->urls().size(); i++) {
			if(i>0) scrubForward();
			QUrl url = event->mimeData()->urls()[i];
			QString filePath = url.toLocalFile();
			if(filePath.endsWith(".png") || filePath.endsWith(".jpg") || filePath.endsWith(".jpeg")) 
				importImage( filePath );
			if(filePath.endsWith(".aif") || filePath.endsWith(".mp3") || filePath.endsWith(".wav")) 
				importSound( filePath );
		}
	}
}


void Editor::showCounter(int n) {
	toolSet->setCounter(n);
}

void Editor::newDocument() {
	if (maybeSave()) {
		//if (!exportFramesDialog) createNewDocumentDialog();
		//newDocumentDialog->exec();
		//if(newDocumentDialog->result() == QDialog::Rejected) return;
		//QSize documentSize = QSize(newDocumentDialog_hBox->value(), newDocumentDialog_vBox->value());
		//newObject(documentSize);
		newObject(); // default size
	}
}

void Editor::openDocument()
{
	if (maybeSave()) {
		QSettings settings("Pencil","Pencil");
		QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), myPath);
		if (!fileName.isEmpty()) {
			openObject(fileName);
		}
	}
}

void Editor::openRecent() {
	QSettings settings("Pencil","Pencil");
	QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
	openObject(myPath);
}

bool Editor::saveDocument()
{
	QAction *action = qobject_cast<QAction *>(sender()); // ?
	QByteArray fileFormat = action->data().toByteArray();  // ?
	QSettings settings("Pencil","Pencil");
	QString myPath = settings.value("lastFilePath", QVariant(QDir::homePath())).toString();
	
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), myPath);
			
	if (fileName.isEmpty()) {
		return false;
	} else {
		QSettings settings("Pencil","Pencil");
		settings.setValue("lastFilePath", QVariant(fileName));
		return saveObject(fileName);
	}
}

void Editor::setWidth(qreal width)
{
	scribbleArea->setWidth(width);
	toolSet->setWidth(width);
}

void Editor::applyWidth(qreal width)
{
	setWidth(width);
	Layer* layer = getCurrentLayer(); if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0)->applyWidthToSelection(width);
}

void Editor::setFeather(qreal feather)
{
	scribbleArea->setFeather(feather);
	toolSet->setFeather(feather);
}

void Editor::applyFeather(qreal feather)
{
	setFeather(feather);
	Layer* layer = getCurrentLayer(); if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0)->applyFeatherToSelection(feather);
}

void Editor::setOpacity(qreal opacity)
{
	scribbleArea->setOpacity(opacity);
	toolSet->setOpacity(opacity);
}

void Editor::applyOpacity(qreal opacity)
{
	setOpacity(opacity);
	Layer* layer = getCurrentLayer(); if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0)->applyOpacityToSelection(opacity);
}

void Editor::setInvisibility(int invisibility)
{
	if(invisibility>=0) scribbleArea->setInvisibility(invisibility>0);
	toolSet->setInvisibility(invisibility);
}

void Editor::applyInvisibility(int invisibility)
{
	setInvisibility(invisibility);
	Layer* layer = getCurrentLayer(); if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0)->applyInvisibilityToSelection(invisibility>0);
}

void Editor::setPressure(int pressure)
{
	if(pressure>=0) scribbleArea->setPressure(pressure>0);
	toolSet->setPressure(pressure);
}

void Editor::applyPressure(int pressure)
{
	setPressure(pressure);
	Layer* layer = getCurrentLayer(); if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0)->applyVariableWidthToSelection(pressure>0);
}

void Editor::selectColour(int i)
{
	//currentColour = i;
	if(i > -1) {
		scribbleArea->setColour(i);
		toolSet->setColour(object->getColour(i).colour);
		palette->selectColour(i);
	}
	Layer* layer = getCurrentLayer(); if(layer == NULL) return;
	if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0)->applyColourToSelection(i);
}

void Editor::setColour(QColor colour)
{
	scribbleArea->setColour(colour);
	toolSet->setColour(colour);
}

void Editor::changeColour(int i)
{
	QColor newColour = QColorDialog::getColor(object->getColour(i).colour);
	if (newColour.isValid()) {
		object->setColour(i, newColour);
		Layer* layer = object->getLayer(currentLayer);
		if(layer != NULL) {
			//if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0)->setModified(true);
			if(layer->type == Layer::VECTOR) scribbleArea->setModified(layer, currentFrame);
		}
		palette->updateList();
		selectColour(i);
	}
}

void Editor::addColour() {
	QColor initialColour = Qt::white;
	int currentColourIndex = palette->currentColour();
  if( currentColourIndex > -1 ) {
		initialColour = object->getColour(currentColourIndex).colour;
	}
	QColor newColour = QColorDialog::getColor( initialColour );
	if (newColour.isValid()) {
		object->addColour(newColour);
		palette->updateList();
		//selectColour(object->getColourCount());
	}
}


void Editor::removeColour(int i) {
	if(object->removeColour(i)) {
		palette->updateList();
	} else { 
		QMessageBox::warning(this, tr("Warning"),
                   tr("You cannot remove this colour because it is used!"),
                   QMessageBox::Ok,
									QMessageBox::Ok);
	}
}

void Editor::modification() {
	modified = true;
	if(object != NULL) object->modification();
	lastModifiedFrame = currentFrame;
	lastModifiedLayer = currentLayer;
}

void Editor::backup() {
	if(lastModifiedLayer>-1 && lastModifiedFrame > 0) {
		backup(lastModifiedLayer, lastModifiedFrame);
	}
	if( lastModifiedLayer != currentLayer || lastModifiedFrame != currentFrame ) {
		backup(currentLayer, currentFrame);
	}
}

void Editor::backup(int backupLayer, int backupFrame) {
	while(backupList.size()-1 > backupIndex && backupList.size() > 0) {
		delete backupList.takeLast();
	}
	while(backupList.size() > 7) { // we authorize only 8 levels of cancellation
		delete backupList.takeFirst();
		backupIndex--;
	}
	Layer* layer = object->getLayer(backupLayer);
	if(layer != NULL) {
		if(layer->type == Layer::BITMAP) {
			BackupBitmapElement* element = new BackupBitmapElement();
			element->layer = backupLayer;
			element->frame = backupFrame;
			element->bitmapImage =  ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(backupFrame, 0)->copy();  // copy the image
			backupList.append(element);
			backupIndex++;
		}
		if(layer->type == Layer::VECTOR) {
			BackupVectorElement* element = new BackupVectorElement();
			element->layer = backupLayer;
			element->frame = backupFrame;
			element->vectorImage = *(  ((LayerVector*)layer)->getLastVectorImageAtFrame(backupFrame, 0)  );  // copy the image (that works but I should also provide a copy() method)
			backupList.append(element);
			backupIndex++;
		}
	}
}

void BackupBitmapElement::restore(Editor* editor) {
	Layer* layer = editor->object->getLayer(this->layer);
	if(layer != NULL) {
		if(layer->type == Layer::BITMAP) {
			*(   ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(this->frame, 0)    ) = this->bitmapImage;  // restore the image
		}
	}
	editor->updateFrame(this->frame);
	editor->scrubTo(this->frame);
}

void BackupVectorElement::restore(Editor* editor) {
	Layer* layer = editor->object->getLayer(this->layer);
	if(layer != NULL) {
		if(layer->type == Layer::VECTOR) {
			*(  ((LayerVector*)layer)->getLastVectorImageAtFrame(this->frame, 0)  ) = this->vectorImage;  // restore the image
			//((LayerVector*)layer)->getLastVectorImageAtFrame(this->frame, 0)->setModified(true); // why?
			//editor->scribbleArea->setModified(layer, this->frame);
		}
	}
	editor->updateFrameAndVector(this->frame);
	editor->scrubTo(this->frame);
}

void Editor::undo() {
	if( backupList.size() > 0 && backupIndex > -1) {
		if(backupIndex == backupList.size()-1) {
			BackupElement* lastBackupElement = backupList[backupIndex];
			if(lastBackupElement->type() == BackupElement::BITMAP_MODIF) {
				BackupBitmapElement* lastBackupBitmapElement = (BackupBitmapElement*)lastBackupElement;
				backup( lastBackupBitmapElement->layer, lastBackupBitmapElement->frame );
				backupIndex--;
			}
			if(lastBackupElement->type() == BackupElement::VECTOR_MODIF) {
				BackupVectorElement* lastBackupVectorElement = (BackupVectorElement*)lastBackupElement;
				backup( lastBackupVectorElement->layer, lastBackupVectorElement->frame );
				backupIndex--;
			}
		}
		//
		backupList[backupIndex]->restore(this);
		backupIndex--;
		scribbleArea->calculateSelectionRect(); // really ugly -- to improve
	}
}

void Editor::redo() {
	if( backupList.size() > 0 && backupIndex < backupList.size()-2) {
		backupIndex++;
		backupList[backupIndex+1]->restore(this);
	}
}

void Editor::clearBackup() {
	backupIndex = -1;
	while( !backupList.isEmpty() ) {
		delete backupList.takeLast();
	}
	lastModifiedLayer = -1;
	lastModifiedFrame = -1;
}

void Editor::copy() {
	Layer* layer = object->getLayer(currentLayer);
	if(layer != NULL) {
		if(layer->type == Layer::BITMAP) {
			if(scribbleArea->somethingSelected) {
				clipboardBitmapImage =   ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(currentFrame, 0)->copy( scribbleArea->getSelection().toRect() );  // copy part of the image
			} else {
				clipboardBitmapImage =   ((LayerBitmap*)layer)->getLastBitmapImageAtFrame(currentFrame, 0)->copy();  // copy the whole image
			}
			clipboardBitmapOk = true;
			if( clipboardBitmapImage.image != NULL ) QApplication::clipboard()->setImage( *(clipboardBitmapImage.image) );
		}
		if(layer->type == Layer::VECTOR) {
			clipboardVectorOk = true;
			clipboardVectorImage = *(  ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0)  );  // copy the image (that works but I should also provide a copy() method)
		}
	}
}

void Editor::paste() {
	Layer* layer = object->getLayer(currentLayer);
	if(layer != NULL) {
		if(layer->type == Layer::BITMAP && clipboardBitmapImage.image != NULL) { // clipboardBitmapOk
			backup();
			BitmapImage tobePasted = clipboardBitmapImage.copy();
			qDebug() << "to be pasted --->" << tobePasted.image->size();
			if(scribbleArea->somethingSelected) {
				QRectF selection = scribbleArea->getSelection();
				if( clipboardBitmapImage.width() <= selection.width() && clipboardBitmapImage.height() <= selection.height() ) {
					tobePasted.moveTopLeft( selection.topLeft() );
				} else {
					tobePasted.transform( selection, true );
				}
			}
			((LayerBitmap*)layer)->getLastBitmapImageAtFrame(currentFrame, 0)->paste( &tobePasted ); // paste the clipboard
		}
		if(layer->type == Layer::VECTOR && clipboardVectorOk) {
			backup();
			scribbleArea->deselectAll();
			VectorImage* vectorImage = ((LayerVector*)layer)->getLastVectorImageAtFrame(currentFrame, 0);
			vectorImage->paste( clipboardVectorImage );  // paste the clipboard
			scribbleArea->setSelection( vectorImage->getSelectionRect() );
			//((LayerVector*)layer)->getLastVectorImageAtFrame(backupFrame, 0)->modification(); ????
		}
	}
	scribbleArea->updateFrame();
}

void Editor::clipboardChanged() {
	if(clipboardBitmapOk == false) {
		clipboardBitmapImage.image = new QImage( QApplication::clipboard()->image() );
		clipboardBitmapImage.boundaries = QRect( clipboardBitmapImage.topLeft(), clipboardBitmapImage.image->size() );
		qDebug() << "New clipboard image" << clipboardBitmapImage.image->size();
	} else {
		clipboardBitmapOk = false;
		qDebug() << "The image has been saved in the clipboard";
	}
}

void Editor::newBitmapLayer() {
	if(object != NULL) {
		object->addNewBitmapLayer();
		currentLayer = object->getLayerCount()-1;
		timeLine->updateLayerNumber( object->getLayerCount() );
		scribbleArea->updateAllFrames();
	}
}

void Editor::newVectorLayer() {
	if(object != NULL) {
		object->addNewVectorLayer();
		currentLayer = object->getLayerCount()-1;
		timeLine->updateLayerNumber( object->getLayerCount() );
		scribbleArea->updateAllFrames();
	}
}

void Editor::newSoundLayer() {
	if(object != NULL) {
		object->addNewSoundLayer();
		currentLayer = object->getLayerCount()-1;
		timeLine->updateLayerNumber( object->getLayerCount() );
	}
}

void Editor::deleteCurrentLayer() {
	int ret = QMessageBox::warning(this, tr("Warning"),
                   tr("Are yo sure you want to delete the selected layer?"),
                   QMessageBox::Ok | QMessageBox::Cancel,
			QMessageBox::Ok);
	if(ret == QMessageBox::Ok) {
		object->deleteLayer(currentLayer);
		if(currentLayer == object->getLayerCount()) currentLayer = currentLayer-1;
		timeLine->updateLayerNumber( object->getLayerCount() );
		//timeLine->update();
		scribbleArea->updateAllFrames();
	}
}


void Editor::toggleMirror() {
	object->toggleMirror();
	scribbleArea->toggleMirror();
}

void Editor::toggleShowAllLayers() {
	scribbleArea->toggleShowAllLayers();
	timeLine->update();
}

void Editor::resetMirror() {
	object->resetMirror();
	toolSet->resetMirror();
}

void Editor::showPalette() {
	if(palette->isVisible()) {
		palette->close();
	} else {
		palette->show();
	}
}

void Editor::saveLength(QString x) {
	bool ok;
    int dec = x.toInt(&ok, 10);   
	QSettings settings("Pencil","Pencil");
	settings.setValue("length", dec);
}

void Editor::about()
{
			QMessageBox::about(this, tr("Pencil 0.4.3b"),
            tr("<div style='background-color: #DDDDDD; text-align: center'>"
							 "<img src=':icons/logo.png' width='100%'><br>"
							 "<table style='background-color: #DDDDDD'><tr><td width='160px'>"
							 "Developed by: <i>Pascal Naidon & Patrick Corrieri</i><br>"
							 "Version: <b>0.4.3b</b> (1st July, 2007)<br><br>"
               "<b>Thanks to:</b><br>"
               "Trolltech for the Qt libraries<br>"
               "Roland for the Movie export functions<br>"
							 "Axel for his help with Qt<br>"
							 "Mark for his help with Qt and SVN<br><br>"
               "<a href='http://www.les-stooges.org/pascal/pencil'>http://www.les-stooges.org/pascal/pencil/</a><br><br>"
							 "Distributed under the GPL License."
							 "</td></tr></table>"
							 "</div>"));
}

void Editor::helpBox() {
	QMessageBox::about(this, tr("Hummm..."), tr("There's supposed to be a README file...<br><br>"
		"For more information, please visit:<br>"
		"<a href='http://www.les-stooges.org/pascal/pencil'>http://www.les-stooges.org/pascal/pencil/</a>."));
	}


bool Editor::maybeSave()
{
    if (object->modified) {
        int ret = QMessageBox::warning(this, tr("Warning"),
                          tr("This animation has been modified.\n"
                             "Do you want to save your changes?"),
                          QMessageBox::Yes | QMessageBox::Default,
                          QMessageBox::No,
                          QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes) {
            saveForce();
			return true; 
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

bool Editor::saveObject(QString filePath)
{
	QFileInfo fileInfo(filePath);
	if(fileInfo.isDir()) return false;
	
	QFileInfo dataInfo(filePath+".data");
	if(!dataInfo.exists()) {
		QDir dir(fileInfo.absolutePath()); // the directory where filePath is or will be saved
		dir.mkpath(filePath+".data"); // creates a directory with the same name +".data"
	}
	
	savedName=filePath;
	mainWindow->setWindowTitle(savedName);
	
	// save data
	for(int i=0; i < object->getLayerCount(); i++) {
		Layer* layer = object->getLayer(i);
		if(layer->type == Layer::BITMAP) ((LayerBitmap*)layer)->saveImages(filePath+".data", i);
		//if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->saveImages(filePath+".data", i);
		if(layer->type == Layer::SOUND) ((LayerSound*)layer)->saveImages(filePath+".data", i);
	}
	
	// save palette
	object->savePalette(filePath+".data");
	
	// save object
	object->write(filePath);

	object->modified = false;
	return true;
}

void Editor::newObject() {
	//if(object != NULL) delete object;
	Object *newObject = new Object();
	newObject->defaultInitialisation();
	
	setObject(newObject);
	updateObject();
	savedName = "";
	mainWindow->setWindowTitle(tr("Pencil v0.4.3b"));
}

void Editor::setObject(Object *object) {
	if ((this->object)&&(object!=this->object)) {
		disconnect( this->object, 0, 0, 0); // disconnect the current object from everything
		delete this->object;
	}
	this->object = object;
	if(object) {
		connect( object, SIGNAL(imageAdded(int)), this, SLOT(addFrame(int)) );
		connect( object, SIGNAL(imageRemoved(int)), this, SLOT(removeFrame(int)) );
		
		//currentLayer = object->getLayerCount()-1; // the default selected layer is the last one
		currentLayer = 0; // the default selected layer is the first one
		currentFrame = 1;
		frameList.clear(); frameList << 1;
	}
}

void Editor::updateObject() {
		scribbleArea->resetColours();
		palette->selectColour(0);
		
		//scribbleArea->resize(object->size);
		//scribbleArea->updateAllFrames();
		scribbleArea->resetView();
		timeLine->updateLayerNumber(object->getLayerCount());
		palette->updateList();
		clearBackup();
		scribbleArea->resetColours();
		palette->selectColour(0);
		scribbleArea->updateAllFrames();
}

void Editor::openObject(QString fileName) {
	savedName = fileName;
	QSettings settings("Pencil","Pencil");
	settings.setValue("lastFilePath", QVariant(savedName) );
	mainWindow->setWindowTitle(savedName);
	
	Object* newObject = new Object();
	if(!newObject->loadPalette(savedName+".data")) newObject->loadDefaultPalette();
	setObject(newObject);
	newObject->read(savedName);
	updateObject();
}

void Editor::saveForce() {
	if (savedName!="")
		saveObject(savedName);
	else saveDocument();
}		

void Editor::createNewDocumentDialog() {
	/*newDocumentDialog = new QDialog(this, Qt::Dialog);
	QGridLayout *mainLayout = new QGridLayout;
	
	QGroupBox *resolutionBox = new QGroupBox(tr("Resolution"));
	newDocumentDialog_hBox = new QSpinBox(this);
	newDocumentDialog_hBox->setMinimum(1);
	newDocumentDialog_hBox->setMaximum(10000);
	int defaultWidth = 720; int defaultHeight = 540;
	//if(object != NULL) {
	//	defaultWidth = object->size.width(); defaultHeight = object->size.height();
	//}
	newDocumentDialog_hBox->setValue(defaultWidth);
	newDocumentDialog_hBox->setFixedWidth(80);
	newDocumentDialog_vBox = new QSpinBox(this);
	newDocumentDialog_vBox->setMinimum(1);
	newDocumentDialog_vBox->setMaximum(10000);
	newDocumentDialog_vBox->setValue(defaultHeight);
	newDocumentDialog_vBox->setFixedWidth(80);
	QGridLayout *resolutionLayout = new QGridLayout;
	resolutionLayout->addWidget(newDocumentDialog_hBox,0,0);
	resolutionLayout->addWidget(newDocumentDialog_vBox,0,1);
	resolutionBox->setLayout(resolutionLayout);
	
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), newDocumentDialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), newDocumentDialog, SLOT(reject()));
	
	mainLayout->addWidget(resolutionBox, 0, 0);
	mainLayout->addWidget(buttonBox, 1, 0);
	newDocumentDialog->setLayout(mainLayout);
	newDocumentDialog->setWindowTitle(tr("Options"));
	newDocumentDialog->setModal(true);*/
}

void Editor::createExportSizeBox() {
	exportFramesDialog_hBox = new QSpinBox(this);
	exportFramesDialog_hBox->setMinimum(1);
	exportFramesDialog_hBox->setMaximum(10000);
	int defaultWidth = 720; int defaultHeight = 540;
	//if(object != NULL) {
		//defaultWidth = object->size.width(); defaultHeight = object->size.height();
	//}
	exportFramesDialog_hBox->setValue(defaultWidth);
	exportFramesDialog_hBox->setFixedWidth(80);
	exportFramesDialog_vBox = new QSpinBox(this);
	exportFramesDialog_vBox->setMinimum(1);
	exportFramesDialog_vBox->setMaximum(10000);
	exportFramesDialog_vBox->setValue(defaultHeight);
	exportFramesDialog_vBox->setFixedWidth(80);
}

void Editor::createExportFramesDialog() {
	exportFramesDialog = new QDialog(this, Qt::Dialog);
	QGridLayout *mainLayout = new QGridLayout;
	
	QGroupBox *resolutionBox = new QGroupBox(tr("Resolution"));
	if(exportFramesDialog_hBox == NULL || exportFramesDialog_vBox == NULL) {
		createExportSizeBox();
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

void Editor::createExportMovieDialog() {
	exportMovieDialog = new QDialog(this, Qt::Dialog);
	QGridLayout *mainLayout = new QGridLayout;
	
	QGroupBox *resolutionBox = new QGroupBox(tr("Resolution"));
	if(!exportFramesDialog_hBox || !exportFramesDialog_vBox) {
		createExportSizeBox();
	}
	QGridLayout *resolutionLayout = new QGridLayout;
	resolutionLayout->addWidget(exportFramesDialog_hBox,0,0);
	resolutionLayout->addWidget(exportFramesDialog_vBox,0,1);
	resolutionBox->setLayout(resolutionLayout);
	
	QGroupBox *formatBox = new QGroupBox(tr("Format"));
	exportFramesDialog_format = new QComboBox();
	exportFramesDialog_format->addItem("MOV");
	//exportFramesDialog_format->addItem("JPEG");
	QGridLayout *formatLayout = new QGridLayout;
	formatLayout->addWidget(exportFramesDialog_format,0,0);
	formatBox->setLayout(formatLayout);
	
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), exportMovieDialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), exportMovieDialog, SLOT(reject()));
	
	mainLayout->addWidget(resolutionBox, 0, 0);
	mainLayout->addWidget(formatBox, 1, 0);
	mainLayout->addWidget(buttonBox, 2, 0);
	exportMovieDialog->setLayout(mainLayout);
	exportMovieDialog->setWindowTitle(tr("Options"));
	exportMovieDialog->setModal(true);
}


void Editor::createExportFlashDialog() {
	exportFlashDialog = new QDialog(this, Qt::Dialog);
	QGridLayout *mainLayout = new QGridLayout;
	
	QSettings settings("Pencil","Pencil");
	
	
	exportFlashDialog_compression = new QSlider(Qt::Horizontal);
	exportFlashDialog_compression->setTickPosition(QSlider::TicksBelow);
	exportFlashDialog_compression->setMinimum(0);
	exportFlashDialog_compression->setMaximum(10);
	exportFlashDialog_compression->setValue( 10 - settings.value("flashCompressionLevel").toInt() );
	QLabel* label1 = new QLabel("Large file");
	QLabel* label2 = new QLabel("Small file");
	
	QGroupBox *compressionBox = new QGroupBox(tr("Compression"));
	QGridLayout *compressionLayout = new QGridLayout;
	compressionLayout->addWidget(label1,0,0);
	compressionLayout->addWidget(exportFlashDialog_compression,0,1);
	compressionLayout->addWidget(label2,0,2);
	compressionBox->setLayout(compressionLayout);
	
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	connect(buttonBox, SIGNAL(accepted()), exportFlashDialog, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), exportFlashDialog, SLOT(reject()));
	
	mainLayout->addWidget(compressionBox, 0, 0);
	mainLayout->addWidget(buttonBox, 1, 0);
	exportFlashDialog->setLayout(mainLayout);
	exportFlashDialog->setWindowTitle(tr("Options"));
	exportFlashDialog->setModal(true);
}

QMatrix Editor::map(QRectF source, QRectF target) { // this method should be put somewhere else...
		qreal x1 = source.left(); qreal y1 = source.top(); qreal x2 = source.right(); qreal y2 = source.bottom();
		qreal x1P = target.left(); qreal y1P = target.top(); qreal x2P = target.right(); qreal y2P = target.bottom();
		QMatrix matrix;
		bool mirror = false;
		if( (x1 != x2) && (y1 != y2) ) {
			if( !mirror) {
				matrix = QMatrix ( (x2P-x1P)/(x2-x1), 0, 0, (y2P-y1P)/(y2-y1), (x1P*x2-x2P*x1)/(x2-x1), (y1P*y2-y2P*y1)/(y2-y1) );
			} else {
				matrix = QMatrix ( (x2P-x1P)/(x1-x2), 0, 0, (y2P-y1P)/(y2-y1), (x1P*x1-x2P*x2)/(x1-x2), (y1P*y2-y2P*y1)/(y2-y1) );
			}
		} else {
			matrix.reset();
		}
	return matrix;
}

bool Editor::exportSeq() {
	QSettings settings("Pencil","Pencil");
	QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();		
	if(initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled";
	QString filePath = QFileDialog::getSaveFileName(this, tr("Export As"),initialPath);
	if (filePath.isEmpty()) {
		return false;
	} else {
		settings.setValue("lastExportPath", QVariant(filePath));
		
		if (!exportFramesDialog) createExportFramesDialog();
		exportFramesDialog_hBox->setValue( scribbleArea->size().width() );
		exportFramesDialog_vBox->setValue( scribbleArea->size().height() );
		exportFramesDialog->exec();
		if(exportFramesDialog->result() == QDialog::Rejected) return false;
		
		QSize exportSize = QSize(exportFramesDialog_hBox->value(), exportFramesDialog_vBox->value());
		QMatrix view = map( QRectF(QPointF(0,0), scribbleArea->size() ), QRectF(QPointF(0,0), exportSize) );
		view = scribbleArea->getView() * view;
	
		QByteArray exportFormat(exportFramesDialog_format->currentText().toLatin1());
		updateMaxFrame();		
		object->exportFrames(1, maxFrame, view, exportSize, filePath, exportFormat, -1, false, true, 2);
		return true; 
	}
}

bool Editor::exportX() {
	QSettings settings("Pencil","Pencil");
	QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();		
	if(initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled";
	QString filePath = QFileDialog::getSaveFileName(this, tr("Save As"),initialPath);
	if (filePath.isEmpty()) {
		qDebug() << "empty file";
		return false;
	} else {
		settings.setValue("lastExportPath", QVariant(filePath));
		updateMaxFrame();
		QMatrix view = scribbleArea->getView();
		object->exportX(1, maxFrame, view, scribbleArea->size(), filePath, true, 2);
		return true; 
	}
}


bool Editor::exportMov() {
	QSettings settings("Pencil","Pencil");
	QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();		
	if(initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled";
	QString filePath = QFileDialog::getSaveFileName(this, tr("Export As"),initialPath);
	if (filePath.isEmpty()) {
		return false;
	} else {
		settings.setValue("lastExportPath", QVariant(filePath));
		if (!exportMovieDialog) createExportMovieDialog();
		if(!exportFramesDialog_hBox || !exportFramesDialog_vBox) {
			createExportSizeBox();
		}
		exportFramesDialog_hBox->setValue( scribbleArea->size().width() );
		exportFramesDialog_vBox->setValue( scribbleArea->size().height() );
		exportMovieDialog->exec();
		if(exportMovieDialog->result() == QDialog::Rejected) return false;
		
		QSize exportSize = QSize(exportFramesDialog_hBox->value(), exportFramesDialog_vBox->value());
		QMatrix view = map( QRectF(QPointF(0,0), scribbleArea->size() ), QRectF(QPointF(0,0), exportSize) );
		view = scribbleArea->getView() * view;
		
		updateMaxFrame();
		object->exportMovie(1, maxFrame, view, exportSize, filePath, fps);
		return true; 
	}
}

bool Editor::exportFlash() {
	QSettings settings("Pencil","Pencil");
	QString initialPath = settings.value("lastExportPath", QVariant(QDir::homePath())).toString();		
	if(initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled";
	QString filePath = QFileDialog::getSaveFileName(this, tr("Export As"),initialPath);
	if (filePath.isEmpty()) {
		return false;
	} else {
		settings.setValue("lastExportPath", QVariant(filePath));
		if (!exportFlashDialog) createExportFlashDialog();
		exportFlashDialog->exec();
		if(exportFlashDialog->result() == QDialog::Rejected) return false;
		
		settings.setValue("flashCompressionLevel", 10-exportFlashDialog_compression->value() );
	
		QSize exportSize = scribbleArea->size();
		updateMaxFrame();
		object->exportFlash(1, maxFrame, scribbleArea->getView(), exportSize, filePath, fps, exportFlashDialog_compression->value());
		return true; 
	}
}

void Editor::importImage()
{
	importImage("fromDialog");
}

void Editor::importImage(QString filePath)
{
	Layer* layer = object->getLayer(currentLayer);
	if(layer != NULL) {
		if( layer->type == Layer::BITMAP) {
			if(filePath == "fromDialog") {
				QSettings settings("Pencil","Pencil");
				QString initialPath = settings.value("lastImportPath", QVariant(QDir::homePath())).toString();		
				if(initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled";
				filePath = QFileDialog::getOpenFileName(this, tr("Import image..."),initialPath);
				if (!filePath.isEmpty()) settings.setValue("lastImportPath", QVariant(filePath));
			}
			if (!filePath.isEmpty()) {
				backup();
				// --- option 1
				//((LayerBitmap*)layer)->loadImageAtFrame(filePath, currentFrame);
				// --- option 2
				// TO BE IMPROVED
				BitmapImage* bitmapImage = ((LayerBitmap*)layer)->getBitmapImageAtFrame(currentFrame);
				if(bitmapImage == NULL) { addKey(); bitmapImage = ((LayerBitmap*)layer)->getBitmapImageAtFrame(currentFrame); }
				QImage* importedImage = new QImage(filePath);
				QRect boundaries = importedImage->rect();
				//boundaries.moveTopLeft( scribbleArea->getView().inverted().map(QPoint(0,0)) );
				boundaries.moveTopLeft( scribbleArea->getCentralPoint().toPoint() - QPoint(boundaries.width()/2, boundaries.height()/2) );
				BitmapImage* importedBitmapImage = new BitmapImage(NULL, boundaries, *importedImage);
				if(scribbleArea->somethingSelected) {
					QRectF selection = scribbleArea->getSelection();
					if( importedImage->width() <= selection.width() && importedImage->height() <= selection.height() ) {
						importedBitmapImage->boundaries.moveTopLeft( selection.topLeft().toPoint() );
					} else {
						importedBitmapImage->transform( selection.toRect(), true );
					}
				}
				bitmapImage->paste( importedBitmapImage );
				
				scribbleArea->updateFrame();
				timeLine->update();
			}
		}  else {
			// create a new Bitmap layer ?
			QMessageBox::warning(this, tr("Warning"),
				tr("Please select a bitmap layer in order to import images."),
				QMessageBox::Ok,
				QMessageBox::Ok);
		}
	}
}

void Editor::importSound()
{
	importSound("fromDialog");
}

void Editor::importSound(QString filePath)
{
	Layer* layer = object->getLayer(currentLayer);
	if(layer != NULL) {
		if( layer->type == Layer::SOUND) {
			if(filePath == "fromDialog") {
				QSettings settings("Pencil","Pencil");
				QString initialPath = settings.value("lastImportPath", QVariant(QDir::homePath())).toString();		
				if(initialPath.isEmpty()) initialPath = QDir::homePath() + "/untitled";
				QString filePath = QFileDialog::getOpenFileName(this, tr("Import sound..."),initialPath);
				if (!filePath.isEmpty()) settings.setValue("lastImportPath", QVariant(filePath));
			}
			if (!filePath.isEmpty()) {
				((LayerSound*)layer)->loadSoundAtFrame(filePath, currentFrame);
				timeLine->update();
			}	
		} else {
			// create a new Sound layer ?
			QMessageBox::warning(this, tr("Warning"),
				tr("Please select a sound layer in order to import sounds."),
				QMessageBox::Ok,
				QMessageBox::Ok);
		}
	}
}

void Editor::updateFrame(int frameNumber) {
	scribbleArea->updateFrame(frameNumber);
}

void Editor::updateFrameAndVector(int frameNumber) {
	scribbleArea->updateAllVectorLayersAt(frameNumber);
}

void Editor::scrubTo(int frameNumber) {
	if(frameNumber < 1) frameNumber = 1;
	currentFrame = frameNumber;
	//toolSet->setCounter(frameNumber);
	//timeLine->setCurrentFrame(currentFrame);
	timeLine->update();
	scribbleArea->update();
}

void Editor::scrubForward() {
	scrubTo(currentFrame+1);
}

void Editor::scrubBackward() {
	if(currentFrame > 1) scrubTo(currentFrame-1);
}

/*void Editor::scrubKF() {
	//timeLine->scrubKF();
}

void Editor::scrubKB() {
	//timeLine->scrubKB();
}*/

void Editor::previousLayer() {
	currentLayer--;
	if(currentLayer<0) currentLayer = 0;
	timeLine->update();
	scribbleArea->updateAllFrames();
}

void Editor::nextLayer() {
	currentLayer++;
	if(currentLayer == object->getLayerCount()) currentLayer = object->getLayerCount()-1;
	timeLine->update();
	scribbleArea->updateAllFrames();
}

void Editor::addKey() {
	addKey(currentLayer, currentFrame);
}

void Editor::addKey(int layerNumber, int &frameNumber) {
	Layer* layer = object->getLayer(layerNumber);
	if(layer != NULL) {
		if(layer->type == Layer::BITMAP || layer->type == Layer::VECTOR) {
			bool success = false;
			if(layer->type == Layer::BITMAP) success = ((LayerBitmap*)layer)->addImageAtFrame(frameNumber);
			if(layer->type == Layer::VECTOR) success = ((LayerVector*)layer)->addImageAtFrame(frameNumber);
			if(success) {
				timeLine->update();
				//scribbleArea->addFrame(frameNumber);
			} else {
				frameNumber++;
				addKey();
			}
		}
	}
}

void Editor::removeKey() {
	Layer* layer = object->getLayer(currentLayer);
	if(layer != NULL) {
		if(layer->type == Layer::BITMAP) ((LayerBitmap*)layer)->removeImageAtFrame(currentFrame);
		if(layer->type == Layer::VECTOR) ((LayerVector*)layer)->removeImageAtFrame(currentFrame);
		timeLine->update();
		scribbleArea->updateFrame();
	}
}

void Editor::addFrame(int frameNumber) {
	frameList << frameNumber;
	qSort(frameList);
	scribbleArea->updateFrame();
}

void Editor::removeFrame(int frameNumber) {
	frameList.removeAt( getLastIndexAtFrame(frameNumber) );
	scribbleArea->updateFrame();
}

int Editor::getLastIndexAtFrame(int frameNumber) {
	int position  = -1;
	int index = -1;
	for(int i=0; i < frameList.size(); i++) {
		if(frameList.at(i) > position && frameList.at(i) <= frameNumber) {
			position = frameList.at(i);
			index = i;
		}
	}
	return index;
}

int Editor::getLastFrameAtFrame(int frameNumber) {
	return frameList.at( getLastIndexAtFrame(frameNumber) );
}



void Editor::config() {
	preferences->show();
}

void Editor::play() {
	updateMaxFrame();
	if(currentFrame > maxFrame) {
		scrubTo(maxFrame);
	} else {
		if(currentFrame == maxFrame) {
			if( !playing ) {
				scrubTo(0);
			} else {
				if( looping ) { scrubTo(0); } else { startOrStop(); }
			}
		} else {
			startOrStop();
		}
	}
}

void Editor::startOrStop() {
	if(!playing) {
		playing = true;
		timer->start();
	} else {
		playing = false;
		timer->stop();
		object->stopSoundIfAny();
	}
}

void Editor::playNextFrame() {
	if(currentFrame < maxFrame) {
		object->playSoundIfAny(currentFrame);
		scrubForward();
	} else {
		play();
	}
}

void Editor::changeFps(int x) {
	fps=x;
	QSettings settings("Pencil","Pencil");
	settings.setValue("fps", x);
	timer->setInterval(1000/fps);
}

int Editor::getFps() {
	return fps;
}

void Editor::setLoop() {
     if (looping) looping=false;
     else looping=true;
}

void Editor::setCurrentLayer(int layerNumber) {
	currentLayer = layerNumber;
	timeLine->update();
	scribbleArea->updateAllFrames();
}

void Editor::switchVisibilityOfLayer(int layerNumber) {
	Layer* layer = object->getLayer(layerNumber);
	if(layer != NULL) layer->switchVisibility();
	scribbleArea->updateAllFrames();
	timeLine->update();
}

void Editor::moveLayer(int i, int j) {
	object->moveLayer(i, j);
	if(j<i) { currentLayer = j; } else { currentLayer = j-1; }
	timeLine->update();
	scribbleArea->updateAllFrames();
}

void Editor::updateMaxFrame() {
	maxFrame = -1;
	for(int i=0; i < object->getLayerCount(); i++) {
		int frameNumber = object->getLayer(i)->getMaxFrame();
		if( frameNumber > maxFrame) maxFrame = frameNumber;
	}
}
