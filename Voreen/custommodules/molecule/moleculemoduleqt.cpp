#include "moleculemoduleqt.h"

#include "qt/moleculepropertywidgetfactory.h"

namespace voreen {

const std::string MoleculeModuleQt::loggerCat_("3mview.molecule.MoleculeModuleQt");

MoleculeModuleQt::MoleculeModuleQt(const std::string& modulePath)
    : VoreenModuleQt(modulePath)
{
    setName("Molecule (Qt)");

    registerPropertyWidgetFactory(new MoleculePropertyWidgetFactory());
}

} // namespace