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

#ifndef VRN_TRANSFUNCLISTPROPERTYWIDGET_H
#define VRN_TRANSFUNCLISTPROPERTYWIDGET_H


#include "voreen/core/datastructures/volume/volumecollection.h"
#include "../properties/transfunclistproperty.h"
#include "../datastructures/moleculevolume.h"
#include "voreen/core/datastructures/volume/volumeram.h"
#include "voreen/core/properties/transfuncproperty.h"

#include "voreen/qt/widgets/voreentoolwindow.h"
#include "voreen/qt/widgets/transfunc/transfuncplugin.h"

#include "voreen/qt/widgets/property/qpropertywidgetwitheditorwindow.h"
#include "voreen/qt/widgets/volumeiohelper.h"

#include <QDialog>
#include <QCheckBox>
#include <QTreeWidget>

namespace voreen {


class TransFuncListPropertyWidget : public QPropertyWidgetWithEditorWindow {
    Q_OBJECT

public:
    TransFuncListPropertyWidget(TransFuncListProperty* volumeCollectionProp, QWidget* parent);

    /// Returns the null pointer, since this widget does not need a separate label.
    virtual CustomLabel* getNameLabel() const;

    virtual void updateFromProperty();
    
    virtual QWidget* createEditorWindowWidget();
    virtual void customizeEditorWindow();
    virtual Property* getProperty();

private:
    void updateSelection();

    TransFuncListProperty* TFListProperty_;
    VolumeIOHelper volumeIOHelper_;

    QTreeWidget* volumeTreeWidget_;
    QPushButton* loadButton_;
    QPushButton* clearButton_;
    QCheckBox* selectAll_;
    
    TransFuncPlugin* plugin_;
    TransFuncProperty* property_;

    static const std::string loggerCat_;

private slots:
    void TFChanged();
    void volumeLoaded(const VolumeBase* handle);
    void clearVolumes();

    void itemSelected(QTreeWidgetItem*, int);
    void selectAll(bool);
};

} //namespace

#endif // VRN_VOLUMEURLISTPROPERTYWIDGET_H
