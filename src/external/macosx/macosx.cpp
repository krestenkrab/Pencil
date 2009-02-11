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
#include <QString>
#include <QStringList>
#include <QDir>
#include <QProgressDialog>
#include <QProcess>

#include "object.h"
#include "mainwindow.h"
#include "style.h"

#include <CoreFoundation/CoreFoundation.h>

void initialise() {
	qApp->setStyle(new AquaStyle());
}

