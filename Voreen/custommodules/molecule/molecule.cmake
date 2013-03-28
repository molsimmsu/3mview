
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
    ${MOD_DIR}/processors/moleculegeometrybuilder.cpp
    ${MOD_DIR}/processors/moleculecollectiongeometrybuilder.cpp
    ${MOD_DIR}/processors/moleculecoprocessor.cpp
    ${MOD_DIR}/processors/moleculemanipulation.cpp
    ${MOD_DIR}/properties/moleculeselectionproperty.cpp
    ${MOD_DIR}/properties/moleculeurllistproperty.cpp
    ${MOD_DIR}/utils/stridereader.cpp
    ${MOD_DIR}/utils/moleculeio.cpp
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
    ${MOD_DIR}/processors/moleculegeometrybuilder.h
    ${MOD_DIR}/processors/moleculecollectiongeometrybuilder.h
    ${MOD_DIR}/processors/moleculecoprocessor.h
    ${MOD_DIR}/processors/moleculemanipulation.h
    ${MOD_DIR}/properties/moleculeselectionproperty.h
    ${MOD_DIR}/properties/moleculeurllistproperty.h
    ${MOD_DIR}/utils/stridereader.h
    ${MOD_DIR}/utils/moleculeio.h
)

SET(MOD_QT_MODULECLASS MoleculeModuleQt)

SET(MOD_QT_SOURCES
    ${MOD_DIR}/qt/moleculepropertywidgetfactory.cpp
    ${MOD_DIR}/qt/moleculeurllistpropertywidget.cpp
)
    
SET(MOD_QT_HEADERS
    ${MOD_DIR}/qt/moleculeurllistpropertywidget.h
)

SET(MOD_QT_HEADERS_NONMOC
    ${MOD_DIR}/qt/moleculepropertywidgetfactory.h
)
