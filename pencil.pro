CONFIG += qt release console
TEMPLATE = app
TARGET = Pencil
DEPENDPATH += src release
OBJECTS_DIR = release/build
MOC_DIR = release/build
RCC_DIR = release/build
DESTDIR = release
INCLUDEPATH +=	src \
		   src/external/linux \
		   src/external/macosx \
		   src/external/win32 \
		   src/interface \
		   src/graphics/bitmap \
		   src/graphics/vector \
		   src/structure
# Input
HEADERS += 	src/graphics/vector/bezierarea.h \
		   src/graphics/vector/gradient.h \
		   src/interfaces.h \
		   src/interface/editor.h \
		   src/interface/mainwindow.h \
		   src/interface/palette.h \
		   src/interface/preferences.h \
		   src/interface/scribblearea.h \
		   src/interface/timecontrols.h \
		   src/interface/timeline.h \
		   src/interface/toolset.h \
		   src/structure/layer.h \
		   src/structure/layerbitmap.h \
		   src/structure/layercamera.h \
		   src/structure/layerimage.h \
		   src/structure/layervector.h \
		   src/structure/layersound.h \
		   src/structure/object.h
SOURCES += 	src/graphics/bitmap/bitmapimage.cpp \
		   src/graphics/bitmap/blur.cpp \
		   src/graphics/vector/bezierarea.cpp \
		   src/graphics/vector/beziercurve.cpp \
		   src/graphics/vector/colourref.cpp \
		   src/graphics/vector/gradient.cpp \
		   src/graphics/vector/vectorimage.cpp \
		   src/graphics/vector/vertexref.cpp \
		   src/interface/editor.cpp \
		   src/interface/mainwindow.cpp \
		   src/interface/palette.cpp \
		   src/interface/preferences.cpp \
		   src/interface/scribblearea.cpp \
		   src/interface/timecontrols.cpp \
		   src/interface/timeline.cpp \
		   src/interface/toolset.cpp \
		   src/structure/layer.cpp \
		   src/structure/layerimage.cpp \
		   src/structure/layerbitmap.cpp \
		   src/structure/layervector.cpp \
		   src/structure/layercamera.cpp \
		   src/structure/layersound.cpp \
		   src/structure/object.cpp \
		   src/main.cpp
win32 {
	INCLUDEPATH += src libwin32
	SOURCES += src/external/win32/win32.cpp
	#LIBS += -Llibwin32 -lming -lpng
	LIBS += -Llibwin32
	RC_FILE = pencil.rc
}
macx {
	INCLUDEPATH += src libmacosx
	HEADERS += src/external/macosx/style.h
	SOURCES += src/external/macosx/macosx.cpp \
	 	   src/external/macosx/style.cpp
	#LIBS += -Llibmacosx -lming
	LIBS += -Llibmacosx
	RC_FILE = pencil.icns
}
linux-g++ {
	INCLUDEPATH += src /liblinux
	SOURCES += src/external/linux/linux.cpp
	#LIBS += -Lliblinux -lming -lpng
	LIBS += -Lliblinux
}
RESOURCES += pencil.qrc
QT += xml opengl phonon
