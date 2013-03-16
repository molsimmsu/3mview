SET(MOD_CORE_MODULECLASS HomologyModule)

SET(MOD_CORE_SOURCES
    ${MOD_DIR}/processors/domaindatabaseadministration.cpp
    ${MOD_DIR}/processors/homologyfinder.cpp
    ${MOD_DIR}/properties/alignmentlistproperty.cpp
)

SET(MOD_CORE_HEADERS
    ${MOD_DIR}/processors/domaindatabaseadministration.h
    ${MOD_DIR}/processors/homologyfinder.h
    ${MOD_DIR}/properties/alignmentlistproperty.h
)
