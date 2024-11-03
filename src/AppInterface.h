#ifndef APPINTERFACE_H
#define APPINTERFACE_H

//******************************************************************************
// Includes
//******************************************************************************
#include "ConfFile.h"
#include <QWidget>
#include <QMessageBox>
#include "Serial.h"
#include "udp.h"

//******************************************************************************
// Class
//******************************************************************************
class AppInterface{

    QWidget * mainWindow;

    bool in_loadWidgetGeometry;
    QWidget * wid_loadWidgetGeometry;

public:

    //--------------------------------------------------------------------------
    class Settings {
    public:
        int  defaultIfc;

/*
        Settings& Settings::operator=(const Settings& s) {
            return *this;
        }
*/
    } settings;
    //--------------------------------------------------------------------------

    ConfFile confFile;

    AppInterface(const char * filename, QWidget * mWindow);
    ~AppInterface();

    void saveWidgetGeometry(QWidget * wid, const char * category);
    void loadWidgetGeometry(QWidget * wid, const char * category);

    // Messages
    static int MsgErr(const QString & txt, const QString & info);
    static int MsgWrn(const QString & txt, const QString & info,
                QMessageBox::StandardButtons btns = QMessageBox::Ok,
                QMessageBox::StandardButton btn = QMessageBox::Ok);

    // Open/Save dialogs
    QString SaveDialog(const QString & directory,
                        const QString & filter,
                       const QString & extension);
    QString OpenDialog(const QString & directory,
                        const QString & filter,
                       const QString & extension);
};

//******************************************************************************
#endif // APPINTERFACE_H
