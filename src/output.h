#ifndef OUTPUT_H
#define OUTPUT_H

//******************************************************************************
// Includes
//******************************************************************************
#include <QObject>
#include <QMutex>
#include <QMainWindow>

#include "ConfFile.h"
#include "ComInterface.h"

//******************************************************************************
// Define
//******************************************************************************
#define OUT_BUFF_LEN    COM_BUFF_LEN*3

//******************************************************************************
// Class
//******************************************************************************
class Output : public QObject{

    Q_OBJECT

    char buffStr[OUT_BUFF_LEN];

    QMutex buffMutex;
    QMainWindow * mWin;

    int rxCnt;
    int txCnt;

    void updateRxTxCnt();

public:


    //--------------------------------------------------------------------------
    // Settings
    //--------------------------------------------------------------------------
    typedef enum {
        Mode_None = 0,
        Mode_Hex,
        Mode_Ascii,
        Mode_Hex_Ascii
    } Mode;

    class Settings{
    public:

        bool separateRxTx;
        bool noToutDisplRx;
        Mode mode;

        Settings();
        Settings(const Settings & s);
        Settings& operator=(const Settings& s);
        bool operator!=(const Settings & s);

        void load(ConfFile & conf);
        void save(ConfFile & conf);

    } settings;

    //--------------------------------------------------------------------------

    explicit Output(QObject *parent = 0);

    void setUi(QMainWindow * mw);
    void releaseUi();

    void applySettings(Settings * ptr = NULL);

    void rx(unsigned char * data, int len);
    void tx(unsigned char * data, int len);

    void msg(QString & str);
    void msg(const char * str);

signals:

public slots:

    void btnSave_clicked( bool checked = false);
    void btnClear_clicked( bool checked = false);
    void mode_IndexChanged(int index);
};

//******************************************************************************
#endif // OUTPUT_H
