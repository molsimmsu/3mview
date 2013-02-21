SET(MOD_CORE_MODULECLASS ElectronDensityModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/processors/segmentationbase.cpp
    ${MOD_DIR}/processors/manipulationbase.cpp
    ${MOD_DIR}/processors/densitymapcollectionsource.cpp
    ${MOD_DIR}/io/mrcvolumereader.cpp
    ${MOD_DIR}/properties/volumeselectionproperty.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/processors/segmentationbase.h
    ${MOD_DIR}/processors/manipulationbase.h
    ${MOD_DIR}/processors/densitymapcollectionsource.h
    ${MOD_DIR}/io/mrcvolumereader.h
    ${MOD_DIR}/properties/volumeselectionproperty.h
)

SET(MOD_QT_MODULECLASS ElectronDensityModuleQt)

SET(MOD_QT_SOURCES
    #${MOD_DIR}/qt/electrondensitypropertywidgetfactory.cpp
    #${MOD_DIR}/qt/volumeselectionpropertywidget.cpp
)
    
SET(MOD_QT_HEADERS
    #${MOD_DIR}/qt/volumeselectionpropertywidget.h
)

SET(MOD_QT_HEADERS_NONMOC
    #${MOD_DIR}/qt/electrondensitypropertywidgetfactory.h
)
