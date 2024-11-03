#ifndef UDP_H
#define UDP_H

//******************************************************************************
// Includes
//******************************************************************************
#include <QObject>
#include <QtNetwork/QUdpSocket>
#include "ComInterface.h"
#include "ConfFile.h"

//******************************************************************************
// Defines
//******************************************************************************

//******************************************************************************
// Class
//******************************************************************************
class UDP : public ComInterface {

    Q_OBJECT

public:

    //--------------------------------------------------------------------------
    // Settings
    //--------------------------------------------------------------------------
    class Settings {
    public:
        QString remAddress;     // Remote IP address
        int remPort;            // Remote port
        int locPort;            // Local port

        Settings();
        Settings(const Settings & s);
        Settings& operator=(const Settings& s);

        void load(ConfFile & conf);
        void save(ConfFile & conf);

    } settings;

    //--------------------------------------------------------------------------

    explicit UDP(QObject *parent = 0);
    virtual ~UDP();

    virtual bool open();
    virtual bool close();
    virtual bool send(unsigned char * data, int len);
    virtual bool isOpened();

public slots:

    void slotReceive();

private:

    QUdpSocket udpSocket;
    bool portOpened;

    unsigned char rxData[COM_BUFF_LEN];

    unsigned char txData[COM_BUFF_LEN];
    int txDataLen;
};

//******************************************************************************
#endif // UDP_H
