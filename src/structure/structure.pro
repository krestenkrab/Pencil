TEMPLATE      = lib
CONFIG       += dll release
INCLUDEPATH  += . \
				../graphics/bitmap \
				../graphics/vector
HEADERS       = object.h \
				layer.h \
				layerimage.h \
				layerbitmap.h \
				layervector.h \
				layercamera.h \
				layersound.h \
				../graphics/bitmap/bitmapimage.h \
				../graphics/bitmap/blur.h \
				../graphics/vector/vectorimage.h \
				../graphics/vector/vertexref.h \
				../graphics/vector/colourref.h \
				../graphics/vector/beziercurve.h \
				../graphics/vector/bezierarea.h \
				../graphics/vector/gradient.h
SOURCES       = object.cpp \
				layer.cpp \
				layerimage.cpp \
				layerbitmap.cpp \
				layervector.cpp \
				layercamera.cpp \
				layersound.cpp \
				../graphics/bitmap/bitmapimage.cpp \
				../graphics/bitmap/blur.cpp \
				../graphics/vector/vectorimage.cpp \
				../graphics/vector/vertexref.cpp \
				../graphics/vector/colourref.cpp \
				../graphics/vector/beziercurve.cpp \
				../graphics/vector/bezierarea.cpp \
				../graphics/vector/gradient.cpp
OBJECTS_DIR	  = ../../release/build
MOC_DIR       = ../../release/build
RCC_DIR       = ../../release/build
TARGET        = pencil_structure
DESTDIR       = ../../release
QT			 += xml 