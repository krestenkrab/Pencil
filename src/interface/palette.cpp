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
#include <QtDebug>
#include <QtGui>
#include "palette.h"
#include "colourref.h"
#include "object.h"
#include "editor.h"

Palette::Palette(Editor* editor) : QDockWidget(editor, Qt::Tool)
{
	this->editor = editor;
	
	QWidget* paletteContent = new QWidget(this);
	//paletteContent->setWindowFlags(Qt::FramelessWindowHint);
	
	
	listOfColours = new QListWidget();
	QWidget *buttons = new QWidget();
	addButton = new QToolButton(this);
	rmButton = new QToolButton(this);
	addButton->setIcon(QIcon(":icons/add.png"));
	addButton->setToolTip("Add Colour");
	rmButton->setIcon(QIcon(":icons/remove.png"));
	rmButton->setToolTip("Remove Colour");
	
	QGridLayout *buttonLayout = new QGridLayout();
	buttonLayout->addWidget(addButton,0,0);
	buttonLayout->addWidget(rmButton,0,1);
	buttons->setLayout(buttonLayout);
	
	listOfColours->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	listOfColours->setLineWidth(1);
	listOfColours->setFocusPolicy(Qt::NoFocus);
	
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(buttons);
	layout->addWidget(listOfColours);
	
	paletteContent->setLayout(layout);
	setWidget(paletteContent);
	
	//setFrameStyle(QFrame::Panel);
	//setWindowFlags(Qt::Tool);
	setWindowFlags(Qt::WindowStaysOnTopHint);
	//setWindowFlags(Qt::SubWindow);
	setFloating(true);
	//setAllowedAreas(Qt::NoDockWidgetArea);
	setMinimumSize(100, 300);
	setMaximumWidth(130);
	setGeometry(10,60,100, 300);
	//setFocusPolicy(Qt::NoFocus);
	//setWindowOpacity(0.7);
	setWindowTitle(tr("Colours"));
	
	connect(listOfColours, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(selectColour(QListWidgetItem *, QListWidgetItem *)));
	connect(listOfColours, SIGNAL(itemDoubleClicked ( QListWidgetItem *)), this, SLOT(changeColour( QListWidgetItem *)));
	
	connect(addButton, SIGNAL(clicked()), this, SLOT(addClick()));
	connect(rmButton, SIGNAL(clicked()), this, SLOT(rmClick()));
	
	connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(closeIfDocked(bool)));
}

void Palette::updateList() {
	
	//listOfColours->clear(); // for some reason, this creates an bus error when one removes the last element
	while(listOfColours->count() > 0) listOfColours->takeItem(0);
	for(int i=0; i <  editor->object->getColourCount(); i++) {
		ColourRef colourRef = editor->object->getColour(i);
		QListWidgetItem* colourItem = new QListWidgetItem(listOfColours);
		colourItem->setText( colourRef.name );
		QPixmap colourSwatch(32,32);
		colourSwatch.fill( colourRef.colour );
		QPainter painter(&colourSwatch);
		painter.setPen( QColor(0,0,0,128) );
		painter.drawRect( QRect(0,-1,31,31) );
		colourItem->setIcon( colourSwatch );
	}
	update();
}

void Palette::selectColour(QListWidgetItem* current, QListWidgetItem* previous) {
	if (!current) current = previous;
	editor->selectColour(listOfColours->row(current));
}

void Palette::changeColour( QListWidgetItem * item ) {
	if(item != NULL) editor->changeColour(listOfColours->row(item));
}

void Palette::addClick() {
	editor->addColour();
}

void Palette::rmClick() {
	editor->removeColour(listOfColours->currentRow());
}

void Palette:: closeIfDocked(bool floating) {
	//if(floating == false) close(); // we don't want to dock the palette in the mainwindow (or do we?)
}

