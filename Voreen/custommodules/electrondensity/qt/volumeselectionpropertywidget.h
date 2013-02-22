#ifndef VRN_VOLUMESELECTIONPROPERTYWIDGET_H
#define VRN_VOLUMESELECTIONPROPERTYWIDGET_H

#include "voreen/qt/widgets/property/qpropertywidget.h"
#include "../properties/volumeselectionproperty.h"

#include <QCheckBox>
#include <QTreeWidget>

namespace voreen {


class VolumeSelectionPropertyWidget : public QPropertyWidget {
    Q_OBJECT

public:
    VolumeSelectionPropertyWidget(VolumeSelectionProperty* volumeSelectionProp, QWidget* parent);

    /// Returns the null pointer, since this widget does not need a separate label.
    virtual CustomLabel* getNameLabel() const;

    virtual void updateFromProperty();

private:
    void updateSelection();

    VolumeSelectionProperty* volumeSelectionProp_;

    QTreeWidget* volumeTreeWidget_;
    QCheckBox* selectAll_;

    static const std::string loggerCat_;

private slots:
    void itemSelected(QTreeWidgetItem*, int);
    void selectAll(bool);
};

} //namespace

#endif // VRN_VOLUMEURLISTPROPERTYWIDGET_H

