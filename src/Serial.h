#ifndef SERIAL_H
#define SERIAL_H

//******************************************************************************
// Includes
//******************************************************************************
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "ComInterface.h"
#include "ConfFile.h"

//******************************************************************************
// Defines
//******************************************************************************

//******************************************************************************
// Class
//******************************************************************************
class Serial : public ComInterface {

    Q_OBJECT

public:

    //--------------------------------------------------------------------------

    // baudrates
    static const qint32 baudRates[];
    static const int    baudRatesCnt;

    // parity settings
    static const char   parityText[][10];
    static const int    parityVals[];
    static const int    parityCnt;

    //--------------------------------------------------------------------------
    // Port settings
    //--------------------------------------------------------------------------
    class Settings {
    public:
        QString portName;
        qint32  baudRate;
        int     parity;

        Settings();
        Settings(const Settings & s);
        Settings& operator=(const Settings& s);

        void load(ConfFile & conf);
        void save(ConfFile & conf);

    } settings;

    //--------------------------------------------------------------------------

    static QString getParityText(Settings & set);
    static QString getErrorText(int code);

    explicit Serial(QObject *parent = 0);
    virtual ~Serial();

    virtual bool open();
    virtual bool close();
    virtual bool send(unsigned char * data, int len);
    virtual bool isOpened();

private:

    QSerialPort port;

    // Port status
    bool portOpened;

    // Data
    unsigned char txData[COM_BUFF_LEN];
    int txDataLen;

    unsigned char rxData[COM_BUFF_LEN];
    int rxDataLen;

public slots:
    void readyRead();

};

//******************************************************************************
#endif // SERIAL_H
