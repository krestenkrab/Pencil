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
#include "timecontrols.h"

TimeControls::TimeControls(QWidget* parent) {

	QSettings settings("Pencil","Pencil");

	QFrame* frame = new QFrame();

	QSpinBox* fpsBox = new QSpinBox(this);
	//fpsBox->setFixedWidth(50);
	fpsBox->setFont( QFont("Helvetica", 10) );
	fpsBox->setFixedHeight(22);
	fpsBox->setValue(settings.value("fps").toInt());
	fpsBox->setMinimum(1);
	fpsBox->setMaximum(50);
	fpsBox->setToolTip("Frames per second");
	fpsBox->setFocusPolicy(Qt::NoFocus);

	QPushButton* playButton = new QPushButton();
	loopButton = new QPushButton();
	soundButton = new QPushButton();
	QLabel* separator = new QLabel();
	separator->setPixmap(QPixmap(":icons/controls/separator.png"));
	QLabel* spacingLabel = new QLabel(""); spacingLabel->setIndent(6);
	QLabel* fpsLabel = new QLabel(tr("Fps: ")); fpsLabel->setIndent(6);

	playButton->setIcon(QIcon(":icons/controls/play.png"));
	loopButton->setIcon(QIcon(":icons/controls/loop.png"));
	soundButton->setIcon(QIcon(":icons/controls/sound.png"));

	playButton->setToolTip(tr("Play"));
	loopButton->setToolTip(tr("Loop"));
	soundButton->setToolTip(tr("Sound on/off"));

	loopButton->setCheckable(true);
	soundButton->setCheckable(true);
	soundButton->setChecked(true);

	QHBoxLayout* frameLayout = new QHBoxLayout();
	frameLayout->setMargin(0);
	frameLayout->setSpacing(5);
	frameLayout->addWidget(separator);
	frameLayout->addWidget(playButton);
	frameLayout->addWidget(loopButton);
	frameLayout->addWidget(soundButton);
	frameLayout->addWidget(fpsLabel);
	frameLayout->addWidget(fpsBox);
	frameLayout->addWidget(spacingLabel);
	frame->setLayout(frameLayout);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setAlignment(Qt::AlignRight);
	layout->addWidget(frame);
	layout->setMargin(0);
	//layout->setSizeConstraint(QLayout::SetNoConstraint);
	setLayout(layout);
	//setBackgroundRole(QPalette::Dark);
	//setFixedWidth(200);

	connect(playButton, SIGNAL(clicked()), this, SIGNAL(playClick()));
	connect(loopButton, SIGNAL(clicked()), this, SIGNAL(loopClick()));
	connect(soundButton, SIGNAL(clicked()), this, SIGNAL(soundClick()));
	connect(fpsBox,SIGNAL(valueChanged(int)), this, SIGNAL(fpsClick(int)));
	
	updateButtons(false);
}

void TimeControls::updateButtons(bool floating) {
	if(floating) {
		loopButton->setFixedSize(QSize(45,22));
		soundButton->setFixedSize(QSize(45,22));
	} else {
		loopButton->setFixedSize(QSize(26,17));
		soundButton->setFixedSize(QSize(26,17));
	}
}

/*void TimeControls::updateLoopButton(bool checked) {
	if(checked) {
		loopButton->setIcon(QIcon(":icons/controls/loopOn.png"));
	} else {
		loopButton->setIcon(QIcon(":icons/controls/loopOff.png"));
	}
}*/
