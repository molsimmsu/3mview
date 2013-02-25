#include "volumeselectionpropertywidget.h"
#include "voreen/core/datastructures/volume/volumecollection.h"
#include "voreen/qt/widgets/volumeviewhelper.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
#ifdef __APPLE__
    int fontSize = 13;
#else
    int fontSize = 8;
#endif
}

namespace voreen {

const std::string VolumeSelectionPropertyWidget::loggerCat_("voreenqt.VolumeSelectionPropertyWidget");

VolumeSelectionPropertyWidget::VolumeSelectionPropertyWidget(VolumeSelectionProperty* volumeSelectionProp, QWidget* parent)
    : QPropertyWidget(volumeSelectionProp, parent, false)
{
    volumeSelectionProp_ = volumeSelectionProp;
    tgtAssert(volumeSelectionProp_, "No volume collection property");

    setFocusPolicy(Qt::StrongFocus);
    QVBoxLayout* mainLayout = new QVBoxLayout();
    layout_->addLayout(mainLayout);

    selectAll_ = new QCheckBox("Select All", this);
    mainLayout->addWidget(selectAll_);

    volumeTreeWidget_ = new QTreeWidget(this);
    QTreeWidgetItem* header = volumeTreeWidget_->headerItem();
    header->setText(0, tr(""));
    volumeTreeWidget_->setColumnCount(1);
    volumeTreeWidget_->show();
    volumeTreeWidget_->setIconSize(QSize(50,50));
    volumeTreeWidget_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(volumeTreeWidget_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemSelected(QTreeWidgetItem*, int)));
    connect(volumeTreeWidget_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SIGNAL(widgetChanged()));
    mainLayout->addWidget(volumeTreeWidget_);

    connect(selectAll_, SIGNAL(toggled(bool)), this, SLOT(selectAll(bool)));

    updateFromProperty();
}

CustomLabel* VolumeSelectionPropertyWidget::getNameLabel() const {
    return 0;
}

void VolumeSelectionPropertyWidget::updateFromProperty() {
    if (!volumeSelectionProp_)
        return;
    if (!volumeTreeWidget_->updatesEnabled())
        return;
    
    // Get full collection of volumes
    const VolumeCollection* collection = volumeSelectionProp_->getInputVolumes();
    tgtAssert(collection, "null pointer returned");

    volumeTreeWidget_->clear();

    int numSelected = 0;
    for (size_t i = 0 ; i< collection->size(); i++) {
        VolumeBase* volume = collection->at(i);
        std::string url = volume->getOrigin().getURL();
        QTreeWidgetItem* qtwi = new QTreeWidgetItem(volumeTreeWidget_);

        qtwi->setFont(0, QFont(QString("Arial"), fontSize));
        QString info = QString::fromStdString(VolumeViewHelper::getStrippedVolumeName(volume)
            + "\n"
            + VolumeViewHelper::getVolumePath(volume));
        qtwi->setText(0, info);
        qtwi->setIcon(0, QIcon(VolumeViewHelper::generateBorderedPreview(volume, 27, 0)));
        qtwi->setSizeHint(0,QSize(27,27));
        qtwi->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        // Set tree widget to be checked, if the corresponding volume handle is contained by the property's collection
        bool selected = volumeSelectionProp_->isSelected(volume) ? Qt::Checked : Qt::Unchecked;
        qtwi->setCheckState(0, selected ? Qt::Checked : Qt::Unchecked);
        if (selected)
            numSelected++;

        volumeTreeWidget_->addTopLevelItem(qtwi);
    }

    selectAll_->setEnabled(!collection->empty());
    if (numSelected == 0)
        selectAll_->setChecked(false);
    else if (numSelected == (int)collection->size())
        selectAll_->setChecked(true);
}

void VolumeSelectionPropertyWidget::updateSelection() {
    if (!prop_)
        return;

    const VolumeCollection* collection = volumeSelectionProp_->getInputVolumes();
    tgtAssert(collection, "null pointer returned");

    volumeTreeWidget_->setUpdatesEnabled(false);

    QList<QTreeWidgetItem*> items = volumeTreeWidget_->findItems("", Qt::MatchContains);
    for(size_t i = 0; i < collection->size(); i++) {
        bool selected = items.at(static_cast<int>(i))->checkState(0) == Qt::Checked;
        volumeSelectionProp_->setSelected(collection->at(i), selected);
    }
    volumeTreeWidget_->setUpdatesEnabled(true);
}

void VolumeSelectionPropertyWidget::itemSelected(QTreeWidgetItem*, int) {
    updateSelection();
}

void VolumeSelectionPropertyWidget::selectAll(bool toggle) {
    volumeSelectionProp_->setAllSelected(toggle);
}

} //namespace

