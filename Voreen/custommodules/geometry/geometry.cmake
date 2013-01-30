
# module class must reside in sample/samplemodule.h + sample/samplemodule.cpp
SET(MOD_CORE_MODULECLASS GeometryModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/processors/primitivegeometrysource.cpp
    ${MOD_DIR}/utils/primitivegeometrybuilder.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/processors/primitivegeometrysource.h
    ${MOD_DIR}/utils/primitivegeometrybuilder.h
)   
