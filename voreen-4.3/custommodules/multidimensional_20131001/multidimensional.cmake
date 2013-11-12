SET(MOD_CORE_MODULECLASS MultidimensionalModule)

SET(MOD_CORE_SOURCES
    #Processors        
    ${MOD_DIR}/processors/emptyvolumesource.cpp
    ${MOD_DIR}/processors/hypercubebuilder.cpp
    ${MOD_DIR}/utils/primitivegeometrybuilder.cpp
)

SET(MOD_CORE_HEADERS
    #Processors    
    ${MOD_DIR}/processors/emptyvolumesource.h
    ${MOD_DIR}/processors/hypercubebuilder.h
    ${MOD_DIR}/utils/primitivegeometrybuilder.h
)

