TEMPLATE      = lib
CONFIG       += plugin release
INCLUDEPATH  += ../.. \
				../../structure \
				../../graphics/bitmap \
				../../graphics/vector
HEADERS       = imageplugin.h
SOURCES       = imageplugin.cpp
OBJECTS_DIR	  = ../../../release/build
MOC_DIR       = ../../../release/build
RCC_DIR       = ../../../release/build
LIBS += -L../../../release -lpencil_structure
win32 {
	INCLUDEPATH += . libwin32
	LIBS += -Llibwin32
}
macx {
	INCLUDEPATH += . libmacosx
	LIBS += -Llibmacosx
}
linux-g++ {
	INCLUDEPATH += . liblinux
	LIBS += -Lliblinux
}
TARGET        = $$qtLibraryTarget(pencil_imageplugin)
DESTDIR       = ../../../release/plugins
QT					 += xml 