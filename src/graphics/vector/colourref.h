/*

Pencil - Traditional Animation Software
Copyright (C) 2006-2009 Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef COLOURREF_H
#define COLOURREF_H

#include <QColor>
#include <QString>

class ColourRef
{
public:
	ColourRef();
	ColourRef(QColor theColour, QString theName);
	bool operator==(ColourRef colourRef1);
	bool operator!=(ColourRef colourRef1); 
	
	QColor colour;
	QString name;
};

#endif
