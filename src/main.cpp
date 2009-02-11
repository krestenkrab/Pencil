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
	MainWindow mainWindow;
	mainWindow.show();
	qDebug() << "MainWindow thread" << mainWindow.thread();
	qDebug() << "App thread" << app.thread();
	return app.exec();
}

