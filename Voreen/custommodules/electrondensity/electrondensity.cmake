
# module class must reside in sample/samplemodule.h + sample/samplemodule.cpp
SET(MOD_CORE_MODULECLASS ElectronDensityModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/processors/segmentationbase.cpp
    ${MOD_DIR}/io/mrcvolumereader.cpp
    ${MOD_DIR}/io//vmd/ccp4plugin.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/processors/segmentationbase.h
    ${MOD_DIR}/io/mrcvolumereader.h
)   
