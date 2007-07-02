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
#include "timeline.h"
#include "editor.h"
#include "toolset.h"

TimeLine::TimeLine(QWidget *parent, Editor *editor) : QDockWidget(parent, Qt::Tool) // DockPalette("")
{
	QWidget* timeLineContent = new QWidget(this);
	
	list = new TimeLineCells(this, editor, "layers");
	cells = new TimeLineCells(this, editor, "tracks");
	
	numberOfLayers = 0;
	hScrollBar = new QScrollBar(Qt::Horizontal);
	vScrollBar = new QScrollBar(Qt::Vertical);
	vScrollBar->setMinimum(0);
	vScrollBar->setMaximum(1);
	vScrollBar->setPageStep(1);
	
	QWidget* leftToolBar = new QWidget();
	leftToolBar->setFixedHeight(22);
	QWidget* rightToolBar = new QWidget();
	rightToolBar->setFixedHeight(22);
	
	QWidget* leftWidget = new QWidget();
	leftWidget->setMinimumWidth(100);
	QWidget* rightWidget = new QWidget();
	
	QLabel* layerLabel = new QLabel(tr("Layers:"));
	layerLabel->setFixedWidth(40);
	layerLabel->setIndent(5);
	QToolButton* addLayerButton = new QToolButton(this);
	addLayerButton->setIcon(QIcon(":icons/add.png"));
	addLayerButton->setToolTip("Add Layer");
	addLayerButton->setFixedSize(16,16);
	QToolButton* removeLayerButton = new QToolButton(this);
	removeLayerButton->setIcon(QIcon(":icons/remove.png"));
	removeLayerButton->setToolTip("Remove Layer");
	removeLayerButton->setFixedSize(16,16);
	QHBoxLayout* leftToolBarLayout = new QHBoxLayout();
	leftToolBarLayout->setAlignment(Qt::AlignLeft);
	leftToolBarLayout->addWidget(layerLabel);
	leftToolBarLayout->addWidget(addLayerButton);
	leftToolBarLayout->addWidget(removeLayerButton);
	leftToolBarLayout->setMargin(0);
	leftToolBarLayout->setSpacing(5);
	leftToolBar->setLayout(leftToolBarLayout);
	
	QAction* newBitmapLayerAct = new QAction(tr("New Bitmap Layer"), this);
	QAction* newVectorLayerAct = new QAction(tr("New Vector Layer"), this);
	QAction* newSoundLayerAct = new QAction(tr("New Sound Layer"), this);
	QMenu* layerMenu = new QMenu(tr("&Layer"), this);
	layerMenu->addAction(newBitmapLayerAct);
	layerMenu->addAction(newVectorLayerAct);
	layerMenu->addAction(newSoundLayerAct);
	addLayerButton->setMenu(layerMenu);
	addLayerButton->setPopupMode(QToolButton::InstantPopup);
	
	QGridLayout *leftLayout = new QGridLayout();
	leftLayout->addWidget(leftToolBar,0,0);
	leftLayout->addWidget(list,1,0);
	leftLayout->setMargin(0);
	leftLayout->setSpacing(0);
	leftWidget->setLayout(leftLayout);
	
	QLabel* keyLabel = new QLabel(tr("Keys:"));
	keyLabel->setFixedWidth(40);
	keyLabel->setIndent(5);
	QToolButton* addKeyButton = new QToolButton(this);
	addKeyButton->setIcon(QIcon(":icons/add.png"));
	addKeyButton->setToolTip("Add Key");
	addKeyButton->setFixedSize(16,16);
	QToolButton* removeKeyButton = new QToolButton(this);
	removeKeyButton->setIcon(QIcon(":icons/remove.png"));
	removeKeyButton->setToolTip("Remove Key");
	removeKeyButton->setFixedSize(16,16);
	QHBoxLayout* rightToolBarLayout = new QHBoxLayout();
	rightToolBarLayout->setAlignment(Qt::AlignLeft);
	rightToolBarLayout->addWidget(keyLabel);
	rightToolBarLayout->addWidget(addKeyButton);
	rightToolBarLayout->addWidget(removeKeyButton);
	rightToolBarLayout->setMargin(0);
	rightToolBarLayout->setSpacing(5);
	rightToolBar->setLayout(rightToolBarLayout);
	
	QGridLayout *rightLayout = new QGridLayout();
	rightLayout->addWidget(rightToolBar,0,0);
	rightLayout->addWidget(cells,1,0);
	rightLayout->setMargin(0);
	rightLayout->setSpacing(0);
	rightWidget->setLayout(rightLayout);
	
	
	QSplitter *splitter = new QSplitter(parent);
	splitter->addWidget(leftWidget);
	splitter->addWidget(rightWidget);
	splitter->setSizes( QList<int>() << 100 << 600 );
	//splitter->addWidget(cells);
	
		
	QGridLayout *lay = new QGridLayout();
	//lay->addWidget(cells,0,0);
	//lay->addWidget(toolBar,0,0);
	lay->addWidget(splitter,0,0);
	lay->addWidget(vScrollBar,0,1);
	lay->addWidget(hScrollBar,1,0);
	//lay->addWidget(vScrollBar,1,1);
	lay->setMargin(0);
	lay->setSpacing(0);
	timeLineContent->setLayout(lay);
	//timeLineContent->setBackgroundRole(QPalette::Dark);
	//timeLineContent->setForegroundRole(QPalette::Dark);
	setWidget(timeLineContent);
	
	setWindowFlags(Qt::WindowStaysOnTopHint);
	setWindowTitle("TimeLine");
	//setWindowFlags(Qt::SubWindow);
	setFloating(true);
	//setMinimumSize(100, 300);
	//setGeometry(10,60,100, 300);
	
	connect(this,SIGNAL(lengthChange(QString)), cells, SLOT(lengthChange(QString)));
	connect(this,SIGNAL(fontSizeChange(int)), cells, SLOT(fontSizeChange(int)));
	connect(this,SIGNAL(frameSizeChange(int)), cells, SLOT(frameSizeChange(int)));
	connect(this,SIGNAL(labelChange(int)), cells, SLOT(labelChange(int)));
	
	connect(hScrollBar,SIGNAL(valueChanged(int)), cells, SLOT(hScrollChange(int)));
	connect(vScrollBar,SIGNAL(valueChanged(int)), cells, SLOT(vScrollChange(int)));
	connect(vScrollBar,SIGNAL(valueChanged(int)), list, SLOT(vScrollChange(int)));
	
	connect(addKeyButton, SIGNAL(clicked()), this, SIGNAL(addKeyClick()));
	connect(removeKeyButton, SIGNAL(clicked()), this, SIGNAL(removeKeyClick()));

	connect(newBitmapLayerAct, SIGNAL(triggered()), this, SIGNAL(newBitmapLayer()));
	connect(newVectorLayerAct, SIGNAL(triggered()), this, SIGNAL(newVectorLayer()));
	connect(newSoundLayerAct, SIGNAL(triggered()), this, SIGNAL(newSoundLayer()));
	connect(removeLayerButton, SIGNAL(clicked()), this, SIGNAL(deleteCurrentLayer()));

	//scrubbing = false;
	//QSettings settings("Pencil","Pencil");
	//layerHeight = (settings.value("layerHeight").toInt());
	//if(layerHeight==0) { layerHeight=20; settings.setValue("layerHeight", layerHeight); }
	//startY = 0;
	//endY = 0;
	//startLayerNumber = -1;
	//offsetY = 15;
}


void TimeLine::resizeEvent(QResizeEvent *event) {
	//QWidget::resizeEvent(event);
	updateLayerView();
	//event->accept();
}


void TimeLine::updateLayerView() {
	vScrollBar->setPageStep( (height()-cells->getOffsetY()-hScrollBar->height())/cells->getLayerHeight() -2 );
	vScrollBar->setMinimum( 0 );
	vScrollBar->setMaximum( qMax(0, numberOfLayers - vScrollBar->pageStep()) );
	update();
}

void TimeLine::updateLayerNumber(int numberOfLayers) {
	this->numberOfLayers = numberOfLayers;
	updateLayerView();
}

void TimeLine::updateLength(int frameLength) {
	hScrollBar->setMaximum( frameLength );
}

/*void TimeLine::mousePressEvent(QMouseEvent *event) {
	//int frameNumber = getFrameNumber(event->pos().x());
	int layerNumber = getLayerNumber(event->pos().y());
	startY = event->pos().y();
	startLayerNumber = layerNumber;
	endY = event->pos().y();
	
	//if(frameNumber == editor->currentFrame) {
	//	scrubbing = true;
	//} else {
		if( (layerNumber != -1) && layerNumber < editor->object->getLayerCount()) {
			//editor->object->getLayer(layerNumber)->mousePress(event, frameNumber);
			editor->setCurrentLayer(layerNumber);
		//} else {
			//editor->scrubTo(frameNumber);
			//scrubbing = true;
		}
	//}
}

void TimeLine::mouseMoveEvent(QMouseEvent *event) {
	endY = event->pos().y();
	//int frameNumber = getFrameNumber(event->pos().x());
	int layerNumber = getLayerNumber(event->pos().y());
	//if(scrubbing) {
	//	editor->scrubTo(frameNumber);
	//} else {
		//if(layerNumber != -1 && layerNumber < editor->object->getLayerCount()) {
		//	editor->object->getLayer(layerNumber)->mouseMove(event, frameNumber);
		//}
	//}
	update();
}

void TimeLine::mouseReleaseEvent(QMouseEvent *event) {
	endY = startY;
	//scrubbing = false;
	//int frameNumber = getFrameNumber(event->pos().x());
	int layerNumber = getLayerNumber(event->pos().y());
	//if(layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
	//	editor->object->getLayer(layerNumber)->mouseRelease(event, frameNumber);
	//}
	if(layerNumber != startLayerNumber && startLayerNumber != -1 && layerNumber != -1) {
		editor->moveLayer(startLayerNumber, layerNumber);
	}
	update();
}

void TimeLine::mouseDoubleClickEvent(QMouseEvent *event) {
	int layerNumber = getLayerNumber(event->pos().y());
	if(layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
		editor->switchVisibilityOfLayer(layerNumber);
	}
}*/



















TimeLineCells::TimeLineCells(TimeLine *parent, Editor *editor, QString type) : QWidget(parent)
{
	this->timeLine = parent;
	this->editor = editor;
	this->type = type;
	
	cache = NULL;
	QSettings settings("Pencil","Pencil");
	
	frameLength = settings.value("length").toInt();
	if (frameLength==0) { frameLength=240; settings.setValue("length", frameLength); }
	
	fps = editor->fps;
	
	//playing = false;
	scrubbing = false;
	startY = 0;
	endY = 0;
	startLayerNumber = -1;
	offsetX = 0;
	offsetY = 20;
	frameOffset = 0;
	layerOffset = 0;
	
	frameSize = (settings.value("frameSize").toInt());
	if (frameSize==0) { frameSize=12; settings.setValue("frameSize", frameSize); }
	
	fontSize = (settings.value("labelFontSize").toInt());
	if (fontSize==0) { fontSize=12; settings.setValue("labelFontSize", fontSize); }
	
	layerHeight = (settings.value("layerHeight").toInt());
	if(layerHeight==0) { layerHeight=20; settings.setValue("layerHeight", layerHeight); }
	
	//setMinimumSize(frameLength*frameSize, 3*layerHeight);
	//setMinimumWidth(500);
	setMinimumSize(500, 4*layerHeight);
	setSizePolicy( QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding) );
}

int TimeLineCells::getFrameNumber(int x) {
	int frameNumber = frameOffset+1+(x-offsetX)/frameSize;
	return frameNumber;
}

int TimeLineCells::getFrameX(int frameNumber) {
	int x = offsetX + (frameNumber-frameOffset)*frameSize;
	return x;
}

int TimeLineCells::getLayerNumber(int y) {
	int layerNumber = layerOffset + (y-offsetY)/layerHeight;
	if (y < offsetY) layerNumber = -1;
	if (layerNumber >= editor->object->getLayerCount()) layerNumber = editor->object->getLayerCount();
	return layerNumber;
}

int TimeLineCells::getLayerY(int layerNumber) {
	return offsetY + (layerNumber-layerOffset)*layerHeight;
}

int TimeLineCells::getOffset() {
	return endY-startY;
}

void TimeLineCells::updateContent() {
	drawContent();
	update();
}

void TimeLineCells::drawContent() {
	QPainter painter( cache );
	Object* object = editor->object;
	Layer* layer = object->getLayer(editor->currentLayer);
	if(layer == NULL) return;
		
	// grey background of the view
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::lightGray);
	painter.drawRect(QRect(0,0, width(), height()));
	
	// --- updates the offsetX
	/*painter.setPen(Qt::black);
	painter.setFont(QFont("helvetica", getLayerHeight()/2));
	for(int i=0; i< object->getLayerCount(); i++) {
		Layer* layeri = object->getLayer(i);
		if(layeri != NULL) {
			int rightmost = 20 + painter.boundingRect(QRect(0,0,300,100), Qt::AlignLeft, layeri->name).width() + 5;
			if( rightmost > offsetX) offsetX = rightmost;
		}
	}*/

	// --- draw layers of the current object
	for(int i=0; i< object->getLayerCount(); i++) {
		if(i != editor->currentLayer) {
			Layer* layeri = object->getLayer(i);
			if(layeri != NULL) {
				if(type == "tracks") layeri->paintTrack(painter, this, offsetX, getLayerY(i), width()-offsetX, getLayerHeight(), false, frameSize);
				if(type == "layers") layeri->paintLabel(painter, this, 0, getLayerY(i), width()-1, getLayerHeight(), false, editor->allLayers());
			}
		}
	}
	if(scrubbing == false && abs(getOffset()) > 5) {
		if(type == "tracks") layer->paintTrack(painter, this, offsetX, getLayerY(editor->currentLayer)+getOffset(), width()-offsetX, getLayerHeight(), true, frameSize);
		if(type == "layers") layer->paintLabel(painter, this, 0, getLayerY(editor->currentLayer)+getOffset(), width()-1, getLayerHeight(), true, editor->allLayers());
		painter.setPen( Qt::black );
		painter.drawRect(0, getLayerY( getLayerNumber(endY) ) -1, width(), 2);
	} else {
		if(type == "tracks") layer->paintTrack(painter, this, offsetX, getLayerY(editor->currentLayer), width()-offsetX, getLayerHeight(), true, frameSize);
		if(type == "layers") layer->paintLabel(painter, this, 0, getLayerY(editor->currentLayer), width()-1, getLayerHeight(), true, editor->allLayers());
	}
	// --- draw top
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(220,220,220));
	painter.drawRect(QRect(0,0, width()-1, offsetY-1));
	painter.setPen( Qt::gray );
	painter.drawLine(0,0, width()-1, 0);
	painter.drawLine(0,offsetY-2, width()-1, offsetY-2);
	painter.setPen( Qt::lightGray );
	painter.drawLine(0,offsetY-3, width()-1, offsetY-3);
	painter.drawLine(0,0, 0, offsetY-3);
	
	if(type == "layers") {
		// --- draw circle
		painter.setPen(Qt::black);
		if(editor->allLayers()) { painter.setBrush(Qt::black); } else { painter.setBrush(Qt::darkGray); }
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.drawEllipse(6, 4, 9, 9);
		painter.setRenderHint(QPainter::Antialiasing, false);
	}
	
	if(type == "tracks") {
		// --- draw ticks
		painter.setPen( QColor(70,70,70,255) );
		painter.setBrush(Qt::darkGray);
		painter.setFont(QFont("helvetica", 10));
		int incr = 0;
		for(int i=frameOffset; i<frameOffset+(width()-offsetX)/frameSize; i++) {
			if(i < 10) { incr = 4; } else { incr = 0; }
			if (i%fps==0) painter.drawLine( getFrameX(i), 1, getFrameX(i), 5 );
			else if (i%fps==fps/2) painter.drawLine( getFrameX(i), 1, getFrameX(i), 5);
			else painter.drawLine( getFrameX(i), 1, getFrameX(i), 3);
			if(i==0 || i%fps==fps-1) painter.drawText(QPoint(getFrameX(i)+incr, 15), QString::number(i+1));
		}
		// --- draw the cached images ---
		painter.setPen( Qt::red );
		QList<int> frameList = editor->frameList;
		for(int i=0; i<frameList.size(); i++) {
			//painter.setBrush( Qt::red );
			int j = frameList.at(i);
			if( j > frameOffset ) {
				painter.drawLine( getFrameX(j-1-frameOffset), 1, getFrameX(j-frameOffset), 1);
			}
		}
	}
}
	
void TimeLineCells::paintEvent(QPaintEvent *event) {
	Object* object = editor->object;
	Layer* layer = object->getLayer(editor->currentLayer);
	if(layer == NULL) return;
	
	QPainter painter( this );
	if(!editor->playing) drawContent();
	if(cache) painter.drawPixmap(QPoint(0,0), *cache); 
	
	if(type == "tracks") {
		// --- draw the position of the current frame
		if(editor->currentFrame > frameOffset) {
			painter.setBrush(QColor(255,0,0,128));
			painter.setPen(Qt::NoPen);
			painter.setFont(QFont("helvetica", 10));
			painter.setCompositionMode(QPainter::CompositionMode_Source);
			QRect scrubRect;
			scrubRect.setTopLeft(QPoint( getFrameX(editor->currentFrame-1), 0));
			scrubRect.setBottomRight(QPoint( getFrameX(editor->currentFrame), height()));
			painter.drawRect(scrubRect);
			painter.setPen( QColor(70,70,70,255) );
			int incr = 0;
			if(editor->currentFrame < 10) { incr = 4; } else { incr = 0; }
			painter.drawText(QPoint(getFrameX(editor->currentFrame-1)+incr, 15), QString::number(editor->currentFrame));
		}
	}
	event->accept();
}

void TimeLineCells::resizeEvent(QResizeEvent *event) {
	//QWidget::resizeEvent(event);
	if(cache) delete cache;
	cache = new QPixmap(size());
	event->accept();
}

void TimeLineCells::mousePressEvent(QMouseEvent *event) {
	int frameNumber = getFrameNumber(event->pos().x());
	int layerNumber = getLayerNumber(event->pos().y());
	startY = event->pos().y();
	startLayerNumber = layerNumber;
	endY = event->pos().y();
	
	if(type == "layers") {
		if(layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
			if(event->pos().x() < 15) {
				editor->switchVisibilityOfLayer(layerNumber);
			} else {
				editor->setCurrentLayer(layerNumber);
				updateContent();
			}
		}
		if(layerNumber == -1) {
			if(event->pos().x() < 15) {
				editor->toggleShowAllLayers();
			}
		}
	}
	
	if(type == "tracks") {
		if(frameNumber == editor->currentFrame) {
			scrubbing = true;
		} else {
			if( (layerNumber != -1) && layerNumber < editor->object->getLayerCount()) {
				editor->object->getLayer(layerNumber)->mousePress(event, frameNumber);
				//if(event->pos().x() > 15) editor->setCurrentLayer(layerNumber);
				editor->setCurrentLayer(layerNumber);
				updateContent();
			} else {
				if(frameNumber > 0) {
					editor->scrubTo(frameNumber);
					scrubbing = true;
				}
			}
		}
	}
}


void TimeLineCells::mouseMoveEvent(QMouseEvent *event) {
	//if(event->pos().x() < offsetX) endY = event->pos().y();
	if(type == "layers") endY = event->pos().y();
	int frameNumber = getFrameNumber(event->pos().x());
	int layerNumber = getLayerNumber(event->pos().y());
	if(scrubbing) {
		editor->scrubTo(frameNumber);
	} else {
		if(layerNumber != -1 && layerNumber < editor->object->getLayerCount()) {
			editor->object->getLayer(layerNumber)->mouseMove(event, frameNumber);
		}
	}
	updateContent();
}

void TimeLineCells::mouseReleaseEvent(QMouseEvent *event) {
	endY = startY;
	scrubbing = false;
	int frameNumber = getFrameNumber(event->pos().x());
	if(frameNumber < 1) frameNumber = -1;
	int layerNumber = getLayerNumber(event->pos().y());
	if(type == "tracks" && layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
		editor->object->getLayer(layerNumber)->mouseRelease(event, frameNumber);
	}
	//if(event->pos().x() < offsetX && layerNumber != startLayerNumber && startLayerNumber != -1 && layerNumber != -1) {
	if(type == "layers" && layerNumber != startLayerNumber && startLayerNumber != -1 && layerNumber != -1) {
		editor->moveLayer(startLayerNumber, layerNumber);
	}
	updateContent();
}

void TimeLineCells::mouseDoubleClickEvent(QMouseEvent *event) {
	int frameNumber = getFrameNumber(event->pos().x());
	int layerNumber = getLayerNumber(event->pos().y());
	
	if(type == "tracks" && (layerNumber != -1) && (frameNumber > 0) && layerNumber < editor->object->getLayerCount()) {
		editor->object->getLayer(layerNumber)->mouseDoubleClick(event, frameNumber);
	}
	/*int layerNumber = getLayerNumber(event->pos().y());
	if(layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
		editor->switchVisibilityOfLayer(layerNumber);
		event->accept();
	}*/
}

// --- changes ---

void TimeLineCells::fontSizeChange(int x) {
	fontSize=x;
	QSettings settings("Pencil","Pencil");
	settings.setValue("labelFontSize", x);
	updateContent();
}

void TimeLineCells::frameSizeChange(int x) {
	//int old;
	//old=frameSize;
	frameSize = x;
	QSettings settings("Pencil","Pencil");
	settings.setValue("frameSize", x);
	/*int i;
	for(i=0;i<frame.size();i++) {
		frame[i].moveTopLeft(QPoint((frame[i].x()/old)*frameSize,0));
		frame[i].setWidth(frameSize);
	}*/
	updateContent();
}

void TimeLineCells::labelChange(int x) {
	QSettings settings("Pencil","Pencil");
	if (x==0) { drawFrameNumber=false; settings.setValue("drawLabel","false"); }
	else { drawFrameNumber=true; settings.setValue("drawLabel","true"); }
	updateContent();
}

void TimeLineCells::lengthChange(QString x) {
	bool ok;
	int dec = x.toInt(&ok, 10);   
	frameLength=dec;
	timeLine->updateLength(frameLength);
	//setMinimumSize(dec*frameSize,40);
	//setFixedWidth(dec*frameSize);
	updateContent();
	QSettings settings("Pencil","Pencil");
	settings.setValue("length", dec);
}

void TimeLineCells::hScrollChange(int x) {
	frameOffset = x;
	updateContent();
}

void TimeLineCells::vScrollChange(int x) {
	layerOffset = x;
	updateContent();
}
