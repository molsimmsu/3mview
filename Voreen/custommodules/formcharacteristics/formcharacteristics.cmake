# module class must reside in sample/samplemodule.h + sample/samplemodule.cpp
SET(MOD_CORE_MODULECLASS FormCharacteristicsModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/datastructures/pointcloud.cpp
    ${MOD_DIR}/processors/calculatescore.cpp
    ${MOD_DIR}/processors/volvolalign.cpp
    ${MOD_DIR}/processors/molmolalign.cpp
    ${MOD_DIR}/processors/formfinder.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/datastructures/pointcloud.h
    ${MOD_DIR}/processors/calculatescore.h
    ${MOD_DIR}/processors/volvolalign.h
    ${MOD_DIR}/processors/molmolalign.h
    ${MOD_DIR}/processors/formfinder.h
)   
