//******************************************************************************
// Includes
//******************************************************************************
#include "AppInterface.h"
#include <QFileDialog>

//******************************************************************************
//
//******************************************************************************
AppInterface::AppInterface(const char * filename, QWidget * mWindow) :
    confFile(filename), mainWindow(mWindow){

    int val;

    // Load interface settings
    // Default interface
    if(confFile.get("General", "defaultIfc", &val))
        settings.defaultIfc = val;
}

//******************************************************************************
// App Interface Destructor
//******************************************************************************
AppInterface::~AppInterface(){

    // Save interface settings
    confFile.set("General", "defaultIfc", settings.defaultIfc);
}

//******************************************************************************
// Save Widget Geometry to the configuratoin
//******************************************************************************
void AppInterface::saveWidgetGeometry(QWidget * wid, const char * category){

    // Do not save geometry if it was modified by loadWidgetGeometry
    if(in_loadWidgetGeometry
    &&(wid == wid_loadWidgetGeometry))
        return;

    // if maximized or minimized save only status
    if(wid->isMinimized())
        confFile.set(category, "Minimized", 1);
    else if(wid->isMaximized())
        confFile.set(category, "Maximized", 1);
    else{
        // Save position and size only if not maximized/minimized
        confFile.set(category, "Left",      wid->pos().x());
        confFile.set(category, "Top",       wid->pos().y());
        confFile.set(category, "Width",     wid->size().width());
        confFile.set(category, "Height",    wid->size().height());
        confFile.set(category, "Maximized", 0);
        confFile.set(category, "Minimized", 0);
    }
}

//******************************************************************************
// Load Widget Geometry from the configuration
//******************************************************************************
void AppInterface::loadWidgetGeometry(QWidget * wid, const char * category){

    int x, y, w, h, min, max;

    in_loadWidgetGeometry = true;
    wid_loadWidgetGeometry = wid;

    // Restore window geometry
    if(confFile.get(category, "Left", &x)
    && confFile.get(category, "Top", &y))
        mainWindow->move(x, y);

    if(confFile.get(category, "Width", &w)
    && confFile.get(category, "Height", &h))
        mainWindow->resize(w, h);

    // Restore status
    min = max = 0;
    confFile.get(category, "Minimized", &min);
    confFile.get(category, "Maximized", &max);
    if(max)
        mainWindow->showMaximized();
    else if(min)
        mainWindow->showMinimized();

    in_loadWidgetGeometry = false;
}

//******************************************************************************
// Messages
//******************************************************************************
int AppInterface::MsgErr(const QString & txt,
                        const QString & info){

    QMessageBox msgBox;
    msgBox.setText(txt);
    msgBox.setInformativeText(info);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    return msgBox.exec();
}

//******************************************************************************
int AppInterface::MsgWrn(const QString & txt,
                        const QString & info,
                        QMessageBox::StandardButtons btns,
                        QMessageBox::StandardButton btn){

    QMessageBox msgBox;
    msgBox.setText(txt);
    msgBox.setInformativeText(info);
    msgBox.setStandardButtons(btns);
    msgBox.setDefaultButton(btn);
    msgBox.setIcon(QMessageBox::Warning);
    return msgBox.exec();
}

//******************************************************************************
QString AppInterface::SaveDialog(const QString & directory,
                                 const QString & filter,
                                 const QString & extension){

    QStringList fileNames;
    QFileDialog dialog(mainWindow,
                        "Save File",
                       directory,
                       filter);

    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDefaultSuffix(extension);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if(dialog.exec()){
        fileNames = dialog.selectedFiles();
        return fileNames[0];
    }

    return QString();
}

//******************************************************************************
QString AppInterface::OpenDialog(const QString & directory,
                                 const QString & filter,
                                 const QString & extension){

    QStringList fileNames;
    QFileDialog dialog(mainWindow,
                        "Open File",
                       directory,
                       filter);

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDefaultSuffix(extension);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    if(dialog.exec()){
        fileNames = dialog.selectedFiles();
        return fileNames[0];
    }

    return QString();
}
