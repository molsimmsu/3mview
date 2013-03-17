#include "../datastructures/moleculecollection.h"
#include "../properties/moleculeurllistproperty.h"

//#include "voreen/qt/widgets/moleculeviewhelper.h"
#include "moleculeurllistpropertywidget.h"

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

const std::string MoleculeURLListPropertyWidget::loggerCat_("3mview.MoleculeCollectionPropertyWidget");

MoleculeURLListPropertyWidget::MoleculeURLListPropertyWidget(MoleculeURLListProperty* moleculeCollectionProp, QWidget* parent)
    : QPropertyWidget(moleculeCollectionProp, parent, false)
    //, moleculeIOHelper_(parent, MoleculeIOHelper::MULTI_FILE)
{
    urlListProperty_ = moleculeCollectionProp;
    tgtAssert(urlListProperty_, "No molecule collection property");

    setFocusPolicy(Qt::StrongFocus);
    QVBoxLayout* mainLayout = new QVBoxLayout();
    layout_->addLayout(mainLayout);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    loadButton_ = new QPushButton(tr("Load Molecules..."));
    loadButton_->setIcon(QPixmap(":/qt/icons/open-volume.png"));
    loadButton_->setMinimumWidth(110);
    clearButton_ = new QPushButton(tr("Clear Molecules"));
    buttonLayout->addWidget(loadButton_);
    buttonLayout->addWidget(clearButton_);
    mainLayout->addLayout(buttonLayout);

    selectAll_ = new QCheckBox("Select All", this);
    //selectAll_->move(8, 0);
    mainLayout->addWidget(selectAll_);

    moleculeTreeWidget_ = new QTreeWidget(this);
    QTreeWidgetItem* header = moleculeTreeWidget_->headerItem();
    header->setText(0, tr(""));
    moleculeTreeWidget_->setColumnCount(1);
    moleculeTreeWidget_->show();
    moleculeTreeWidget_->setIconSize(QSize(50,50));
    moleculeTreeWidget_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    //FIXME connect(loadButton_, SIGNAL(clicked()), &moleculeIOHelper_, SLOT(showFileOpenDialog()));
    //FIXME connect(&moleculeIOHelper_, SIGNAL(moleculeLoaded(const Molecule*)), this, SLOT(moleculeLoaded(const Molecule*)));
    connect(clearButton_, SIGNAL(clicked()), this, SLOT(clearMolecules()));

    connect(moleculeTreeWidget_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemSelected(QTreeWidgetItem*, int)));
    connect(moleculeTreeWidget_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SIGNAL(widgetChanged()));
    mainLayout->addWidget(moleculeTreeWidget_);

    connect(selectAll_, SIGNAL(toggled(bool)), this, SLOT(selectAll(bool)));

    updateFromProperty();
}



CustomLabel* MoleculeURLListPropertyWidget::getNameLabel() const {
    return 0;
}

void MoleculeURLListPropertyWidget::updateFromProperty() {
    if (!urlListProperty_)
        return;
    if (!moleculeTreeWidget_->updatesEnabled())
        return;

    MoleculeCollection* collection = urlListProperty_->getMolecules(false);
    tgtAssert(collection, "null pointer returned");

    moleculeTreeWidget_->clear();

    int numSelected = 0;
    for (size_t i = 0 ; i< collection->size(); i++) {
        Molecule* handle = collection->at(i);
        std::string url = handle->getOrigin().getURL();
        QTreeWidgetItem* qtwi = new QTreeWidgetItem(moleculeTreeWidget_);

        qtwi->setFont(0, QFont(QString("Arial"), fontSize));
        QString info = QString::fromStdString(handle->getOrigin().getFilename()
            + "\n"
            + handle->getOrigin().getPath());
        qtwi->setText(0, info);
        //qtwi->setIcon(0, QIcon(MoleculeViewHelper::generateBorderedPreview(handle, 27, 0)));
        qtwi->setSizeHint(0,QSize(27,27));
        qtwi->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        // set tree widget to checked, if the corresponding molecule handle is contained by the property's collection
        bool selected = urlListProperty_->isSelected(url) ? Qt::Checked : Qt::Unchecked;
        qtwi->setCheckState(0, selected ? Qt::Checked : Qt::Unchecked);
        if (selected)
            numSelected++;

        moleculeTreeWidget_->addTopLevelItem(qtwi);
    }

    clearButton_->setEnabled(!collection->empty());
    selectAll_->setEnabled(!collection->empty());
    if (numSelected == 0)
        selectAll_->setChecked(false);
    else if (numSelected == (int)collection->size())
        selectAll_->setChecked(true);

    delete collection;
}

void MoleculeURLListPropertyWidget::updateSelection() {
    if (!prop_)
        return;

    MoleculeCollection* collection = urlListProperty_->getMolecules(false);
    tgtAssert(collection, "null pointer returned");

    moleculeTreeWidget_->setUpdatesEnabled(false);

    QList<QTreeWidgetItem*> items = moleculeTreeWidget_->findItems("", Qt::MatchContains);
    for(size_t i = 0; i < collection->size(); i++) {
        bool selected = items.at(static_cast<int>(i))->checkState(0) == Qt::Checked;
        urlListProperty_->setSelected(collection->at(i)->getOrigin().getURL(), selected);
    }
    moleculeTreeWidget_->setUpdatesEnabled(true);

    delete collection;
}

void MoleculeURLListPropertyWidget::moleculeLoaded(const Molecule* handle) {
    urlListProperty_->addMolecule(const_cast<Molecule*>(handle), true, true);
}

void MoleculeURLListPropertyWidget::clearMolecules() {
    urlListProperty_->clear();
}

void MoleculeURLListPropertyWidget::itemSelected(QTreeWidgetItem*, int) {
    updateSelection();
}

void MoleculeURLListPropertyWidget::selectAll(bool toggle) {
    urlListProperty_->setAllSelected(toggle);
}

} //namespace
