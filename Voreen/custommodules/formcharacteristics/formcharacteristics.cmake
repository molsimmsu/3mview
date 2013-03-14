# module class must reside in sample/samplemodule.h + sample/samplemodule.cpp
SET(MOD_CORE_MODULECLASS FormCharacteristicsModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/ports/matrix4doubleport.cpp
    ${MOD_DIR}/processors/alignbymoments.cpp
    ${MOD_DIR}/processors/databasematch.cpp
    ${MOD_DIR}/processors/calculatescore.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/ports/matrix4doubleport.h
    ${MOD_DIR}/processors/alignbymoments.h
    ${MOD_DIR}/processors/databasematch.h
    ${MOD_DIR}/processors/calculatescore.h
)   
