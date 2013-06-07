/***********************************************************************************
 *                                                                                 *
 * Voreen - The Volume Rendering Engine                                            *
 *                                                                                 *
 * Copyright (C) 2005-2012 University of Muenster, Germany.                        *
 * Visualization and Computer Graphics Group <http://viscg.uni-muenster.de>        *
 * For a list of authors please refer to the file "CREDITS.txt".                   *
 *                                                                                 *
 * This file is part of the Voreen software package. Voreen is free software:      *
 * you can redistribute it and/or modify it under the terms of the GNU General     *
 * Public License version 2 as published by the Free Software Foundation.          *
 *                                                                                 *
 * Voreen is distributed in the hope that it will be useful, but WITHOUT ANY       *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR   *
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.      *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License in the file   *
 * "LICENSE.txt" along with this file. If not, see <http://www.gnu.org/licenses/>. *
 *                                                                                 *
 * For non-commercial academic use see the license exception specified in the file *
 * "LICENSE-academic.txt". To get information about commercial licensing please    *
 * contact the authors.                                                            *
 *                                                                                 *
 ***********************************************************************************/

#include "voreen/core/datastructures/volume/volumecollection.h"
#include "../properties/transfunclistproperty.h"

#include "voreen/qt/widgets/volumeviewhelper.h"
#include "transfunclistpropertywidget.h"

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

const std::string TransFuncListPropertyWidget::loggerCat_("voreenqt.TFListPropertyertyWidget");

TransFuncListPropertyWidget::TransFuncListPropertyWidget(TransFuncListProperty* TFListProperty, QWidget* parent) :
        QPropertyWidgetWithEditorWindow(TFListProperty, parent, false),
        volumeIOHelper_(parent, VolumeIOHelper::MULTI_FILE)
{
    TFListProperty_ = TFListProperty;
    tgtAssert(TFListProperty_, "No volume collection property");

    setFocusPolicy(Qt::StrongFocus);
    QVBoxLayout* mainLayout = new QVBoxLayout();
    layout_->addLayout(mainLayout);

    if (TFListProperty->isLoadable()) {
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        loadButton_ = new QPushButton(tr("Load"));
        loadButton_->setIcon(QPixmap(":/qt/icons/open-volume.png"));
        //loadButton_->setMinimumWidth(110);
        clearButton_ = new QPushButton(tr("Clear"));
        buttonLayout->addWidget(loadButton_);
        buttonLayout->addWidget(clearButton_);
        mainLayout->addLayout(buttonLayout);
        
        connect(loadButton_, SIGNAL(clicked()), &volumeIOHelper_, SLOT(showFileOpenDialog()));
        connect(clearButton_, SIGNAL(clicked()), this, SLOT(clearVolumes()));
    }

    selectAll_ = new QCheckBox("Select All", this);
    //selectAll_->move(8, 0);
    mainLayout->addWidget(selectAll_);

    volumeTreeWidget_ = new QTreeWidget(this);
    QTreeWidgetItem* header = volumeTreeWidget_->headerItem();
    header->setText(0, tr(""));
    volumeTreeWidget_->setColumnCount(1);
    volumeTreeWidget_->show();
    volumeTreeWidget_->setIconSize(QSize(50,50));
    volumeTreeWidget_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);


    connect(&volumeIOHelper_, SIGNAL(volumeLoaded(const VolumeBase*)), this, SLOT(volumeLoaded(const VolumeBase*)));

    connect(volumeTreeWidget_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemSelected(QTreeWidgetItem*, int)));
    connect(volumeTreeWidget_, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SIGNAL(widgetChanged()));
    mainLayout->addWidget(volumeTreeWidget_);

    connect(selectAll_, SIGNAL(toggled(bool)), this, SLOT(selectAll(bool)));

    updateFromProperty();
}

CustomLabel* TransFuncListPropertyWidget::getNameLabel() const {
    return 0;
}

void TransFuncListPropertyWidget::updateFromProperty() {
    if (!TFListProperty_)
        return;
    if (!volumeTreeWidget_->updatesEnabled())
        return;

    VolumeCollection* collection = TFListProperty_->getVolumes(false);
    tgtAssert(collection, "null pointer returned");

    volumeTreeWidget_->clear();

    int numSelected = 0;
    for (size_t i = 0 ; i< collection->size(); i++) {
        VolumeBase* handle = collection->at(i);
        std::string url = handle->getOrigin().getURL();
        QTreeWidgetItem* qtwi = new QTreeWidgetItem(volumeTreeWidget_);

        qtwi->setFont(0, QFont(QString("Arial"), fontSize));
        QString info = QString::fromStdString(VolumeViewHelper::getStrippedVolumeName(handle)
            + "\n"
            + VolumeViewHelper::getVolumePath(handle));
        qtwi->setText(0, info);
        qtwi->setIcon(0, QIcon(VolumeViewHelper::generateBorderedPreview(handle, 27, 0)));
        qtwi->setSizeHint(0,QSize(27,27));
        qtwi->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        // set tree widget to checked, if the corresponding volume handle is contained by the property's collection
        bool selected = TFListProperty_->isSelected(url) ? Qt::Checked : Qt::Unchecked;
        qtwi->setCheckState(0, selected ? Qt::Checked : Qt::Unchecked);
        if (selected)
            numSelected++;

        volumeTreeWidget_->addTopLevelItem(qtwi);
    }

    //clearButton_->setEnabled(!collection->empty());
    selectAll_->setEnabled(!collection->empty());
    if (numSelected == 0)
        selectAll_->setChecked(false);
    else if (numSelected == (int)collection->size())
        selectAll_->setChecked(true);

    delete collection;
}

void TransFuncListPropertyWidget::updateSelection() {
    if (!prop_)
        return;

    VolumeCollection* collection = TFListProperty_->getVolumes(false);
    tgtAssert(collection, "null pointer returned");

    volumeTreeWidget_->setUpdatesEnabled(false);

    QList<QTreeWidgetItem*> items = volumeTreeWidget_->findItems("", Qt::MatchContains);
    for(size_t i = 0; i < collection->size(); i++) {
        bool selected = items.at(static_cast<int>(i))->checkState(0) == Qt::Checked;
        TFListProperty_->setSelected(collection->at(i)->getOrigin().getURL(), selected);
    }
    volumeTreeWidget_->setUpdatesEnabled(true);

    delete collection;
}

void TransFuncListPropertyWidget::volumeLoaded(const VolumeBase* handle) {
    TFListProperty_->addVolume(const_cast<VolumeBase*>(handle), true, true);
}

void TransFuncListPropertyWidget::clearVolumes() {
    TFListProperty_->clear();
}

void TransFuncListPropertyWidget::itemSelected(QTreeWidgetItem*, int) {
    updateSelection();
    
    property_ = new TransFuncProperty("MolVolTransFunc", "Edit transfer function");
    
    VolumeCollection* collection = TFListProperty_->getVolumes(false);
    
    if (!volumeTreeWidget_->currentIndex().isValid()) return;
    
    int row = volumeTreeWidget_->currentIndex().row();
    
    volumeTreeWidget_->setCurrentIndex(QModelIndex()); // reset current index
    
    VolumeBase* v = collection->at(row);
    
    if (typeid(*v) == typeid(MoleculeVolume)) {
        TransFunc* tf = dynamic_cast<MoleculeVolume*>(v)->getTransFunc();
        //TransFunc* tf = new TransFunc1DKeys();
        
        property_->set(tf);

        if (editorWindow_) {
            if (editorWindow_->isVisible())
                editorWindow_->close();
                
            delete editorWindow_;
            editorWindow_ = 0;
        }
        
        createEditorWindow(Qt::RightDockWidgetArea);
            
        tgtAssert(editorWindow_, "Transfunc editor not instantiated");

        editorWindow_->showNormal();
        plugin_->updateFromProperty();
        
    }
    else {
        LWARNING("Volume does not have transfer function");
    }
}

void TransFuncListPropertyWidget::selectAll(bool toggle) {
    TFListProperty_->setAllSelected(toggle);
}

QWidget* TransFuncListPropertyWidget::createEditorWindowWidget() {
    plugin_ = new TransFuncPlugin(property_, parentWidget(), Qt::Horizontal);
    plugin_->createWidgets();
    plugin_->createConnections();
    connect(plugin_, SIGNAL(transferFunctionChanged()), this, SLOT(TFChanged()));

    return plugin_;
}

void TransFuncListPropertyWidget::customizeEditorWindow() {
    editorWindow_->setAllowedAreas(Qt::RightDockWidgetArea);
    editorWindow_->setFloating(true);
}

Property* TransFuncListPropertyWidget::getProperty() {
    return property_;
}

void TransFuncListPropertyWidget::TFChanged() {
    TFListProperty_->invalidate();
}

} //namespace
