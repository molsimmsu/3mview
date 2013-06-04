SET(MOD_CORE_MODULECLASS ElectronDensityModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/processors/addvolumetocollection.cpp
    ${MOD_DIR}/processors/checklistvolumeselector.cpp
    ${MOD_DIR}/processors/correlation.cpp
    ${MOD_DIR}/processors/densitymapcollectionsource.cpp
    ${MOD_DIR}/processors/densitymapcoprocessor.cpp
    ${MOD_DIR}/processors/densitymapmanipulation.cpp
    ${MOD_DIR}/processors/densitymapraycaster.cpp
    ${MOD_DIR}/processors/densitymapsaver.cpp
    ${MOD_DIR}/processors/multivolumeselector.cpp
    ${MOD_DIR}/processors/pdbtoedm.cpp
    ${MOD_DIR}/processors/segmentation.cpp
    ${MOD_DIR}/processors/segmentationsplit.cpp
    ${MOD_DIR}/processors/transferfunctioneditor.cpp
    
    #${MOD_DIR}/processors/score.cpp
    
    ${MOD_DIR}/io/mrcvolumereader.cpp
    
    ${MOD_DIR}/properties/transfunclistproperty.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/processors/addvolumetocollection.h
    ${MOD_DIR}/processors/checklistvolumeselector.h
    ${MOD_DIR}/processors/correlation.h
    ${MOD_DIR}/processors/densitymapcollectionsource.h
    ${MOD_DIR}/processors/densitymapcoprocessor.h
    ${MOD_DIR}/processors/densitymapmanipulation.h
    ${MOD_DIR}/processors/densitymapraycaster.h
    ${MOD_DIR}/processors/densitymapsaver.h
    ${MOD_DIR}/processors/multivolumeselector.h
    ${MOD_DIR}/processors/pdbtoedm.h
    ${MOD_DIR}/processors/segmentation.h
    ${MOD_DIR}/processors/segmentationsplit.h
    ${MOD_DIR}/processors/transferfunctioneditor.h
    
    #${MOD_DIR}/processors/score.h
    
    ${MOD_DIR}/io/mrcvolumereader.h
    
    ${MOD_DIR}/properties/transfunclistproperty.h
)

SET(MOD_QT_MODULECLASS ElectronDensityModuleQt)

SET(MOD_QT_SOURCES
    ${MOD_DIR}/qt/electrondensitypropertywidgetfactory.cpp
    ${MOD_DIR}/qt/transfunclistpropertywidget.cpp
)
    
SET(MOD_QT_HEADERS
    ${MOD_DIR}/qt/transfunclistpropertywidget.h
)

SET(MOD_QT_HEADERS_NONMOC
    ${MOD_DIR}/qt/electrondensitypropertywidgetfactory.h
)
