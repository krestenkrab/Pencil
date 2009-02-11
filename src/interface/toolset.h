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
#ifndef TOOLSET_H
#define TOOLSET_H

#include <QtGui>

class SpinSlider : public QWidget
{
    Q_OBJECT
	public:
    SpinSlider(QString text, QString type, QString dataType, qreal min, qreal max, QWidget *parent = 0);
	public slots:
		void changeValue(qreal);
		void changeValue(int);
		void setValue(qreal);
		void sliderReleased();
		void sliderMoved(int);
	signals:
		void valueChanged(qreal);
	private:
		QLabel* valueLabel;
		QSlider* slider;
		qreal value, min, max;
		QString type, dataType;
};


class ToolSet : public QWidget
{
    Q_OBJECT

public:
	ToolSet();
	
	QDockWidget *drawPalette;
	QDockWidget *optionPalette;
	QDockWidget *displayPalette;
	QDockWidget *timePalette;
	
public slots:
	void setCounter(int);
	void setWidth(qreal);
	void setFeather(qreal);
	void setOpacity(qreal);
	void setInvisibility(int);
	void setPressure(int);
	void setPreserveAlpha(int);
	void setFollowContour(int);
	void setColour(QColor);
	//void playStart();
	void changePencilButton();
	void changeEraserButton();
	void changeSelectButton();
	void changeMoveButton();
	void changeHandButton();
	void changePenButton();
	void changePolylineButton();
	void changeBucketButton();
	void changeEyedropperButton();
	void changeColouringButton();
	void changeThinLinesButton(bool);
	void changeOutlinesButton(bool);
	void changeSmudgeButton();
	
	void resetMirror();
	void pressureClick(bool);
	void invisibleClick(bool);
	void preserveAlphaClick(bool);
	void followContourClick(bool);
	
signals:
	void pencilClick();
	void eraserClick();
	void selectClick();
	void moveClick();
	void handClick();
	void penClick();
	void pressureClick(int);
	void invisibleClick(int);
	void preserveAlphaClick(int);
	void followContourClick(int);
	void polylineClick();
	void bucketClick();
	void eyedropperClick();
	void colouringClick();
	void smudgeClick();
	
	void lengthClick(QString);
	void clearClick();
	void thinLinesClick();
	void outlinesClick();
	void mirrorClick();
	void prevClick();
	void nextClick();
	void widthClick(qreal);
	void featherClick(qreal);
	void opacityClick(qreal);
	void colourClick();
	
private:
	void newToolButton(QToolButton* &toolButton,QWidget* widget);
	
	QToolButton *choseColour;
	QPixmap *colourSwatch;
	QToolButton *pencilButton;
	QToolButton *selectButton;
	QToolButton *moveButton;
	QToolButton *handButton;
	QToolButton *eraserButton;
	QToolButton *penButton;
	QToolButton *polylineButton;
	QToolButton *bucketButton;
	QToolButton *colouringButton;
	QToolButton *eyedropperButton;
	QToolButton *magnifyButton;
	QToolButton *smudgeButton;
	
	QToolButton *thinLinesButton;
	QToolButton *outlinesButton;
	
	QToolButton *mirrorButton;
	QToolButton *clearButton;
	
	SpinSlider *sizeSlider;
	SpinSlider *featherSlider;
	SpinSlider *opacitySlider;
	
	QCheckBox *usePressureBox;
	QCheckBox *makeInvisibleBox;
	QCheckBox *preserveAlphaBox;
	QCheckBox *followContourBox;
	
	void deselectAllTools();
	
	
};

#endif
