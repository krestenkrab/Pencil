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
#include <QApplication>
#include <QDir>
#include "mainwindow.h"

void initialise();

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
#ifndef Q_WS_MAC
	app.setWindowIcon(QIcon(":/icons/icon.png"));
#endif
	initialise();
	QDir dir(QApplication::applicationDirPath());
	dir.cdUp();
	if(dir.cd("plugins")) {
		QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
	}
	MainWindow mainWindow;
	mainWindow.show();
    return app.exec();
}

