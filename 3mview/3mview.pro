QT += gui \
    opengl
TARGET = 3mview
TEMPLATE = app
CONFIG += -static
SOURCES += main.cpp \
    DataLoaderWidget.cpp \
    MoleculeListWidget.cpp \
    MainWindow.cpp \
    MoleculeList.cpp \
    Molecule.cpp \
    DensityMapList.cpp \
    DensityMap.cpp \
    DensityMapListWidget.cpp \
    DensityMapWidget.cpp \
    StrideReader.cpp
HEADERS += OpenStructure.h \
    DataLoaderWidget.h \
    MoleculeListWidget.h \
    MainWindow.h \
    MoleculeList.h \
    Molecule.h \
    DensityMapList.h \
    DensityMap.h \
    DensityMapListWidget.h \
    DensityMapWidget.h \
    StrideReader.h
INCLUDEPATH += /usr/local/include/openbabel-2.0
LIBS += -lopenbabel \
    -lboost_system \
    -lboost_filesystem \
    -lost_gui \
    -lost_gfx \
    -lost_mol \
    -lost_base \
    -lost_io \
    -lost_img \
    -lost_img_alg
