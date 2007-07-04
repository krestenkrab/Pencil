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
#ifndef TIMECONTROL_H
#define TIMECONTROL_H

#include <QWidget>
#include <QPushButton>
#include <QToolButton>

class TimeControls : public QWidget
{
    Q_OBJECT
  
public:
	TimeControls(QWidget *parent = 0);

signals:
	void playClick();
	void loopClick();
	void soundClick();
	
public slots:
	void updateLoopButton(bool);
	
protected:
	
private:
	QPushButton* loopButton;
};

#endif