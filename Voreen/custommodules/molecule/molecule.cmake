
# module class must reside in sample/samplemodule.h + sample/samplemodule.cpp
SET(MOD_CORE_MODULECLASS MoleculeModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/datastructures/molecule.cpp
    ${MOD_DIR}/ports/moleculeport.cpp
    ${MOD_DIR}/processors/moleculesource.cpp
    ${MOD_DIR}/processors/moleculetransformer.cpp
    ${MOD_DIR}/processors/moleculegeometrybuilder.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/datastructures/molecule.h
    ${MOD_DIR}/ports/moleculeport.h
    ${MOD_DIR}/processors/moleculesource.h
    ${MOD_DIR}/processors/moleculetransformer.h
    ${MOD_DIR}/processors/moleculegeometrybuilder.h
)   
