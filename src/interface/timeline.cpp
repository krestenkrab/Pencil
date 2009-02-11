/*

Pencil - Traditional Animation Software
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
#include "timeline.h"
#include "editor.h"
#include "toolset.h"
#include "timecontrols.h"

#include "layer.h"
#include "layerimage.h"

TimeLine::TimeLine(QWidget *parent, Editor *editor) : QDockWidget(parent, Qt::Tool) // DockPalette("")
{
	QWidget* timeLineContent = new QWidget(this);

	list = new TimeLineCells(this, editor, "layers");
	cells = new TimeLineCells(this, editor, "tracks");
	connect(list, SIGNAL(mouseMovedY(int)), list, SLOT(setMouseMoveY(int)));
	connect(list, SIGNAL(mouseMovedY(int)), cells, SLOT(setMouseMoveY(int)));

	numberOfLayers = 0;
	hScrollBar = new QScrollBar(Qt::Horizontal);
	vScrollBar = new QScrollBar(Qt::Vertical);
	vScrollBar->setMinimum(0);
	vScrollBar->setMaximum(1);
	vScrollBar->setPageStep(1);

	QWidget* leftWidget = new QWidget();
	leftWidget->setMinimumWidth(120);
	QWidget* rightWidget = new QWidget();

	QWidget* leftToolBar = new QWidget();
	leftToolBar->setFixedHeight(31);
	QWidget* rightToolBar = new QWidget();
	rightToolBar->setFixedHeight(31);
	
	// --- left widget ---
	// --------- layer buttons ---------
	//QFrame* layerButtons = new QFrame(this);
	QToolBar* layerButtons = new QToolBar(this);
	//QHBoxLayout* layerButtonLayout = new QHBoxLayout();
		QLabel* layerLabel = new QLabel(tr("Layers:"));
		layerLabel->setIndent(5);
		layerLabel->setFont( QFont("Helvetica", 10) );
		QToolButton* addLayerButton = new QToolButton(this);
		addLayerButton->setIcon(QIcon(":icons/add.png"));
		addLayerButton->setToolTip("Add Layer");
		addLayerButton->setFixedSize(24,24);
		QToolButton* removeLayerButton = new QToolButton(this);
		removeLayerButton->setIcon(QIcon(":icons/remove.png"));
		removeLayerButton->setToolTip("Remove Layer");
		removeLayerButton->setFixedSize(24,24);
		/*layerButtonLayout->addWidget(layerLabel);
		layerButtonLayout->addWidget(addLayerButton);
		layerButtonLayout->addWidget(removeLayerButton);
		layerButtonLayout->setMargin(0);
		layerButtonLayout->setSpacing(5);
	layerButtons->setLayout(layerButtonLayout);*/
	layerButtons->addWidget(layerLabel);
	layerButtons->addWidget(addLayerButton);
	layerButtons->addWidget(removeLayerButton);

	QHBoxLayout* leftToolBarLayout = new QHBoxLayout();
	leftToolBarLayout->setAlignment(Qt::AlignLeft);
	leftToolBarLayout->setMargin(0);
	leftToolBarLayout->addWidget(layerButtons);
	leftToolBar->setLayout(leftToolBarLayout);

	QAction* newBitmapLayerAct = new QAction(QIcon(":icons/layer-bitmap.png"), tr("New Bitmap Layer"), this);
	QAction* newVectorLayerAct = new QAction(QIcon(":icons/layer-vector.png"), tr("New Vector Layer"), this);
	QAction* newSoundLayerAct = new QAction(QIcon(":icons/layer-sound.png"), tr("New Sound Layer"), this);
	QAction* newCameraLayerAct = new QAction(QIcon(":icons/layer-camera.png"), tr("New Camera Layer"), this);
	QMenu* layerMenu = new QMenu(tr("&Layer"), this);
	layerMenu->addAction(newBitmapLayerAct);
	layerMenu->addAction(newVectorLayerAct);
	layerMenu->addAction(newSoundLayerAct);
	layerMenu->addAction(newCameraLayerAct);
	addLayerButton->setMenu(layerMenu);
	addLayerButton->setPopupMode(QToolButton::InstantPopup);

	QGridLayout *leftLayout = new QGridLayout();
	leftLayout->addWidget(leftToolBar,0,0);
	leftLayout->addWidget(list,1,0);
	leftLayout->setMargin(0);
	leftLayout->setSpacing(0);
	leftWidget->setLayout(leftLayout);

	// --- right widget ---
	// --------- key buttons ---------
	//QFrame* keyButtons = new QFrame(this);
	QToolBar* keyButtons = new QToolBar(this);
	//keyButtons->setFixedWidth(90);
	//QHBoxLayout* keyButtonsLayout = new QHBoxLayout();
		QLabel* keyLabel = new QLabel(tr("Keys:"));
		keyLabel->setFont( QFont("Helvetica", 10) );
		keyLabel->setIndent(5);
		QToolButton* addKeyButton = new QToolButton(this);
		addKeyButton->setIcon(QIcon(":icons/add.png"));
		addKeyButton->setToolTip("Add Key");
		addKeyButton->setFixedSize(24,24);
		QToolButton* removeKeyButton = new QToolButton(this);
		removeKeyButton->setIcon(QIcon(":icons/remove.png"));
		removeKeyButton->setToolTip("Remove Key");
		removeKeyButton->setFixedSize(24,24);
		/*keyButtonsLayout->addWidget(keyLabel);
		keyButtonsLayout->addWidget(addKeyButton);
		keyButtonsLayout->addWidget(removeKeyButton);
		keyButtonsLayout->setMargin(0);
		keyButtonsLayout->setSpacing(0);
		//keyButtonLayout->setSizeConstraint(QLayout::SetMinimumSize);
	keyButtons->setLayout(keyButtonsLayout);*/
	keyButtons->addWidget(keyLabel);
	keyButtons->addWidget(addKeyButton);
	keyButtons->addWidget(removeKeyButton);
	
	// --------- Onion skin buttons ---------
	//QFrame* onionButtons = new QFrame(this);
	QToolBar* onionButtons = new QToolBar(this);
	//onionButtons->setFixedWidth(90);
	//QHBoxLayout* onionButtonsLayout = new QHBoxLayout();
		QLabel* onionLabel = new QLabel(tr("Onion skin: "));
		onionLabel->setFont( QFont("Helvetica", 10) );
		onionLabel->setIndent(5);
		QToolButton* onionPrevButton = new QToolButton(this);
		onionPrevButton->setIcon(QIcon(":icons/onionPrev.png"));
		onionPrevButton->setToolTip("Show previous frame");
		onionPrevButton->setFixedSize(29,29);
		onionPrevButton->setCheckable(true);
		onionPrevButton->setChecked(true);
		QToolButton* onionNextButton = new QToolButton(this);
		onionNextButton->setIcon(QIcon(":icons/onionNext.png"));
		onionNextButton->setToolTip("Show next frame");
		onionNextButton->setFixedSize(29,29);
		onionNextButton->setCheckable(true);
		/*onionButtonsLayout->addWidget(onionLabel);
		onionButtonsLayout->addWidget(onionPrevButton);
		onionButtonsLayout->addWidget(onionNextButton);
		onionButtonsLayout->setMargin(0);
		onionButtonsLayout->setSpacing(0);
		//keyButtonLayout->setSizeConstraint(QLayout::SetMinimumSize);
	onionButtons->setLayout(onionButtonsLayout);*/
	onionButtons->addSeparator();
	onionButtons->addWidget(onionLabel);
	onionButtons->addWidget(onionPrevButton);
	onionButtons->addWidget(onionNextButton);

	
	// --------- Time controls ---------
	TimeControls* timeControls = new TimeControls(this);

	
	QHBoxLayout* rightToolBarLayout = new QHBoxLayout();
	//rightToolBarLayout->setAlignment(Qt::AlignLeft);
	rightToolBarLayout->addWidget(keyButtons);
	rightToolBarLayout->addWidget(onionButtons);
	rightToolBarLayout->addStretch(1);
	rightToolBarLayout->addWidget(timeControls);
	rightToolBarLayout->setMargin(0);
	rightToolBarLayout->setSpacing(0);
	rightToolBar->setLayout(rightToolBarLayout);

	QGridLayout *rightLayout = new QGridLayout();
	rightLayout->addWidget(rightToolBar,0,0);
	rightLayout->addWidget(cells,1,0);
	rightLayout->setMargin(0);
	rightLayout->setSpacing(0);
	rightWidget->setLayout(rightLayout);

	// --- Splitter ---
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

	#ifndef Q_WS_MAC
		setStyleSheet ("QToolBar { border: 0px none black; }");
	#endif

	setWindowFlags(Qt::WindowStaysOnTopHint);
	setWindowTitle("Time Line");
	//setWindowFlags(Qt::SubWindow);
	setFloating(true);
	//setMinimumSize(100, 300);
	//setGeometry(10,60,100, 300);

	connect(this,SIGNAL(lengthChange(QString)), cells, SLOT(lengthChange(QString)));
	connect(this,SIGNAL(fontSizeChange(int)), cells, SLOT(fontSizeChange(int)));
	connect(this,SIGNAL(frameSizeChange(int)), cells, SLOT(frameSizeChange(int)));
	connect(this,SIGNAL(labelChange(int)), cells, SLOT(labelChange(int)));
	connect(this,SIGNAL(scrubChange(int)), cells, SLOT(scrubChange(int)));

	connect(hScrollBar,SIGNAL(valueChanged(int)), cells, SLOT(hScrollChange(int)));
	connect(vScrollBar,SIGNAL(valueChanged(int)), cells, SLOT(vScrollChange(int)));
	connect(vScrollBar,SIGNAL(valueChanged(int)), list, SLOT(vScrollChange(int)));

	connect(addKeyButton, SIGNAL(clicked()), this, SIGNAL(addKeyClick()));
	connect(removeKeyButton, SIGNAL(clicked()), this, SIGNAL(removeKeyClick()));

	connect(onionPrevButton, SIGNAL(clicked()), this, SIGNAL(onionPrevClick()));
	connect(onionNextButton, SIGNAL(clicked()), this, SIGNAL(onionNextClick()));

	connect(timeControls, SIGNAL(playClick()), this, SIGNAL(playClick()));
	connect(timeControls, SIGNAL(loopClick()), this, SIGNAL(loopClick()));
	connect(timeControls, SIGNAL(soundClick()), this, SIGNAL(soundClick()));
	connect(timeControls, SIGNAL(fpsClick(int)), this, SIGNAL(fpsClick(int)));
	//connect(this, SIGNAL(topLevelChanged(bool)), timeControls, SLOT(updateButtons(bool))); // when the windows is docked or made floatable

	connect(newBitmapLayerAct, SIGNAL(triggered()), this, SIGNAL(newBitmapLayer()));
	connect(newVectorLayerAct, SIGNAL(triggered()), this, SIGNAL(newVectorLayer()));
	connect(newSoundLayerAct, SIGNAL(triggered()), this, SIGNAL(newSoundLayer()));
	connect(newCameraLayerAct, SIGNAL(triggered()), this, SIGNAL(newCameraLayer()));
	connect(removeLayerButton, SIGNAL(clicked()), this, SIGNAL(deleteCurrentLayer()));

	scrubbing = false;
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

void TimeLine::updateFrame(int frameNumber) {
	if(cells) cells->updateFrame(frameNumber);
}

void TimeLine::updateLayerView() {
	vScrollBar->setPageStep( (height()-cells->getOffsetY()-hScrollBar->height())/cells->getLayerHeight() -2 );
	vScrollBar->setMinimum( 0 );
	vScrollBar->setMaximum( qMax(0, numberOfLayers - vScrollBar->pageStep()) );
	update();
	updateContent();
}

void TimeLine::updateLayerNumber(int numberOfLayers) {
	this->numberOfLayers = numberOfLayers;
	updateLayerView();
}

void TimeLine::updateLength(int frameLength) {
	hScrollBar->setMaximum( frameLength );
}

void TimeLine::updateContent() {
	list->updateContent();
	cells->updateContent();
	update();
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

	shortScrub = settings.value("shortScrub").toBool();
	
	fps = editor->fps;

	//playing = false;
	//scrubbing = false;
	startY = 0;
	endY = 0;
	mouseMoveY = 0;
	startLayerNumber = -1;
	offsetX = 0;
	offsetY = 20;
	frameOffset = 0;
	selectionOffset = 0;
	layerOffset = 0;
	frameClicked = -1;

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
	setAttribute(Qt::WA_OpaquePaintEvent, false);
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

void TimeLineCells::updateFrame(int frameNumber) {
	int x = getFrameX(frameNumber);
	update(x-frameSize,0,frameSize+1,height());
}

void TimeLineCells::updateContent() {
	drawContent();
	update();
}

void TimeLineCells::drawContent() {
	//qDebug() << "draw content" << QDateTime::currentDateTime() << timeLine->scrubbing;
	if(cache == NULL) { cache = new QPixmap(size()); }
	if(cache->isNull()) return;
	QPainter painter( cache );
	Object* object = editor->object;
        if(object == NULL) return;
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
				if(type == "tracks") paintTrack(painter, layeri, offsetX, getLayerY(i), width()-offsetX, getLayerHeight(), false, frameSize);
				if(type == "layers") paintLabel(painter, layeri, 0, getLayerY(i), width()-1, getLayerHeight(), false, editor->allLayers());
			}
		}
	}
	//if(timeLine->scrubbing == false && abs(getMouseMoveY()) > 5) {
	if( abs(getMouseMoveY()) > 5 ) {
		if(type == "tracks") paintTrack(painter, layer, offsetX, getLayerY(editor->currentLayer)+getMouseMoveY(), width()-offsetX, getLayerHeight(), true, frameSize);
		if(type == "layers") paintLabel(painter, layer, 0, getLayerY(editor->currentLayer)+getMouseMoveY(), width()-1, getLayerHeight(), true, editor->allLayers());
		painter.setPen( Qt::black );
		painter.drawRect(0, getLayerY( getLayerNumber(endY) ) -1, width(), 2);
	} else {
		if(type == "tracks") paintTrack(painter, layer, offsetX, getLayerY(editor->currentLayer), width()-offsetX, getLayerHeight(), true, frameSize);
		if(type == "layers") paintLabel(painter, layer, 0, getLayerY(editor->currentLayer), width()-1, getLayerHeight(), true, editor->allLayers());
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
		if(editor->allLayers() == 0) { painter.setBrush(Qt::NoBrush); }
		if(editor->allLayers() == 1) { painter.setBrush(Qt::darkGray); }
		if(editor->allLayers() == 2) { painter.setBrush(Qt::black); }
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.drawEllipse(6, 4, 9, 9);
		painter.setRenderHint(QPainter::Antialiasing, false);
		// --- draw right border line
		//painter.setPen( Qt::lightGray );
		//painter.drawLine(width()/2-1,0, width()/2-1, height());
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
		// --- indicates the cached images by a line ---
		/*painter.setPen( Qt::red );
		QList<int> frameList = editor->frameList;
		for(int i=0; i<frameList.size(); i++) {
			//painter.setBrush( Qt::red );
			int j = frameList.at(i);
			if( j > frameOffset ) {
				painter.drawLine( getFrameX(j-1-frameOffset), 1, getFrameX(j-frameOffset), 1);
			}
		}*/
		// --- draw left border line
		painter.setPen( Qt::darkGray );
		painter.drawLine(0,0, 0, height());
	}
}




void TimeLineCells::paintLabel(QPainter &painter, Layer *layer, int x, int y, int width, int height, bool selected, int allLayers) {
	painter.setBrush(Qt::lightGray);
	painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
	painter.drawRect(x, y-1, width, height); // empty rectangle  by default
	
	if(layer->visible) {
		if(allLayers==0)  painter.setBrush(Qt::NoBrush);
		if(allLayers==1)   painter.setBrush(Qt::darkGray);
		if((allLayers==2) || selected)  painter.setBrush(Qt::black);
	} else {
		painter.setBrush(Qt::NoBrush);
	}
	painter.setPen(Qt::black);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.drawEllipse(x+6, y+4, 9, 9);
	painter.setRenderHint(QPainter::Antialiasing, false);
	/*if(selected) {
		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(0,0,0,80));
		painter.drawRect(x, y-1, width, height);
	}*/
	if(selected) {
		paintSelection(painter, x, y, width, height);
	}
	
	if(layer->type == Layer::BITMAP) painter.drawPixmap( QPoint(20, y+2), QPixmap(":/icons/layer-bitmap.png") );
	if(layer->type == Layer::VECTOR) painter.drawPixmap( QPoint(20, y+2), QPixmap(":/icons/layer-vector.png") );
	if(layer->type == Layer::SOUND) painter.drawPixmap( QPoint(21, y+2), QPixmap(":/icons/layer-sound.png") );
	if(layer->type == Layer::CAMERA) painter.drawPixmap( QPoint(21, y+2), QPixmap(":/icons/layer-camera.png") );
	
	painter.setFont(QFont("helvetica", height/2));
	painter.setPen(Qt::black);
	painter.drawText(QPoint(45, y+(2*height)/3), layer->name);

}


void TimeLineCells::paintTrack(QPainter &painter, Layer *layer, int x, int y, int width, int height, bool selected, int frameSize) {
	LayerImage* layerImage = static_cast<LayerImage*>(layer);
	//if(layer->type == Layer::BITMAP || layer->type == Layer::VECTOR || layer->type == Layer::CAMERA || layer->type == Layer::SOUND) // should do a static cast
	if(layerImage)
	{
		painter.setFont(QFont("helvetica", height/2));
		if(layer->visible) {
			QColor col;
			if(layer->type == Layer::BITMAP) col = QColor(130,130,245);
			if(layer->type == Layer::VECTOR) col = QColor(100,205,150);
			if(layer->type == Layer::SOUND) col = QColor(245,130,130);
			if(layer->type == Layer::CAMERA) col = QColor(100,128,140);
			if(!selected) col = QColor( (1*col.red() + 2*200)/3, (1*col.green()+2*200)/3, (1*col.blue()+2*200)/3 );
			painter.setBrush( col );
			painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
			painter.drawRect(x, y-1, width, height);
			//painter.setFont(QFont("helvetica", height/2));
			paintImages(painter, layerImage, x, y, width, height, selected, frameSize);
			//painter.drawText(QPoint(10, y+(2*height)/3), name);
			
			// changes the apparence if selected
			if(selected) {
				paintSelection(painter, x, y, width, height);
			}
		} else {
			painter.setBrush(Qt::gray);
			painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
			painter.drawRect(x, y-1, width, height); // empty rectangle  by default
		}
	}
	if(false) {
		painter.setBrush(Qt::lightGray);
		painter.setPen(QPen(QBrush(QColor(100,100,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
		painter.drawRect(x, y, width, height); // empty rectangle  by default
		if(selected) {
			QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
			linearGrad.setColorAt(0, QColor(255,255,255,128) );
			linearGrad.setColorAt(0.40, QColor(255,255,255,0) );
			linearGrad.setColorAt(0.60, QColor(0,0,0,0) );
			linearGrad.setColorAt(1, QColor(0,0,0,64) );
			painter.setBrush( linearGrad );
			painter.setPen(QPen(QBrush(QColor(70,70,70)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
			painter.drawRect(x, y-1, width, height);
		}
	}
}

void TimeLineCells::paintImages(QPainter &painter, LayerImage *layer, int x, int y, int width, int height, bool selected, int frameSize) {
	if(layer->type == Layer::BITMAP || layer->type == Layer::VECTOR || layer->type == Layer::CAMERA ) \
	{
		painter.setPen(QPen(QBrush(QColor(40,40,40)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
		if(layer->visible) {
			for(int i=0; i < layer->framesPosition.size(); i++) {
				if(layer->framesSelected.at(i)) {
					painter.setBrush(QColor(60,60,60));
					//painter.drawRect(x+(framesPosition.at(i)+frameOffset-1)*frameSize+2, y+1, frameSize-2, height-4);
					painter.drawRect( getFrameX(layer->framesPosition.at(i)+selectionOffset)-frameSize+2, y+1, frameSize-2, height-4);
				}
				else {
					if(selected)
						painter.setBrush(QColor(125,125,125));
					else
						painter.setBrush(QColor(125,125,125,125));
						if(layer->framesModified.at(i)) painter.setBrush(QColor(255,125,125,125));
						painter.drawRect( getFrameX(layer->framesPosition.at(i))-frameSize+2, y+1, frameSize-2, height-4 );
						//painter.drawRect(x+(framesPosition.at(i)-1)*frameSize+2, y+1, frameSize-2, height-4);
					//painter.drawText(QPoint( (framesPosition.at(i)-1)*frameSize+5, y+(2*height)/3), QString::number(i) );
				}
			}
		}
	}
	if(layer->type == Layer::SOUND)
	{
		for(int i=0; i < layer->framesSelected.size(); i++) {
			qreal h = x + (layer->framesPosition.at(i)-1)*frameSize+2;
			if(layer->framesSelected.at(i)) {
				painter.setBrush(QColor(60,60,60));
				h = h + frameOffset*frameSize;
				//	painter.drawRect((framesPosition.at(i)+frameOffset-1)*frameSize+2, verticalPosition+1, frameSize-2, layerHeight-4);
			}
			else {
			//	if(framesModified.at(i))
			//		painter.setBrush(QColor(255,125,125));
			//	else
			//painter.setPen(QPen(QBrush(QColor(40,40,40)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
				painter.setBrush(QColor(125,125,125));
			}
			QPointF points[3] = { QPointF(h, y+4), QPointF(h, y+height-4), QPointF(h+15, y+0.5*height) };
			painter.drawPolygon( points, 3 );
			//painter.drawRect((startingFrame.at(i)-1)*frameSize+2, verticalPosition+1, frameSize-2, layerHeight-4);
			painter.drawText(QPoint( h + 20, y+(2*height)/3), layer->framesFilename.at(i) );
			//}
		}
	}
}

void TimeLineCells::paintSelection(QPainter &painter, int x, int y, int width, int height) {
			QLinearGradient linearGrad(QPointF(0, y), QPointF(0, y + height));
			QSettings settings("Pencil","Pencil");
			QString style = settings.value("style").toString();
			if(style == "aqua") {
				linearGrad.setColorAt(0, QColor(225,225,255,100) );
				linearGrad.setColorAt(0.10, QColor(225,225,255,80) );
				linearGrad.setColorAt(0.20, QColor(225,225,255,64) );
				linearGrad.setColorAt(0.35, QColor(225,225,255,20) );
				linearGrad.setColorAt(0.351, QColor(0,0,0,32) );
				linearGrad.setColorAt(0.66, QColor(245,255,235,32) );
				linearGrad.setColorAt(1, QColor(245,255,235,128) );
			} else {
				linearGrad.setColorAt(0, QColor(255,255,255,128) );
				linearGrad.setColorAt(0.49, QColor(255,255,255,0) );
				linearGrad.setColorAt(0.50, QColor(0,0,0,0) );
				linearGrad.setColorAt(1, QColor(0,0,0,48) );
				
				/*linearGrad.setColorAt(0, QColor(255,255,255,128) );
				linearGrad.setColorAt(0.10, QColor(255,255,255,64) );
				linearGrad.setColorAt(0.49, QColor(0,0,0,32) );
				linearGrad.setColorAt(0.50, QColor(0,0,0,32) );
				linearGrad.setColorAt(0.70, QColor(245,255,245,32) );
				linearGrad.setColorAt(1, QColor(245,255,245,128) );*/
				
				/*linearGrad.setColorAt(0, QColor(255,255,255,128) );
				linearGrad.setColorAt(0.10, QColor(255,255,255,64) );
				linearGrad.setColorAt(0.20, QColor(0,0,0,32) );
				linearGrad.setColorAt(0.40, QColor(0,0,0,0) );
				linearGrad.setColorAt(0.41, QColor(255,255,255,0) );
				linearGrad.setColorAt(1, QColor(255,255,255,128) );*/
			}
			painter.setBrush( linearGrad );
			painter.setPen( Qt::NoPen );
			painter.drawRect(x, y, width, height-1);
			//painter.setBrush( Qt::NoBrush );
			//painter.setPen(QPen(QBrush(QColor(0,0,0,100)), 1, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin));
			//painter.drawRect(x, y-1, width, height);
			/*if(style == "aqua") {
				QColor col;
				if(type == BITMAP) col = QColor(65,65,122);
				if(type == VECTOR) col = QColor(50,102,75);
				if(type == SOUND) col = QColor(122,65,65);
				painter.setPen(col);
				painter.drawLine(x,y-1, x+width, y-1);
			}*/
}




void TimeLineCells::mousePress(QMouseEvent *event, int frameNumber, Layer* layer) {
	LayerImage* layerImage = static_cast<LayerImage*>(layer);
	frameClicked = frameNumber;
	int index = layerImage->getIndexAtFrame(frameNumber);
	if(index == -1) {
		layerImage->deselectAllFrames();
	} else {
		if( (event->modifiers() != Qt::ShiftModifier) && (!layerImage->framesSelected.at(index)) && (event->buttons() != Qt::RightButton) ) {
			layerImage->deselectAllFrames();
		}
		layerImage->framesSelected[index] = true;
	}
	if(event->modifiers() == Qt::AltModifier) {
		for(int i=index; i < layerImage->framesPosition.size(); i++) {
			layerImage->framesSelected[i] = true;
		}
	}
}

void TimeLineCells::mouseDoubleClick(QMouseEvent *event, int frameNumber, Layer* layer) {
	LayerImage* layerImage = static_cast<LayerImage*>(layer);
	int index = layerImage->getIndexAtFrame(frameNumber);
	if(index != -1) {
		for(int i=index; i < layerImage->framesPosition.size(); i++) {
			layerImage->framesSelected[i] = true;
		}
	}
}


void TimeLineCells::mouseMove(QMouseEvent *event, int frameNumber, Layer* layer) {
	LayerImage* layerImage = static_cast<LayerImage*>(layer);
	selectionOffset = frameNumber - frameClicked;
	bool ok = true;
	for(int i=0; i < layerImage->framesPosition.size(); i++) {
		if(layerImage->framesSelected.at(i)) {
			if(layerImage->framesPosition.at(i) + selectionOffset < 1) ok = false;
			for(int j=0; j < layerImage->framesPosition.size(); j++) {
				if(!layerImage->framesSelected.at(j)) {
					if(layerImage->framesPosition.at(i) + selectionOffset == layerImage->framesPosition.at(j)) {
						ok = false;
					}
				}
			}
		}
	}
	if(ok == false) selectionOffset = 0;
}

void TimeLineCells::mouseRelease(QMouseEvent *event, int frameNumber, Layer* layer) {
	LayerImage* layerImage = static_cast<LayerImage*>(layer);
	layerImage->moveSelectedFrames(selectionOffset);
	selectionOffset = 0;
}




void TimeLineCells::paintEvent(QPaintEvent *event) {
	Object* object = editor->object;
	if(object == NULL) return;
	Layer* layer = object->getLayer(editor->currentLayer);
	if(layer == NULL) return;

	QPainter painter( this );
	if( (!editor->playing && !timeLine->scrubbing) || cache == NULL) drawContent();
	if(cache) painter.drawPixmap(QPoint(0,0), *cache);

	if(type == "tracks") {
		// --- draw the position of the current frame
		if(editor->currentFrame > frameOffset) {
			painter.setBrush(QColor(255,0,0,128));
			painter.setPen(Qt::NoPen);
			painter.setFont(QFont("helvetica", 10));
			//painter.setCompositionMode(QPainter::CompositionMode_Source); // this causes the message: QPainter::setCompositionMode: PorterDuff modes not supported on device
			QRect scrubRect;
			scrubRect.setTopLeft(QPoint( getFrameX(editor->currentFrame-1), 0));
			scrubRect.setBottomRight(QPoint( getFrameX(editor->currentFrame), height()));
			if(shortScrub) scrubRect.setBottomRight(QPoint( getFrameX(editor->currentFrame), 19));
			painter.drawRect(scrubRect);
			painter.setPen( QColor(70,70,70,255) );
			int incr = 0;
			if(editor->currentFrame < 10) { incr = 4; } else { incr = 0; }
			painter.drawText(QPoint(getFrameX(editor->currentFrame-1)+incr, 15), QString::number(editor->currentFrame));
		}
	}
	//event->accept();
}

void TimeLineCells::resizeEvent(QResizeEvent *event) {
	//QWidget::resizeEvent(event);
	if(cache) delete cache;
	cache = new QPixmap(size());
	updateContent();
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
				update();
			}
		}
		if(layerNumber == -1) {
			if(event->pos().x() < 15) {
				editor->toggleShowAllLayers();
			}
		}
	}

	if(type == "tracks") {
		if(frameNumber == editor->currentFrame && (!shortScrub || (shortScrub && startY < 20)) ) {
			timeLine->scrubbing = true;
		} else {
			if( (layerNumber != -1) && layerNumber < editor->object->getLayerCount()) {
				mousePress(event, frameNumber, editor->object->getLayer(layerNumber));
				//if(event->pos().x() > 15) editor->setCurrentLayer(layerNumber);
				editor->setCurrentLayer(layerNumber);
				update();
			} else {
				if(frameNumber > 0) {
					editor->scrubTo(frameNumber);
					timeLine->scrubbing = true;
				}
			}
		}
	}
}


void TimeLineCells::mouseMoveEvent(QMouseEvent *event) {
	//if(event->pos().x() < offsetX) endY = event->pos().y();
	if(type == "layers") {
		endY = event->pos().y();
		emit mouseMovedY(endY-startY);
	}
	int frameNumber = getFrameNumber(event->pos().x());
	int layerNumber = getLayerNumber(event->pos().y());
	if(timeLine->scrubbing && type == "tracks") {
		editor->scrubTo(frameNumber);
	} else {
		if(layerNumber != -1 && layerNumber < editor->object->getLayerCount()) {
			mouseMove(event, frameNumber, editor->object->getLayer(layerNumber));
		}
	}
	timeLine->update();
}

void TimeLineCells::mouseReleaseEvent(QMouseEvent *event) {
	endY = startY;
	emit mouseMovedY(0);
	timeLine->scrubbing = false;
	int frameNumber = getFrameNumber(event->pos().x());
	if(frameNumber < 1) frameNumber = -1;
	int layerNumber = getLayerNumber(event->pos().y());
	if(type == "tracks" && layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
		mouseRelease(event, frameNumber, editor->object->getLayer(layerNumber));
	}
	//if(event->pos().x() < offsetX && layerNumber != startLayerNumber && startLayerNumber != -1 && layerNumber != -1) {
	if(type == "layers" && layerNumber != startLayerNumber && startLayerNumber != -1 && layerNumber != -1) {
		editor->moveLayer(startLayerNumber, layerNumber);
	}
	update();
}

void TimeLineCells::mouseDoubleClickEvent(QMouseEvent *event) {
	int frameNumber = getFrameNumber(event->pos().x());
	int layerNumber = getLayerNumber(event->pos().y());

	// -- short scrub --
	if(event->pos().y() < 20) {
		if(shortScrub) scrubChange(0); else scrubChange(1);
	}
	
	// -- layer --
	Layer* layer = editor->object->getLayer( layerNumber );
	//if(layerNumber != -1 && layerNumber < editor->object->getLayerCount() ) {
	if(layer) {
		if(type == "tracks" && (layerNumber != -1) && (frameNumber > 0) && layerNumber < editor->object->getLayerCount()) {
			mouseDoubleClick(event, frameNumber, editor->object->getLayer(layerNumber));
		}
		if(type == "layers") {
			layer->editProperties();
		}
	}
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

void TimeLineCells::scrubChange(int x) {
	QSettings settings("Pencil","Pencil");
	if (x==0) { shortScrub=false; settings.setValue("shortScrub","false"); }
	else { shortScrub=true; settings.setValue("shortScrub","true"); }
	update();
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
	update();
}

void TimeLineCells::vScrollChange(int x) {
	layerOffset = x;
	update();
}

