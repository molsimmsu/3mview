QT += gui \
    opengl
TARGET = 3mview
TEMPLATE = app
CONFIG += -static
SOURCES += main.cpp \
    DataLoaderWidget.cpp \
    MainWindow.cpp \
    StrideReader.cpp \
    Stereo.cpp \
    ModulesWidget.cpp \
    DensityMap/DensityMap.cpp \
    DensityMap/DensityMapList.cpp \
    DensityMap/DensityMapListWidget.cpp \
    DensityMap/DensityMapSegmentWidget.cpp \
    DensityMap/DensityMapWidget.cpp \
    Molecule/Molecule.cpp \
    Molecule/MoleculeList.cpp \
    Molecule/MoleculeListWidget.cpp \
    DensityMap/DensityMapSegment.cpp \
    DensityMap/Segment/ThresholdDivision.cpp \
    DataObject.cpp \
    ObjectDispatcher.cpp \
    RGBColor.cpp \
    ObjectScene.cpp

HEADERS += OpenStructure.h \
    DataLoaderWidget.h \
    MainWindow.h \
    StrideReader.h \
    Stereo.h \
    ModulesWidget.h \
    DensityMap/DensityMapWidget.h \
    DensityMap/DensityMapListWidget.h \
    DensityMap/DensityMapList.h \
    DensityMap/DensityMap.h \
    Molecule/MoleculeListWidget.h \
    Molecule/MoleculeList.h \
    Molecule/Molecule.h \
    DensityMap/DensityMapSegmentWidget.h \
    DensityMap/DensityMapSegment.h \
    DensityMap/Segment/ThresholdDivision.h \
    DensityMap/Segment/SegmentAlgorithm.h \
    DataObject.h \
    ObjectDispatcher.h \
    RGBColor.h \
    ObjectScene.h
INCLUDEPATH += openbabel/include \
    ost/stage/include
LIBS += -Lopenbabel/lib \
    -Lost/stage/lib \
    -Lost/stage/lib64 \
    -lopenbabel \
    -lboost_system \
    -lboost_filesystem \
    -lost_gui \
    -lost_gfx \
    -lost_mol \
    -lost_base \
    -lost_io \
    -lost_img \
    -lost_img_alg

QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64

