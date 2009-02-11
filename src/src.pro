CONFIG += qt release console
TEMPLATE = app
TARGET = Pencil
DEPENDPATH += . ../release
OBJECTS_DIR = ../release/build
MOC_DIR = ../release/build
RCC_DIR = ../release/build
DESTDIR = ../release
INCLUDEPATH += . 	 \
			external/linux \
			external/macosx \
			external/win32 \
			interface \
			graphics/bitmap \
			graphics/vector \
			structure
# Input
HEADERS += 	interfaces.h \
			interface/editor.h \
			interface/mainwindow.h \
			interface/palette.h \
			interface/preferences.h \
			interface/scribblearea.h \
			interface/timeline.h \
			interface/timecontrols.h \
			interface/toolset.h
SOURCES += 	interface/editor.cpp \
			interface/mainwindow.cpp \
			interface/palette.cpp \
			interface/preferences.cpp \
			interface/scribblearea.cpp \
			interface/timeline.cpp \
			interface/timecontrols.cpp \
			interface/toolset.cpp \
			main.cpp
LIBS +=	-L../release -lpencil_structure
win32 {
	INCLUDEPATH += . ../libwin32
	SOURCES += external/win32/win32.cpp
	#LIBS += -Llibwin32 -lming -lpng
	LIBS += -Llibwin32
	RC_FILE = ../pencil.rc
}
macx {
	INCLUDEPATH += . ../libmacosx
	HEADERS += external/macosx/style.h
	SOURCES += external/macosx/macosx.cpp \
	 	   external/macosx/style.cpp
	#LIBS += -Llibmacosx -lming
	LIBS += -Llibmacosx
	RC_FILE = ../pencil.icns
}
linux-g++ {
	INCLUDEPATH += . ../liblinux
	SOURCES += external/linux/linux.cpp
	#LIBS += -Lliblinux -lming -lpng
	LIBS += -Lliblinux
}
RESOURCES += ../pencil.qrc
QT += xml opengl
