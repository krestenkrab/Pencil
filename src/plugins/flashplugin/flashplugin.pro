TEMPLATE      = lib
CONFIG       += plugin release
INCLUDEPATH  += ../.. \
				../../structure \
				../../graphics/bitmap \
				../../graphics/vector
HEADERS       = flashplugin.h
SOURCES       = flashplugin.cpp
OBJECTS_DIR	  = ../../../release/build
MOC_DIR       = ../../../release/build
RCC_DIR       = ../../../release/build
LIBS += -L../../../release -lpencil_structure
win32 {
	INCLUDEPATH += . libwin32
	LIBS += -Llibwin32 -lming -lpng
}
macx {
	INCLUDEPATH += . libmacosx
	LIBS += -Llibmacosx -lming
}
linux-g++ {
	INCLUDEPATH += . liblinux
	LIBS += -Lliblinux -lming
}
TARGET        = $$qtLibraryTarget(pencil_flashplugin)
DESTDIR       = ../../../release/plugins
QT					 += xml 