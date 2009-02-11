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
#include <QObject>
#include "editor.h"
#include <interfaces.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
		void exportFile(ExportInterface*);
		
public:
    MainWindow();
    void addRecentFile(QString filePath);

public slots:
		void setOpacity(int opacity);

private slots:
		void exportFile();
				
private:
	void arrangePalettes();
	void createMenus();
	void loadPlugins();
	void populateMenus(QObject *plugin);
	void addToMenu(QObject *plugin, const QString text, QMenu *menu, const char *member, QActionGroup *actionGroup = 0);
	
 	void closeEvent(QCloseEvent *);
	void keyPressEvent( QKeyEvent *e);
	void keyReleaseEvent( QKeyEvent *e);
	void readSettings();
	void writeSettings();
	
	Editor *editor;
	
	QMenu *fileMenu;
    QMenu *openRecentMenu;
    QMenu *importMenu;
    QMenu *exportMenu;
    QMenu *editMenu;
    QMenu *layerMenu;
    QMenu *helpMenu;
	QMenu *windowsMenu;
		
    QAction *newAct;
	QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
    QAction *helpMe;
    QAction *aboutAct;
    QAction *aboutQtAct;

	QAction *exportPaletteAct;
	QAction *importPaletteAct;
	QAction *savAct;
	QAction *importAct;
	QAction *undoAct;
	QAction *redoAct;
	QAction *copyAct;
	QAction *pasteAct;
	QAction *selectAllAct;
	QAction *importSndAct;
	QAction *preferencesAct;
	
	QAction *newBitmapLayerAct;
	QAction *newVectorLayerAct;
	QAction *newSoundLayerAct;
	QAction *newCameraLayerAct;
	QAction *deleteLayerAct;
	
	QAction *dockAllPalettesAct;
	QAction *detachAllPalettesAct;
};


