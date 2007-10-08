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
#include "bitmapimage.h"
#include "object.h"
#include <math.h>

BitmapImage::BitmapImage() {
	// nothing
	image = NULL;
}

BitmapImage::BitmapImage(Object *parent) {
	myParent = parent;
	image = new QImage(0, 0, QImage::Format_ARGB32_Premultiplied);
	boundaries = QRect(0,0,0,0);
}

BitmapImage::BitmapImage(Object *parent, QRect rectangle, QColor colour) {
	myParent = parent;
	boundaries = rectangle;
	image = new QImage( boundaries.size(), QImage::Format_ARGB32_Premultiplied);
	image->fill(colour.rgba());
}

BitmapImage::BitmapImage(Object *parent, QRect rectangle, QImage image) {
	myParent = parent;
	boundaries = rectangle.normalized();
	this->image = new QImage(image);
	if(this->image->width() != rectangle.width() || this->image->height() != rectangle.height()) qDebug() << "Error instancing bitmapImage.";
}

/*BitmapImage::BitmapImage(Object *parent, QImage image, QPoint topLeft) {
	myParent = parent;
	this->image = new QImage(image);
	boundaries =  QRect( topLeft, image.size() );
}*/

BitmapImage::BitmapImage(const BitmapImage &a) {
  myParent=a.myParent;
  boundaries=a.boundaries;
  image=new QImage(*a.image);
}

BitmapImage::BitmapImage(Object *parent, QString path, QPoint topLeft) {
	myParent = parent;
	image = new QImage(path);
	boundaries = QRect( topLeft, image->size() );
}

BitmapImage::~BitmapImage() {
	if(image) delete image;
}

BitmapImage &BitmapImage::operator=(const BitmapImage &a) {
  myParent=a.myParent;
  boundaries=a.boundaries;
  image=new QImage(*a.image);
  return *this;
}

QDomElement BitmapImage::createDomElement(QDomDocument &doc) {
	return QDomElement();  // empty
}

void BitmapImage::loadDomElement(QDomElement imageElement, QString filePath) {
	QString path =  filePath +".data/" + imageElement.attribute("src"); // the file is supposed to be in the data irectory
	QFileInfo fi(path);
	if(!fi.exists()) path = imageElement.attribute("src");
	int x = imageElement.attribute("topLeftX").toInt();
	int y = imageElement.attribute("topLeftY").toInt();
	//loadImageAtFrame( path, position );
	image = new QImage(path);
	if( !image->isNull() ) {
		boundaries = QRect( QPoint(x, y), image->size() );
	}
}



void BitmapImage::modification() {
}

bool BitmapImage::isModified() {
	return false;
}

void BitmapImage::setModified(bool) {
}

void BitmapImage::paintImage(QPainter &painter) {
	painter.drawImage(topLeft(), *image);
}

void outputImage(QImage* image, QSize size, QMatrix myView) {
}

BitmapImage BitmapImage::copy() {
	return BitmapImage(myParent, boundaries, QImage(*image));
}

BitmapImage BitmapImage::copy(QRect rectangle) {
	//QRect intersection = boundaries.intersected( rectangle );
	QRect intersection2  = rectangle.translated( -topLeft() );
	BitmapImage result = BitmapImage(myParent, rectangle, image->copy(intersection2));
	return result;
}

void BitmapImage::paste(BitmapImage* bitmapImage) {
	paste(bitmapImage, QPainter::CompositionMode_SourceOver);
}

void BitmapImage::paste(BitmapImage* bitmapImage, QPainter::CompositionMode cm) {
	QImage* image2 = bitmapImage->image;
	QRect newBoundaries;
	if( image->width() == 0 || image->height() == 0 ) {
		newBoundaries = bitmapImage->boundaries;
	} else {
		newBoundaries = boundaries.united( bitmapImage->boundaries );
	}
	extend( newBoundaries );
	QPainter painter(image);
	painter.setCompositionMode(cm);
	painter.drawImage( bitmapImage->boundaries.topLeft() - boundaries.topLeft(), *image2);
	painter.end();
}

void BitmapImage::add(BitmapImage* bitmapImage) {
	QImage* image2 = bitmapImage->image;
	QRect newBoundaries;
	if( image->width() == 0 || image->height() == 0 ) {
		newBoundaries = bitmapImage->boundaries;
	} else {
		newBoundaries = boundaries.united( bitmapImage->boundaries );
	}
	extend( newBoundaries );
	QPoint offset = bitmapImage->boundaries.topLeft() - boundaries.topLeft();
	for(int y=0; y<image2->height(); y++) {
		for(int x=0; x<image2->width(); x++) {
			/*QRgb p2 = image2->pixel(x,y);
			int r2 = qRed(p2);
			int g2 = qGreen(p2);
			int b2 = qBlue(p2);
			int a2 = qAlpha(p2);
			int r, g, b, a;
			r=0; g=0; b=0; a=0;
			for(int u=0; u<1; u++) {
				for(int v=0; v<1;v++) {
				  if(boundaries.contains(  bitmapImage->boundaries.topLeft() + QPoint(x+u,y+v) )) {
						QRgb p1  = image->pixel(offset.x()+x+u,offset.y()+y+v);
						int r1 = qRed(p1);
						int g1 = qGreen(p1);
						int b1 = qBlue(p1);
						int a1 = qAlpha(p1);
						r = r + r1;
						g = g + g1;
						b = b + b1;
						a = a + a1;
					}
				}
			}
			r = r/1;
			g = g/1;
			b = b/1;
			a = a/1;
			//r = 255;
			//g = 0;
			//b = 0;
			a = 255;
					QRgb p1  = image->pixel(offset.x()+x,offset.y()+y);
					int r1 = qRed(p1);
					int g1 = qGreen(p1);
					int b1 = qBlue(p1);
					int a1 = qAlpha(p1);
			r = (r1*(255-r2) + r2*r)/255;
			g = (g1*(255-g2) + g2*g)/255;
			b = (b1*(255-b2) + b2*b)/255;
			a = (a1*(255-a2) + a2*a)/255;*/
			QRgb p1  = image->pixel(offset.x()+x,offset.y()+y);
			QRgb p2 = image2->pixel(x,y);

			int a1 = qAlpha(p1); int a2 = qAlpha(p2);
			int r1 = qRed(p1);   int r2 = qRed(p2); // remember that the bitmap format is RGB32 Premultiplied
			int g1 = qGreen(p1); int g2 = qGreen(p2);
			int b1 = qBlue(p1);  int b2 = qBlue(p2);

			/*qreal a1 = qAlpha(p1); qreal a2 = qAlpha(p2);
			qreal r1 = qRed(p1);   qreal r2 = qRed(p2); // remember that the bitmap format is RGB32 Premultiplied
			qreal g1 = qGreen(p1); qreal g2 = qGreen(p2);
			qreal b1 = qBlue(p1);  qreal b2 = qBlue(p2);*/

			// unite
			int a = qMax(a1, a2);
			int r = qMax(r1, r2);
			int g = qMax(g1, g2);
			int b = qMax(b1, b2);

			// blend
			/*int a = a2 + a1*(255-a2)/255;
			int r = r2 + r1*(255-a2)/255;
			int g = g2 + g1*(255-a2)/255;
			int b = b2 + b1*(255-a2)/255;*/

			// source
			/*int a = a2;
			int r = r2;
			int g = g2;
			int b = b2;*/

			/*int a = qRound(a1+a2);
			int r = qRound((a1+a2)*((r1+0.)/a1+(r2+0.)/a2)/1);
			int g = qRound((a1+a2)*((g1+0.)/a1+(g2+0.)/a2)/1);
			int b = qRound((a1+a2)*((b1+0.)/a1+(b2+0.)/a2)/1);*/

			// add
			/*int a = qMin(255, qRound(1.0*(a1+a2)));
			int r = qMin(255, qRound(0.5*(r1+r2)));
			int g = qMin(255, qRound(0.5*(g1+g2)));
			int b = qMin(255, qRound(0.5*(b1+b2)));*/

			/*int a = qMin(255, qRound((1.0*a1+0.32*a2)));
			int r = qMin(255, qRound((1.0*r1+0.32*r2)));
			int g = qMin(255, qRound((1.0*g1+0.32*g2)));
			int b = qMin(255, qRound((1.0*b1+0.32*b2)));*/

			QRgb mix = qRgba(r, g, b, a);
			/*qDebug() << "------";
			qDebug() << r1 << g1 << b1 << a1;
			qDebug() << r2 << g2 << b2 << a2;
			qDebug() << r << g << b << a;
			qDebug() << qRed(mix) << qGreen(mix) << qBlue(mix) << qAlpha(mix);*/
			//QRgb mix = qRgba(r2, g2, b2, a);
			if(a2 != 0)
				image->setPixel(offset.x()+x,offset.y()+y, mix);
		}
	}
}

void BitmapImage::moveTopLeft(QPoint point) {
	boundaries.moveTopLeft(point);
}

void BitmapImage::transform(QRect newBoundaries, bool smoothTransform) {
	if(boundaries != newBoundaries) {
		boundaries = newBoundaries;
		newBoundaries.moveTopLeft( QPoint(0,0) );
		QImage* newImage = new QImage( boundaries.size(), QImage::Format_ARGB32_Premultiplied);
		//newImage->fill(QColor(255,255,255).rgb());
		QPainter painter(newImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, smoothTransform);
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.fillRect( newImage->rect(), QColor(0,0,0,0) );
		painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
		painter.drawImage(newBoundaries, *image );
		painter.end();
		//if(image != NULL) delete image;
		image = newImage;
	}
}

BitmapImage BitmapImage::transformed(QRect newBoundaries, bool smoothTransform) {
	BitmapImage transformedImage(NULL, newBoundaries, QColor(0,0,0,0));
	QPainter painter(transformedImage.image);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, smoothTransform);
	newBoundaries.moveTopLeft( QPoint(0,0) );
	painter.drawImage(newBoundaries, *image );
	painter.end();
	return transformedImage;
}


void BitmapImage::extend(QPoint P) {
	if(boundaries.contains( P )) {
		// nothing
	} else {
		extend( QRect(P, QSize(0,0)) ); // can we set QSize(0,0) ?
	}
}

void BitmapImage::extend(QRect rectangle) {
	if(rectangle.width() <= 0) rectangle.setWidth(1);
	if(rectangle.height() <= 0) rectangle.setHeight(1);
	if(boundaries.contains( rectangle )) {
		// nothing
	} else {
		QRect newBoundaries = boundaries.united(rectangle).normalized();
		QImage* newImage = new QImage( newBoundaries.size(), QImage::Format_ARGB32_Premultiplied);
		newImage->fill(qRgba(0,0,0,0));
		if(!newImage->isNull()) {
			QPainter painter(newImage);
			painter.drawImage(boundaries.topLeft() - newBoundaries.topLeft(), *image);
			painter.end();
		}
		if(image != NULL) delete image;
		image = newImage;
		boundaries = newBoundaries;
	}
}

QRgb BitmapImage::pixel(int x, int y) {
	return pixel( QPoint(x,y) );
}

QRgb BitmapImage::pixel(QPoint P) {
	QRgb result = qRgba(0,0,0,0); // black
	if( boundaries.contains( P ) ) result = image->pixel(P - topLeft());
	return result;
}

void BitmapImage::setPixel(int x, int y, QRgb colour) {
	setPixel( QPoint(x,y), colour);
}

void BitmapImage::setPixel(QPoint P, QRgb colour) {
	extend( P );
	image->setPixel(P-topLeft(), colour);
	//drawLine( QPointF(P), QPointF(P), QPen(QColor(colour)), QPainter::CompositionMode_SourceOver, false);
}


void BitmapImage::drawLine( QPointF P1, QPointF P2, QPen pen, QPainter::CompositionMode cm, bool antialiasing) {
	int width = pen.width();
	extend( QRect(P1.toPoint(), P2.toPoint()).normalized().adjusted(-width,-width,width,width) );
	if(image != NULL && !image->isNull() ) {
		QPainter painter(image);
		painter.setCompositionMode(cm);
		painter.setRenderHint(QPainter::Antialiasing, antialiasing);
		painter.setPen(pen);
		painter.drawLine( P1-topLeft(), P2-topLeft());
		painter.end();
	}
}

void BitmapImage::drawRect( QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing) {
	int width = pen.width();
	extend( rectangle.adjusted(-width,-width,width,width).toRect() );
	if(brush.style() == Qt::RadialGradientPattern) {
		QRadialGradient* gradient = (QRadialGradient*)brush.gradient();
		gradient->setCenter( gradient->center() - topLeft() );
		gradient->setFocalPoint( gradient->focalPoint() - topLeft() );
	}
	if(image != NULL && !image->isNull() ) {
		QPainter painter(image);
		painter.setCompositionMode(cm);
		painter.setRenderHint(QPainter::Antialiasing, antialiasing);
		painter.setPen(pen);
		painter.setBrush(brush);
		//painter.fillRect( rectangle.translated(-topLeft()), brush );
		//painter.fillRect( rectangle.translated(-topLeft()), QColor(255,0,0) );
		painter.drawRect( rectangle.translated(-topLeft()) );
		painter.end();
	}
}

void BitmapImage::drawEllipse( QRectF rectangle, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing) {
	int width = pen.width();
	extend( rectangle.adjusted(-width,-width,width,width).toRect() );
	if(image != NULL && !image->isNull() ) {
		QPainter painter(image);
		painter.setCompositionMode(cm);
		painter.setRenderHint(QPainter::Antialiasing, antialiasing);
		painter.setPen(pen);
		painter.setBrush(brush);
		//if(brush == Qt::NoBrush)
		painter.drawEllipse( rectangle.translated(-topLeft()) );
		painter.end();
	}
}

void BitmapImage::drawPath( QPainterPath path, QPen pen, QBrush brush, QPainter::CompositionMode cm, bool antialiasing) {
	int width = pen.width();
	extend( path.controlPointRect().adjusted(-width,-width,width,width).toRect() );
	if(image != NULL && !image->isNull() ) {
		QPainter painter(image);
		painter.setCompositionMode(cm);
		painter.setRenderHint(QPainter::Antialiasing, antialiasing);
		painter.setPen(pen);
		painter.setBrush(brush);
		painter.setWorldMatrix(QMatrix().translate(-topLeft().x(), -topLeft().y()));
		painter.setMatrixEnabled(true);
		painter.drawPath( path );
		painter.end();
	}
}


void BitmapImage::fastBlur(qreal rad) { // --- faster blur  -- doesn't work :-(
	if(image == NULL) return;
	int radius = qRound(rad);
	if (radius<1) return;
	/*
  //int[] pix=img.pixels;
  int w=image->width();
  int h=image->height();
  int wm=w-1;
  int hm=h-1;
  int wh=w*h;
  int div=radius+radius+1;

  //int r[]=new int[wh];
  //int g[]=new int[wh];
  //int b[]=new int[wh];
  QList<int> r, g, b;
	int rsum,gsum,bsum,x,y,i,yp,yi,yw;
	QRgb p;
  //int vmin[] = new int[max(w,h)];
	QList<int> vmin;
	
  int divsum=(div+1)>>1;
  divsum*=divsum;
  //int dv[]=new int[256*divsum];
	QList<int> dv;
  for (i=0;i<256*divsum;i++){
    //dv[i]=(i/divsum);
		dv << (i/divsum);
  }

  yw=yi=0;

  //int[][] stack=new int[div][3];
	QList<int> stack[3];
  int stackpointer;
  int stackstart;
  //int[] sir;
  QList<int> sir;
	int rbs;
  int r1=radius+1;
  int routsum,goutsum,boutsum;
  int rinsum,ginsum,binsum;

  for (y=0;y<h;y++){
    rinsum=ginsum=binsum=routsum=goutsum=boutsum=rsum=gsum=bsum=0;
    for(i=-radius;i<=radius;i++){
      sir=stack[i+radius];
      //p=pix[yi+min(wm,max(i,0))];
			//sir[0]=(p & 0xff0000)>>16;
      //sir[1]=(p & 0x00ff00)>>8;
      //sir[2]=(p & 0x0000ff);
      
			int truc = yi+min(wm,max(i,0));
			p = image->pixel( truc % w, truc / w );
			sir[0]=qRed(p);
			sir[1]=qGreen(p);
			sir[2]=qBlue(p);
			
      rbs=r1-abs(i);
      rsum+=sir[0]*rbs;
      gsum+=sir[1]*rbs;
      bsum+=sir[2]*rbs;
      if (i>0){
        rinsum+=sir[0];
        ginsum+=sir[1];
        binsum+=sir[2];
      } else {
        routsum+=sir[0];
        goutsum+=sir[1];
        boutsum+=sir[2];
      }
    }
    stackpointer=radius;

    for (x=0;x<w;x++){

      r[yi]=dv[rsum];
      g[yi]=dv[gsum];
      b[yi]=dv[bsum];
      
      rsum-=routsum;
      gsum-=goutsum;
      bsum-=boutsum;

      stackstart=stackpointer-radius+div;
      sir=stack[stackstart%div];
      
      routsum-=sir[0];
      goutsum-=sir[1];
      boutsum-=sir[2];
      
      if(y==0){
        vmin[x]=min(x+radius+1,wm);
      }
      //p=pix[yw+vmin[x]];
      //sir[0]=(p & 0xff0000)>>16;
      //sir[1]=(p & 0x00ff00)>>8;
      //sir[2]=(p & 0x0000ff);
			
			int truc = yw+vmin[x];
			p = image->pixel( truc % w, truc / w );
			sir[0]=qRed(p);
			sir[1]=qGreen(p);
			sir[2]=qBlue(p);
			
      rinsum+=sir[0];
      ginsum+=sir[1];
      binsum+=sir[2];

      rsum+=rinsum;
      gsum+=ginsum;
      bsum+=binsum;
      
      stackpointer=(stackpointer+1)%div;
      sir=stack[(stackpointer)%div];
     
      routsum+=sir[0];
      goutsum+=sir[1];
      boutsum+=sir[2];
     
       rinsum-=sir[0];
      ginsum-=sir[1];
      binsum-=sir[2];
     
       yi++;
    }
    yw+=w;
  }
  for (x=0;x<w;x++){
    rinsum=ginsum=binsum=routsum=goutsum=boutsum=rsum=gsum=bsum=0;
    yp=-radius*w;
    for(i=-radius;i<=radius;i++){
      yi=max(0,yp)+x;
     
       sir=stack[i+radius];
      
      sir[0]=r[yi];
      sir[1]=g[yi];
      sir[2]=b[yi];
     
      rbs=r1-abs(i);
      
      rsum+=r[yi]*rbs;
      gsum+=g[yi]*rbs;
      bsum+=b[yi]*rbs;
     
      if (i>0){
        rinsum+=sir[0];
        ginsum+=sir[1];
        binsum+=sir[2];
      } else {
        routsum+=sir[0];
        goutsum+=sir[1];
        boutsum+=sir[2];
      }
      
      if(i<hm){
        yp+=w;
      }
    }
    yi=x;
    stackpointer=radius;
    for (y=0;y<h;y++){
      //pix[yi]=0xff000000 | (dv[rsum]<<16) | (dv[gsum]<<8) | dv[bsum];
			image->setPixel(yi % w, yi/w, qRgb(dv[rsum], dv[gsum], dv[bsum]) );
			
      rsum-=routsum;
      gsum-=goutsum;
      bsum-=boutsum;

      stackstart=stackpointer-radius+div;
      sir=stack[stackstart%div];
     
      routsum-=sir[0];
      goutsum-=sir[1];
      boutsum-=sir[2];
     
       if(x==0){
        vmin[y]=min(y+r1,hm)*w;
      }
      p=x+vmin[y];
      
      sir[0]=r[p];
      sir[1]=g[p];
      sir[2]=b[p];
      
      rinsum+=sir[0];
      ginsum+=sir[1];
      binsum+=sir[2];

      rsum+=rinsum;
      gsum+=ginsum;
      bsum+=binsum;

      stackpointer=(stackpointer+1)%div;
      sir=stack[stackpointer];
     
      routsum+=sir[0];
      goutsum+=sir[1];
      boutsum+=sir[2];
      
      rinsum-=sir[0];
      ginsum-=sir[1];
      binsum-=sir[2];

      yi+=w;
    }
  }*/
}


void BitmapImage::blur2(qreal radius) { // --- faster blur  -- doesn't work :-(
	if(image == NULL) return;
	int rad = qRound(radius);
	
	int w = image->width();
	int h = image->height();
	int w2 = w + 2*rad;
	int h2 = h + 2*rad;
	QImage *newImage = new QImage(w2, h2, QImage::Format_ARGB32_Premultiplied);
	newImage->fill( qRgba(0,0,0,0) );
	
	/*QImage *sourceImage = new QImage(image->width()+4*rad, image->height()+4*rad, QImage::Format_ARGB32_Premultiplied);
	sourceImage->fill( qRgba(0,0,0,0) );
	QPainter painter(sourceImage);
	painter.drawImage( QPoint(2*rad,2*rad), *image );
	//painter.setBrush( QBrush( QPixmap(":backgrounds/dots.png") ) );
	//painter.drawRect( QRect(2*rad,2*rad, image->width(), image->height()) );
	painter.end();
	painter.begin(newImage);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.setRenderHints(QPainter::SmoothPixmapTransform, true);
	painter.setRenderHints(QPainter::Antialiasing, true);
	*/
	for(int i=0; i<w; i++) {
		for(int j=0; j<h; j++) {
			//painter.drawImage( QRect(i,j,1,1), *image, QRect(i-rad,j-rad, rad, rad));
			int sum = 0;
			for(int i2=qMax(0,i-rad-rad); i2<=qMin(w,i); i2++) {
				for(int j2=qMax(0,j-rad-rad); j2<=qMin(h,j); j2++) {
					sum += qAlpha( image->pixel(i2, j2) );
				}
			}
			sum = sum/(4*rad*rad);
			newImage->setPixel( i, j, qRgba(20,20,20, sum) );
			//painter.drawImage( QRect(i,j,1,1), *sourceImage, QRect(i,j, 2*rad, 2*rad));
		}
	}
	delete image;
	image = newImage;
	boundaries.adjust(-rad, -rad, rad, rad);
}

void BitmapImage::blur(qreal radius) {  // please someone implement a fast gaussian blur
	if(image == NULL) return;
	int rad = qRound(radius);
	//extend( boundaries.adjusted(-rad, -rad, rad, rad) );
	boundaries.adjust(-rad, -rad, rad, rad);

	QList<int> gaussian;
	int sum = 0;
	for(int u=0; u<rad; u++) {
		int value = qRound(255*exp(-4*(u/radius)*(u/radius)));
		//int value = 1;
		gaussian << value;
		sum = sum + value;
	}
	sum = 2*sum;
	//qDebug() << gaussian;
	//qDebug() << sum;

	QImage* newImage;
	// --- x blur
	newImage = new QImage(image->width()+2*rad, image->height()+2*rad, QImage::Format_ARGB32_Premultiplied);
	newImage->fill( qRgba(0,0,0,0) );
	for(int i=0; i<newImage->width(); i++) {
		for(int j=0; j<newImage->height(); j++) {
			int r=0; int g = 0; int b = 0; int a = 0;
			for(int u=-rad+1; u<rad; u++) {
				//qDebug() << i << j << u << value;
				//qDebug() << "poppp" << qAlpha(pixel(i+u,j));
				QRgb pix = qRgba(0,0,0,0);
				if(i-rad+u>-1 && i-rad+u<image->width() && j-rad>-1 && j-rad<image->height()) pix = image->pixel(i-rad+u,j-rad);
				r = r + gaussian.at(abs(u))*qRed(pix);
				g = g + gaussian.at(abs(u))*qGreen(pix);
				b = b + gaussian.at(abs(u))*qBlue(pix);
				a = a + gaussian.at(abs(u))*qAlpha(pix);
			}
			r = r/sum;
			g = g/sum;
			b = b/sum;
			a = a/sum;
			//qDebug() << qRound(value/sum);
			//int alpha = 255;
			newImage->setPixel(QPoint(i,j), qRgba(r,g,b,a) );
		}
	}
	delete image;
	image = newImage;

	// --- y blur
	newImage = new QImage(image->size(), QImage::Format_ARGB32_Premultiplied);
	newImage->fill( qRgba(0,0,0,0) );
	for(int i=0; i<newImage->width(); i++) {
		for(int j=0; j<newImage->height(); j++) {
			int r=0; int g = 0; int b = 0; int a = 0;
			for(int u=-rad+1; u<rad; u++) {
				//qDebug() << i << j << u << value;
				//qDebug() << "poppp" << qAlpha(pixel(i+u,j));
				QRgb pix = qRgba(0,0,0,0);
				if(j+u>-1 && j+u<image->height()) pix = image->pixel(i,j+u);
				r = r + gaussian.at(abs(u))*qRed(pix);
				g = g + gaussian.at(abs(u))*qGreen(pix);
				b = b + gaussian.at(abs(u))*qBlue(pix);
				a = a + gaussian.at(abs(u))*qAlpha(pix);
			}
			r = r/sum;
			g = g/sum;
			b = b/sum;
			a = a/sum;
			//qDebug() << qRound(value/sum);
			//int alpha = 255;
			newImage->setPixel(QPoint(i,j), qRgba(r,g,b,a));
		}
	}

	delete image;
	image = newImage;
}

void BitmapImage::clear() {
	if(image != NULL) delete image;
	image = new QImage(0, 0, QImage::Format_ARGB32_Premultiplied);
	boundaries = QRect(0,0,0,0);
}

void BitmapImage::clear(QRect rectangle) {
	QRect clearRectangle = boundaries.intersected( rectangle );
	clearRectangle.moveTopLeft( rectangle.topLeft() - topLeft() );
	QPainter painter(image);
	painter.setCompositionMode(QPainter::CompositionMode_Clear);
	painter.fillRect( clearRectangle, QColor(0,0,0,0) );
	painter.end();
}

int BitmapImage::sqr(int n) { // square of a number
	return n*n;
}

int BitmapImage::rgbDistance(QRgb rgba1, QRgb rgba2) {
	int result = sqr(qRed(rgba1)-qRed(rgba2)) + sqr(qGreen(rgba1)-qGreen(rgba2)) + sqr(qBlue(rgba1)-qBlue(rgba2)) + sqr(qAlpha(rgba1)-qAlpha(rgba2));
	return result;
}

void BitmapImage::floodFill(BitmapImage* targetImage, BitmapImage* fillImage, QPoint point, QRgb targetColour, QRgb replacementColour, int tolerance) {
	QList<QPoint> queue; // queue all the pixels of the filled area (as they are found)
	int j, k; bool condition;
	BitmapImage* replaceImage = new BitmapImage(NULL, targetImage->boundaries.united(fillImage->boundaries), QColor(0,0,0,0));
	//QPainter painter1(replaceImage->image);
	//QPainter painter2(fillImage->image);
	//painter1.setPen( QColor(replacementColour) );
	QPen myPen;
	myPen = QPen( QColor(replacementColour) , 1.0, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin);

	targetColour = targetImage->pixel(point.x(), point.y());
	//if(  rgbDistance(targetImage->pixel(point.x(), point.y()), targetColour) > tolerance ) return;
	queue.append( point );
	// ----- flood fill
	// ----- from the standard flood fill algorithm
	// ----- http://en.wikipedia.org/wiki/Flood_fill
	j = -1; k = 1;
	for(int i=0; i< queue.size(); i++ ) {
		point = queue.at(i);
		if(  replaceImage->pixel(point.x(), point.y()) != replacementColour  && rgbDistance(targetImage->pixel(point.x(), point.y()), targetColour) < tolerance ) {
			j = -1; condition =  (point.x() + j > targetImage->left());
			while( replaceImage->pixel(point.x()+j, point.y()) != replacementColour  && rgbDistance(targetImage->pixel( point.x()+j, point.y() ), targetColour) < tolerance && condition) {
				j = j - 1;
				condition =  (point.x() + j > targetImage->left());
			}

			k = 1; condition = ( point.x() + k < targetImage->right()-1);
			while( replaceImage->pixel(point.x()+k, point.y()) != replacementColour  && rgbDistance(targetImage->pixel( point.x()+k, point.y() ), targetColour) < tolerance && condition) {
				k = k + 1;
				condition = ( point.x() + k < targetImage->right()-1);
			}

			//painter1.drawLine( point.x()+j, point.y(), point.x()+k+1, point.y() );

			replaceImage->drawLine( QPointF(point.x()+j, point.y()), QPointF(point.x()+k, point.y()), myPen, QPainter::CompositionMode_SourceOver, false);
			//for(int l=0; l<=k-j+1 ; l++) {
			//	replaceImage->setPixel( point.x()+j, point.y(), replacementColour );
			//}

			for(int x = j+1; x < k; x++) {
				//replaceImage->setPixel( point.x()+x, point.y(), replacementColour);
				if(point.y() - 1 > targetImage->top() && queue.size() < targetImage->height() * targetImage->width() ) {
					if( replaceImage->pixel(point.x()+x, point.y()-1) != replacementColour) {
						if(rgbDistance(targetImage->pixel( point.x()+x, point.y() - 1), targetColour) < tolerance) {
							queue.append( point + QPoint(x,-1) );
						} else {
							replaceImage->setPixel( point.x()+x, point.y()-1, replacementColour);
						}
					}
				}
				if(point.y() + 1 < targetImage->bottom() && queue.size() < targetImage->height() * targetImage->width() ) {
					if( replaceImage->pixel(point.x()+x, point.y()+1) != replacementColour) {
						if(rgbDistance(targetImage->pixel( point.x()+x, point.y() + 1), targetColour) < tolerance) {
							queue.append( point + QPoint(x, 1) );
						} else {
							replaceImage->setPixel( point.x()+x, point.y()+1, replacementColour);
						}
					}
				}
			}
		}
	}
	//painter2.drawImage( QPoint(0,0), *replaceImage );
	fillImage->paste(replaceImage);
	//replaceImage->fill(qRgba(0,0,0,0));
	//painter1.end();
	//painter2.end();
	delete replaceImage;
	//update();
}

