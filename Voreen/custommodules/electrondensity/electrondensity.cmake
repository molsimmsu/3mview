SET(MOD_CORE_MODULECLASS ElectronDensityModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/processors/segmentation.cpp
    ${MOD_DIR}/processors/densitymapcoprocessor.cpp
    ${MOD_DIR}/processors/densitymapmanipulation.cpp
    ${MOD_DIR}/processors/densitymapcollectionsource.cpp
    ${MOD_DIR}/processors/pdbtoedm.cpp
    ${MOD_DIR}/io/mrcvolumereader.cpp
    ${MOD_DIR}/properties/volumeselectionproperty.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/processors/segmentation.h
    ${MOD_DIR}/processors/densitymapcoprocessor.h
    ${MOD_DIR}/processors/densitymapmanipulation.h
    ${MOD_DIR}/processors/densitymapcollectionsource.h
    ${MOD_DIR}/processors/pdbtoedm.h
    ${MOD_DIR}/io/mrcvolumereader.h
    ${MOD_DIR}/properties/volumeselectionproperty.h
)
