#ifndef THREEMTK_SYSTEM_MESSAGE_H
#define THREEMTK_SYSTEM_MESSAGE_H

#include <QMessageBox>

void QWARNING(const char* text) {
    QMessageBox msgBox;
    msgBox.setText("Warning");
    msgBox.setInformativeText(text);
    msgBox.setStandardButtons(QMessageBox::Ok);
    int ret = msgBox.exec();
}
    
#endif
