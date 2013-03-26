#ifndef VRN_MOLECULEURLISTPROPERTYWIDGET_H
#define VRN_MOLECULEURLISTPROPERTYWIDGET_H


#include "../datastructures/moleculecollection.h"
#include "../properties/moleculeurllistproperty.h"

#include "voreen/qt/widgets/property/qpropertywidget.h"
//#include "voreen/qt/widgets/moleculeiohelper.h"

#include <QDialog>
#include <QCheckBox>
#include <QTreeWidget>

namespace voreen {


class MoleculeURLListPropertyWidget : public QPropertyWidget {
    Q_OBJECT

public:
    MoleculeURLListPropertyWidget(MoleculeURLListProperty* moleculeCollectionProp, QWidget* parent);

    /// Returns the null pointer, since this widget does not need a separate label.
    virtual CustomLabel* getNameLabel() const;

    virtual void updateFromProperty();

private:
    void updateSelection();

    MoleculeURLListProperty* urlListProperty_;
    //MoleculeIOHelper moleculeIOHelper_;

    QTreeWidget* moleculeTreeWidget_;
    QPushButton* loadButton_;
    QPushButton* clearButton_;
    QCheckBox* selectAll_;

    static const std::string loggerCat_;

private slots:
    void moleculeLoaded(const Molecule* handle);
    void clearMolecules();

    void itemSelected(QTreeWidgetItem*, int);
    void selectAll(bool);
    void showFileOpenDialog();
};

} //namespace

#endif // VRN_MOLECULEURLISTPROPERTYWIDGET_H
