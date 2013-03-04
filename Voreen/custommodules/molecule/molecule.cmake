
# module class must reside in sample/samplemodule.h + sample/samplemodule.cpp
SET(MOD_CORE_MODULECLASS MoleculeModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/datastructures/molecule.cpp
    ${MOD_DIR}/datastructures/moleculecollection.cpp
    ${MOD_DIR}/ports/moleculeport.cpp
    ${MOD_DIR}/ports/moleculecollectionport.cpp
    ${MOD_DIR}/processors/moleculesource.cpp
    ${MOD_DIR}/processors/moleculeselector.cpp
    ${MOD_DIR}/processors/moleculecollectionsource.cpp
    ${MOD_DIR}/processors/moleculetransformer.cpp
    ${MOD_DIR}/processors/moleculegeometrybuilder.cpp
    ${MOD_DIR}/properties/moleculeselectionproperty.cpp
    ${MOD_DIR}/utils/stridereader.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/datastructures/molecule.h
    ${MOD_DIR}/datastructures/moleculecollection.h
    ${MOD_DIR}/ports/moleculeport.h
    ${MOD_DIR}/ports/moleculecollectionport.h
    ${MOD_DIR}/processors/moleculesource.h
    ${MOD_DIR}/processors/moleculeselector.h
    ${MOD_DIR}/processors/moleculecollectionsource.h
    ${MOD_DIR}/processors/moleculetransformer.h
    ${MOD_DIR}/processors/moleculegeometrybuilder.h
    ${MOD_DIR}/properties/moleculeselectionproperty.h
    ${MOD_DIR}/utils/stridereader.h
)   
