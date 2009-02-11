/*

Pencil - Traditional Animation Software
Copryight (C) 2005 Patrick Corrieri
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
#include "toolset.h"
#include <math.h>

SpinSlider::SpinSlider(QString text, QString type, QString dataType, qreal min, qreal max, QWidget* parent) : QWidget(parent) {
	value = 1.0;
	this->type = type;
	this->dataType = dataType;
	this->min = min;
	this->max = max;
	QLabel* label = new QLabel(text+": ");
	label->setFont( QFont("Helvetica", 10) );
	valueLabel = new QLabel("--");
	valueLabel->setFont( QFont("Helvetica", 10) );
        //valueLabel->setFixedWidth(30);
	valueLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	slider = new QSlider(Qt::Horizontal, this);
	slider->setMinimum(0);
	slider->setMaximum(100);
				//slider->setFixedHeight(16);
	QGridLayout *lay = new QGridLayout();
	lay->setMargin(2);
	lay->setSpacing(2);
        //lay->setColumnStretch(0,1);
	lay->addWidget(label,0,0,1,1);
	lay->addWidget(valueLabel,0,1,1,1);
	lay->addWidget(slider,1,0,1,2);
	setLayout(lay);
        setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);

	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(changeValue(int)));
	connect(slider, SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
	connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoved(int)));
}

void SpinSlider::changeValue(qreal value) {
	this->value = value;
	if(dataType == "integer") {
		valueLabel->setText( QString::number(qRound(value)) );
	} else {
		valueLabel->setText( QLocale::system().toString(value,'f',1) );
	}
}

void SpinSlider::changeValue(int value) {
	qreal value2 = 0.0;
	if(type=="linear") value2 = min + value*(max-min)/100;
	if(type=="log") value2 = min*exp( value*log(max/min)/100 );
	changeValue(value2);
}

void SpinSlider::setValue(qreal value) {
	qreal value2 = 0.0;
	if(type=="linear") value2 = qRound( 100*(value-min)/(max-min) );
	if(type=="log") value2 = qRound( 100.0*log(value/0.2)/log(1000.0) );
	slider->setSliderPosition(value2);
	changeValue(value);
}

void SpinSlider::sliderReleased() {
	emit valueChanged(this->value);
}

void SpinSlider::sliderMoved(int value) {
	changeValue(value);
	emit valueChanged(this->value);
}

// ----------------------------------------------------------------------------------

ToolSet::ToolSet() {

	drawPalette = new QDockWidget(tr("Tools"));
	optionPalette = new QDockWidget(tr("Options"));
	displayPalette = new QDockWidget(tr("Display"));

	QFrame *drawGroup = new QFrame();
	QFrame *optionGroup = new QFrame();
	QFrame *displayGroup = new QFrame();

	drawPalette->setWidget(drawGroup);
	optionPalette->setWidget(optionGroup);
	displayPalette->setWidget(displayGroup);

	QGridLayout *drawLay = new QGridLayout();
	QGridLayout *optionLay = new QGridLayout();
	QGridLayout *displayLay = new QGridLayout();

	QSettings settings("Pencil","Pencil");

	newToolButton(pencilButton, drawPalette); // = new QToolButton(this);
	newToolButton(selectButton, drawPalette); //  = new QToolButton(this);
	newToolButton(moveButton, drawPalette); //  = new QToolButton(this);
	newToolButton(handButton, drawPalette); //  = new QToolButton(this);
	newToolButton(penButton, drawPalette); //  = new QToolButton(this);
	newToolButton(eraserButton, drawPalette); //  = new QToolButton(this);
	newToolButton(polylineButton, drawPalette); //  = new QToolButton(this);
	newToolButton(bucketButton, drawPalette); //  = new QToolButton(this);
	newToolButton(colouringButton, drawPalette); //  = new QToolButton(this);
	newToolButton(eyedropperButton, drawPalette); //  = new QToolButton(this);
	newToolButton(clearButton, drawPalette); //  = new QToolButton(this);
	//newToolButton(magnifyButton, drawPalette);
	newToolButton(smudgeButton, drawPalette);

	thinLinesButton = new QToolButton(displayPalette);
	outlinesButton = new QToolButton(displayPalette);
	mirrorButton = new QToolButton(displayPalette); //  = new QToolButton(this);

	choseColour = new QToolButton(optionPalette);

	pencilButton->setWhatsThis("Draw");
	penButton->setWhatsThis("Pen");
	polylineButton->setWhatsThis("Polyline");
	bucketButton->setWhatsThis("Paint bucket");
	colouringButton->setWhatsThis("Brush");
	usePressureBox = new QCheckBox("Size with pressure");
	usePressureBox->setFont( QFont("Helvetica", 10) );
	usePressureBox->setChecked(true);
	makeInvisibleBox = new QCheckBox("Invisible");
	makeInvisibleBox->setFont( QFont("Helvetica", 10) );
	makeInvisibleBox->setChecked(false);
	preserveAlphaBox = new QCheckBox("Preserve Alpha");
	preserveAlphaBox->setFont( QFont("Helvetica", 10) );
	preserveAlphaBox->setChecked(false);
	followContourBox = new QCheckBox("Stop at contours");
	followContourBox->setFont( QFont("Helvetica", 10) );
	followContourBox->setChecked(false);

	/*sizeSlider = new QDoubleSpinBox(this);
	sizeSlider->setFocusPolicy(Qt::NoFocus);
	sizeSlider->setFont( QFont("Helvetica", 10) );
	sizeSlider->setDecimals(1);
	sizeSlider->setMinimum(0);
	sizeSlider->setMaximum(200);
	sizeSlider->setValue(settings.value("width").toInt());
	sizeSlider->setFixedWidth(50);
	sizeSlider->setFixedHeight(22);*/

	sizeSlider = new SpinSlider("Size", "log", "real", 0.2, 200.0, optionPalette);
	sizeSlider->setValue(settings.value("pencilWidth").toDouble());

	featherSlider = new SpinSlider("Feather", "log", "real", 0.2, 200.0, optionPalette);
	featherSlider->setValue(settings.value("pencilFeather").toDouble());

	//opacitySlider = new SpinSlider("Opacity", "linear", "real", 0.0, 1.0, optionPalette);
	//opacitySlider->setValue(settings.value("pencilOpacity").toDouble());

	/*featherSlider = new QSlider(Qt::Horizontal, this);
	featherSlider->setMinimum(0);
	featherSlider->setMaximum(100);
	featherSlider->setFixedHeight(22);*/

	sizeSlider->setToolTip("Set Pen Width");
	pencilButton->setIcon(QIcon(":icons/pencil2.png"));
	pencilButton->setToolTip("Sketch");
	penButton->setIcon(QIcon(":icons/pen.png"));
	penButton->setToolTip("Draw");
	polylineButton->setIcon(QIcon(":icons/polyline.png"));
	polylineButton->setToolTip("Polyline");
	bucketButton->setIcon(QIcon(":icons/bucket.png"));
	bucketButton->setToolTip("Fill");
	colouringButton->setIcon(QIcon(":icons/brush.png"));
	colouringButton->setToolTip("Colouring tool");
	eyedropperButton->setIcon(QIcon(":icons/eyedropper.png"));
	eyedropperButton->setToolTip("Pick colour");
	selectButton->setIcon(QIcon(":icons/select.png"));
	selectButton->setToolTip("Select");
	moveButton->setIcon(QIcon(":icons/arrow.png"));
	moveButton->setToolTip("Select/Move object");
	handButton->setIcon(QIcon(":icons/hand.png"));
	handButton->setToolTip("Move/Zoom canvas");
	//magnifyButton->setIcon(QIcon(":icons/magnify.png"));
	//magnifyButton->setToolTip("Magnify canvas");
	//magnifyButton->setEnabled(false);
	smudgeButton->setIcon(QIcon(":icons/smudge.png"));
	smudgeButton->setToolTip("Modify curve");
	smudgeButton->setEnabled(true);
	eraserButton->setIcon(QIcon(":icons/eraser.png"));
	eraserButton->setToolTip("Erase");


	QPixmap colourSwatch(30,30);
	colourSwatch.fill(Qt::black);
	QLabel* colourLabel = new QLabel();
	colourLabel->setText(tr("Colour:"));
	colourLabel->setFont( QFont("Helvetica", 10) );
	choseColour->setIcon(colourSwatch);
	choseColour->setToolTip("Display Colours");

	thinLinesButton->setIcon(QIcon(":icons/thinlines5.png"));
	thinLinesButton->setToolTip("Show invisible lines");
	outlinesButton->setIcon(QIcon(":icons/outlines5.png"));
	outlinesButton->setToolTip("Show outlines only");

	mirrorButton->setIcon(QIcon(":icons/mirror.png"));
	mirrorButton->setToolTip("Horizontal flip");
	clearButton->setIcon(QIcon(":icons/clear.png"));
	clearButton->setToolTip("Clear");
	thinLinesButton->setCheckable(true);
	thinLinesButton->setChecked(false);
	outlinesButton->setCheckable(true);
	outlinesButton->setChecked(false);
	mirrorButton->setCheckable(true);
	mirrorButton->setChecked(false);
	pencilButton->setCheckable(true);
	penButton->setCheckable(true);
	polylineButton->setCheckable(true);
	bucketButton->setCheckable(true);
	colouringButton->setCheckable(true);
	smudgeButton->setCheckable(true);
	eyedropperButton->setCheckable(true);
	selectButton->setCheckable(true);
	moveButton->setCheckable(true);
	handButton->setCheckable(true);
	eraserButton->setCheckable(true);
	pencilButton->setChecked(true);

	drawLay->setMargin(2);
	drawLay->setSpacing(0);

	drawLay->addWidget(pencilButton,0,0); drawLay->setAlignment(pencilButton, Qt::AlignRight);
	drawLay->addWidget(colouringButton,0,1); drawLay->setAlignment(colouringButton, Qt::AlignLeft);

	drawLay->addWidget(penButton,0,2); drawLay->setAlignment(penButton, Qt::AlignRight);
	drawLay->addWidget(polylineButton,0,3); drawLay->setAlignment(polylineButton, Qt::AlignLeft);

	drawLay->addWidget(eraserButton,1,0); drawLay->setAlignment(eraserButton, Qt::AlignRight);
	drawLay->addWidget(clearButton,1,1); drawLay->setAlignment(clearButton, Qt::AlignLeft);

	drawLay->addWidget(bucketButton,1,2); drawLay->setAlignment(bucketButton, Qt::AlignRight);
	drawLay->addWidget(eyedropperButton,1,3); drawLay->setAlignment(eyedropperButton, Qt::AlignLeft);

	drawLay->addWidget(selectButton,2,0); drawLay->setAlignment(selectButton, Qt::AlignRight);
	drawLay->addWidget(moveButton,2,1); drawLay->setAlignment(moveButton, Qt::AlignLeft);

	drawLay->addWidget(handButton,2,3); drawLay->setAlignment(handButton, Qt::AlignRight);
	drawLay->addWidget(smudgeButton,2,2); drawLay->setAlignment(smudgeButton, Qt::AlignLeft);
	//drawLay->addWidget(mirrorButton,5,1); drawLay->setAlignment(mirrorButton, Qt::AlignLeft);

	optionLay->setMargin(8);
	optionLay->setSpacing(8);
	optionLay->addWidget(colourLabel,6,0);
	optionLay->addWidget(choseColour,6,1);
	//optionLay->addWidget(thinLinesButton,6,2);

	//optionLay->addWidget(sizeSlider,7,0,1,2);


	/*QGroupBox* sizeBox = new QGroupBox("Size");
	QHBoxLayout* sizeBoxLay = new QHBoxLayout();
	sizeBoxLay->addWidget(sizeSlider2);
	sizeBoxLay->setMargin(0);
	sizeBoxLay->setSpacing(0);
	sizeBox->setLayout(sizeBoxLay);*/

	optionLay->addWidget(sizeSlider,8,0,1,2);
	optionLay->addWidget(featherSlider,9,0,1,2);
	//optionLay->addWidget(opacitySlider,10,0,1,2);

	optionLay->addWidget(usePressureBox,11,0,1,2);
	optionLay->addWidget(preserveAlphaBox,12,0,1,2);
	optionLay->addWidget(followContourBox,13,0,1,2);
	optionLay->addWidget(makeInvisibleBox,14,0,1,2);
        optionLay->setRowStretch(15,1);

	displayLay->setMargin(4);
	displayLay->setSpacing(0);
	displayLay->addWidget(mirrorButton,0,0);
	displayLay->addWidget(thinLinesButton,0,1);
	displayLay->addWidget(outlinesButton,0,2);

	drawGroup->setLayout(drawLay);
	optionGroup->setLayout(optionLay);
	displayGroup->setLayout(displayLay);

	drawGroup->setMaximumHeight(6*32+1);
	
	drawPalette->setMaximumHeight(200);
	//drawPalette->setFeatures(drawPalette->features() | QDockWidget::DockWidgetVerticalTitleBar);
        //optionPalette->setMaximumHeight(300);
        //optionGroup->setMaximumHeight(160);
	displayPalette->setMaximumHeight(60);

	connect(pencilButton, SIGNAL(clicked()), this, SIGNAL(pencilClick()));
	connect(selectButton, SIGNAL(clicked()), this, SIGNAL(selectClick()));
	connect(moveButton, SIGNAL(clicked()), this, SIGNAL(moveClick()));
	connect(handButton, SIGNAL(clicked()), this, SIGNAL(handClick()));
	connect(eraserButton, SIGNAL(clicked()), this, SIGNAL(eraserClick()));
	connect(penButton, SIGNAL(clicked()), this, SIGNAL(penClick()));
	connect(polylineButton, SIGNAL(clicked()), this, SIGNAL(polylineClick()));
	connect(bucketButton, SIGNAL(clicked()), this, SIGNAL(bucketClick()));
	connect(eyedropperButton, SIGNAL(clicked()), this, SIGNAL(eyedropperClick()));
	connect(colouringButton, SIGNAL(clicked()), this, SIGNAL(colouringClick()));
	connect(smudgeButton, SIGNAL(clicked()), this, SIGNAL(smudgeClick()));

	connect(thinLinesButton, SIGNAL(clicked()), this, SIGNAL(thinLinesClick()));
	connect(outlinesButton, SIGNAL(clicked()), this, SIGNAL(outlinesClick()));
	//connect(outlinesButton, SIGNAL(pressed()), this, SIGNAL(outlinesPressed()));
	//connect(outlinesButton, SIGNAL(released()), this, SIGNAL(outlinesReleased()));

	connect(usePressureBox, SIGNAL(clicked(bool)), this, SLOT(pressureClick(bool)));
	connect(makeInvisibleBox, SIGNAL(clicked(bool)), this, SLOT(invisibleClick(bool)));
	connect(preserveAlphaBox, SIGNAL(clicked(bool)), this, SLOT(preserveAlphaClick(bool)));
	connect(followContourBox, SIGNAL(clicked(bool)), this, SLOT(followContourClick(bool)));
	connect(sizeSlider, SIGNAL(valueChanged(qreal)), this, SIGNAL(widthClick(qreal)));
	connect(featherSlider, SIGNAL(valueChanged(qreal)), this, SIGNAL(featherClick(qreal)));
	//connect(opacitySlider, SIGNAL(valueChanged(qreal)), this, SIGNAL(opacityClick(qreal)));

	connect(choseColour, SIGNAL(clicked()), this, SIGNAL(colourClick()));
	connect(clearButton, SIGNAL(clicked()), this, SIGNAL(clearClick()));
	connect(mirrorButton, SIGNAL(clicked()), this, SIGNAL(mirrorClick()));

	connect(pencilButton, SIGNAL(clicked()), this, SLOT(changePencilButton()));
	connect(selectButton, SIGNAL(clicked()), this, SLOT(changeSelectButton()));
	connect(moveButton, SIGNAL(clicked()), this, SLOT(changeMoveButton()));
	connect(handButton, SIGNAL(clicked()), this, SLOT(changeHandButton()));
	connect(eraserButton, SIGNAL(clicked()), this, SLOT(changeEraserButton()));
	connect(penButton, SIGNAL(clicked()), this, SLOT(changePenButton()));
	connect(polylineButton, SIGNAL(clicked()), this, SLOT(changePolylineButton()));
	connect(bucketButton, SIGNAL(clicked()), this, SLOT(changeBucketButton()));
	connect(eyedropperButton, SIGNAL(clicked()), this, SLOT(changeEyedropperButton()));
	connect(colouringButton, SIGNAL(clicked()), this, SLOT(changeColouringButton()));
	connect(smudgeButton, SIGNAL(clicked()), this, SLOT(changeSmudgeButton()));

	//connect(thinLinesButton, SIGNAL(clicked()), this, SLOT(changeThinLinesButton()));
}

void ToolSet::newToolButton(QToolButton* &toolButton, QWidget* widget) {
	toolButton = new QToolButton(widget);
	toolButton->setAutoRaise(true);
	toolButton->setIconSize( QSize(24,24) );
	toolButton->setFixedSize(32,32);
}

void ToolSet::setCounter(int x) {
	//framecounter->setText(QString::number(x));
}

void ToolSet::setWidth(qreal x) {
	if(x < 0) {
		sizeSlider->setEnabled(false);
	} else {
		sizeSlider->setEnabled(true);
		sizeSlider->setValue(x);
	}
}

void ToolSet::setFeather(qreal x) {
	if(x < 0) {
		featherSlider->setEnabled(false);
	} else {
		featherSlider->setEnabled(true);
		featherSlider->setValue(x);
	}
}

void ToolSet::setOpacity(qreal x) {
	if(x < 0) {
		//opacitySlider->setEnabled(false);
	} else {
		//opacitySlider->setEnabled(true);
		//opacitySlider->setValue(x);
	}
}

void ToolSet::setPressure(int x) { // x = -1, 0, 1
	if(x<0) {
		usePressureBox->setEnabled(false);
	} else {
		usePressureBox->setEnabled(true);
		usePressureBox->setChecked(x>0);
	}
}

void ToolSet::pressureClick(bool x) {
	int y = 0;
	if(x) y = 1;
	emit pressureClick(y);
}

void ToolSet::setInvisibility(int x) { // x = -1, 0, 1
	if(x<0) {
		makeInvisibleBox->setEnabled(false);
	} else {
		makeInvisibleBox->setEnabled(true);
		makeInvisibleBox->setChecked(x>0);
	}
}

void ToolSet::invisibleClick(bool x) {
	int y = 0;
	if(x) y = 1;
	emit invisibleClick(y);
}

void ToolSet::setPreserveAlpha(int x) { // x = -1, 0, 1
	if(x<0) {
		preserveAlphaBox->setEnabled(false);
	} else {
		preserveAlphaBox->setEnabled(true);
		preserveAlphaBox->setChecked(x>0);
	}
}

void ToolSet::preserveAlphaClick(bool x) {
	int y = 0;
	if(x) y = 1;
	emit preserveAlphaClick(y);
}

void ToolSet::setFollowContour(int x) { // x = -1, 0, 1
	if(x<0) {
		followContourBox->setEnabled(false);
	} else {
		followContourBox->setEnabled(true);
		followContourBox->setChecked(x>0);
	}
}

void ToolSet::followContourClick(bool x) {
	int y = 0;
	if(x) y = 1;
	emit followContourClick(y);
}

void ToolSet::setColour(QColor x) {
	QPixmap colourSwatch(30,30);
	colourSwatch.fill(x);
	choseColour->setIcon(colourSwatch);
}

void ToolSet::changePencilButton() {
	deselectAllTools();
	pencilButton->setChecked(true);
}

void ToolSet::changeEraserButton() {
	deselectAllTools();
	eraserButton->setChecked(true);
}

void ToolSet::changeSelectButton() {
	deselectAllTools();
	selectButton->setChecked(true);
}

void ToolSet::changeMoveButton() {
	deselectAllTools();
	moveButton->setChecked(true);
}

void ToolSet::changeHandButton() {
	deselectAllTools();
	handButton->setChecked(true);
}

void ToolSet::changePenButton() {
	deselectAllTools();
	penButton->setChecked(true);
}

void ToolSet::changePolylineButton() {
	deselectAllTools();
	polylineButton->setChecked(true);
}

void ToolSet::changeBucketButton() {
	deselectAllTools();
	bucketButton->setChecked(true);
}

void ToolSet::changeEyedropperButton() {
	deselectAllTools();
	eyedropperButton->setChecked(true);
}

void ToolSet::changeColouringButton() {
	deselectAllTools();
	colouringButton->setChecked(true);
}

void ToolSet::changeSmudgeButton() {
	deselectAllTools();
	smudgeButton->setChecked(true);
}

void ToolSet::changeOutlinesButton(bool trueOrFalse) {
	outlinesButton->setChecked(trueOrFalse);
}


void ToolSet::changeThinLinesButton(bool trueOrFalse) {
	thinLinesButton->setChecked(trueOrFalse);
}


void ToolSet::resetMirror() {
	mirrorButton->setChecked(false);
}

void ToolSet::deselectAllTools() {
	pencilButton->setChecked(false);
	eraserButton->setChecked(false);
	selectButton->setChecked(false);
	moveButton->setChecked(false);
	handButton->setChecked(false);
	penButton->setChecked(false);
	polylineButton->setChecked(false);
	bucketButton->setChecked(false);
	eyedropperButton->setChecked(false);
	colouringButton->setChecked(false);
	smudgeButton->setChecked(false);
}


