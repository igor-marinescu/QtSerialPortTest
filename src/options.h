#ifndef OPTIONS_H
#define OPTIONS_H

//******************************************************************************
// Includes
//******************************************************************************
#include <QObject>
#include <QMainWindow>

#include "ConfFile.h"

//******************************************************************************
// Class
//******************************************************************************
class Options  : public QObject{

    Q_OBJECT
    QMainWindow * mWin;

public:

    //--------------------------------------------------------------------------
    // Settings
    //--------------------------------------------------------------------------
    class Settings{
    public:

        bool cyclic;
        int cyclicRepeat;
        int cyclicPeriod;
        bool telegram;
        int telDstAddr;
        int telSrcAddr;
        bool bcc;

        Settings();
        Settings(const Settings & s);
        Settings& operator=(const Settings& s);

        void load(ConfFile & conf);
        void save(ConfFile & conf);

    } settings;
    //--------------------------------------------------------------------------

    explicit Options(QObject *parent = 0);

    void setUi(QMainWindow * mw);
    void releaseUi();

    void applySettings(Settings * ptr = NULL);

signals:

    void sigCyclicChanged();
    void sigTelegramChanged();

public slots:

    void cyclic_stateChanged(int state);
    void telegram_stateChanged(int state);
    void bcc_stateChanged(int state);

    void cycRepeat_editingFinished();
    void cycPeriod_editingFinished();
    void telDstAddr_editingFinished();
    void telSrcAddr_editingFinished();
};

//******************************************************************************
#endif // OPTIONS_H
