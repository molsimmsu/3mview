#ifndef VRN_VOLUMESELECTIONPROPERTYWIDGET_H
#define VRN_VOLUMESELECTIONPROPERTYWIDGET_H


#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/core/datastructures/volume/volumeram.h"

#include "voreen/qt/widgets/property/qpropertywidget.h"
#Iinclude "../properties/volumeselectionproperty.h"

#include <QDialog>
#include <QCheckBox>
#include <QTreeWidget>

namespace voreen {


class VolumeSelectionPropertyWidget : public QPropertyWidget {
    Q_OBJECT

public:
    VolumeSelectionPropertyWidget(VolumeSelectionProperty* volumeCollectionProp, QWidget* parent);

    /// Returns the null pointer, since this widget does not need a separate label.
    virtual CustomLabel* getNameLabel() const;

    virtual void updateFromProperty();

private:
    void updateSelection();

    VolumeURLListProperty* urlListProperty_;
    VolumeIOHelper volumeIOHelper_;

    QTreeWidget* volumeTreeWidget_;
    QPushButton* loadButton_;
    QPushButton* clearButton_;
    QCheckBox* selectAll_;

    static const std::string loggerCat_;

private slots:
    void volumeLoaded(const VolumeBase* handle);
    void clearVolumes();

    void itemSelected(QTreeWidgetItem*, int);
    void selectAll(bool);
};

} //namespace

#endif // VRN_VOLUMEURLISTPROPERTYWIDGET_H

