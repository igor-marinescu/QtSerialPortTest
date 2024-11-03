#ifndef INPUT_H
#define INPUT_H

//******************************************************************************
// Includes
//******************************************************************************
#include <QObject>
#include <QTimer>
#include <QMainWindow>

#include "ConfFile.h"

//******************************************************************************
// Class
//******************************************************************************
class Input : public QObject
{
    Q_OBJECT

    QMainWindow * mWin;
    QTimer * tmr1;

    bool cyclic;
    int repeat;
    int txCnt[5];
    bool telegram;

    void sendString(const QString & fun, const QString & str);

public:

    //--------------------------------------------------------------------------
    // Settings
    //--------------------------------------------------------------------------
    class Settings{
    public:

        int linesCount;

        Settings();
        Settings(const Settings & s);
        Settings& operator=(const Settings& s);

        void load(ConfFile & conf);
        void save(ConfFile & conf);

    } settings;
    //--------------------------------------------------------------------------

    explicit Input(QObject *parent = 0);
    ~Input();

    void setUi(QMainWindow * mw);
    void releaseUi();

    void load(ConfFile & conf);
    void save(ConfFile & conf);

    void applySettings(Settings * ptr = NULL);

    void options(bool cyclic, int repeat, int period, bool telegram);

    void enable(bool state);

signals:

    void sigSend(const QString & fun, const QString & str);

public slots:

    void fun1_returnPressed();
    void fun2_returnPressed();
    void fun3_returnPressed();
    void fun4_returnPressed();
    void fun5_returnPressed();

    void edt1_returnPressed();
    void edt2_returnPressed();
    void edt3_returnPressed();
    void edt4_returnPressed();
    void edt5_returnPressed();

    void btn1_clicked(bool checked);
    void btn2_clicked(bool checked);
    void btn3_clicked(bool checked);
    void btn4_clicked(bool checked);
    void btn5_clicked(bool checked);

    void tmr1_timeout();
};

//******************************************************************************
#endif // INPUT_H
