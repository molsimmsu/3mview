
# module class must reside in sample/samplemodule.h + sample/samplemodule.cpp
SET(MOD_CORE_MODULECLASS GeometryModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/processors/primitivegeometrysource.cpp
    ${MOD_DIR}/utils/primitivegeometrybuilder.cpp
    ${MOD_DIR}/datastructures/polyline.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/processors/primitivegeometrysource.h
    ${MOD_DIR}/utils/primitivegeometrybuilder.h
    ${MOD_DIR}/datastructures/polyline.h
)   

SET(MOD_QT_MODULECLASS GeometryModuleQt)

SET(MOD_QT_SOURCES
    ${MOD_DIR}/processors/manipulationbase.cpp
)
    
SET(MOD_QT_HEADERS

)

SET(MOD_QT_HEADERS_NONMOC
    ${MOD_DIR}/processors/manipulationbase.cpp
)
