SET(MOD_CORE_MODULECLASS SystemModules)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/processors/qualitymodule.cpp
    ${MOD_DIR}/processors/adminmodule.cpp
    )

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/processors/qualitymodule.h
    ${MOD_DIR}/processors/adminmodule.h
    )
