//******************************************************************************
// Includes
//******************************************************************************
#include "udp.h"
#include "debug_cout.h"

//******************************************************************************
// Settings
//******************************************************************************
UDP::Settings::Settings(){
    remAddress = "192.168.0.1";
    remPort = 50000;
    locPort = 50000;
}
//******************************************************************************
UDP::Settings::Settings(const Settings & s){
    remAddress = s.remAddress;
    remPort = s.remPort;
    locPort = s.locPort;
}
//******************************************************************************
UDP::Settings& UDP::Settings::operator=(const Settings& s){
    remAddress = s.remAddress;
    remPort = s.remPort;
    locPort = s.locPort;
    return *this;
}

//******************************************************************************
void UDP::Settings::load(ConfFile & conf){

    string str;
    int val;

    // Remote Address
    if(conf.get_str("UDP", "RemoteAddr", &str))
        remAddress = QString(str.c_str());

    // Remote Port
    if(conf.get("UDP", "RemotePort", &val))
        remPort = val;

    // Local Port
    if(conf.get("UDP", "LocalPort", &val))
        locPort = val;
}

//******************************************************************************
void UDP::Settings::save(ConfFile & conf){

    string txt;
    txt = remAddress.toStdString();
    conf.set_str("UDP", "RemoteAddr", txt);
    conf.set("UDP", "RemotePort", remPort);
    conf.set("UDP", "LocalPort", locPort);
}

//******************************************************************************
// Constructor
//******************************************************************************
UDP::UDP(QObject *parent) :
    ComInterface(parent),
    udpSocket(parent),
    portOpened(false){

    DBG_ENTER("UDP::UDP");

    // The following signals are not generated for QUdpSocket
    //connect(&udpSocket, SIGNAL(connected()),
    //        this, SLOT(slotOpened()));
    //connect(&udpSocket, SIGNAL(disconnected()),
    //        this, SLOT(slotClosed()));

    connect(&udpSocket, SIGNAL(readyRead()),
            this, SLOT(slotReceive()));

    DBG_EXIT("UDP::UDP");
}

//******************************************************************************
// Destructor
//******************************************************************************
UDP::~UDP(){
    DBG_ENTER("UDP::~UDP");

    DBG_EXIT("UDP::~UDP");
}

//******************************************************************************
// Function:    open
// Description: Binds the specified address to UDP port.
// Returns:     True if success, false error.
//******************************************************************************
bool UDP::open(){
    DBG_ENTER("UDP::open");

    // Check if already opened
    if(portOpened)
        return false;

    // Open
    if(portOpened = udpSocket.bind(QHostAddress::Any, settings.locPort)){
        emit sigOpened(this);
    }

    DBG_EXIT("UDP::open");
    return portOpened;
}

//******************************************************************************
// Function:    close
// Description: Disconnects the UDP port.
// Returns:     always true.
//******************************************************************************
bool UDP::close(){
    DBG_ENTER("UDP::close");

    udpSocket.abort();
    portOpened = false;

    emit sigClosed(this);

    DBG_EXIT("UDP::close");
    return true;
}

//******************************************************************************
// Function:    isOpened
// Description: Returns the connection status of the UDP port.
// Returns:     true if connected
//******************************************************************************
bool UDP::isOpened(){
    return portOpened;
}

//******************************************************************************
// Function:    sent
// Description: Send a buffer of data on UDP port
// Parameters:  data - data buffer to be send
//              len - number of elements in data buffer
//******************************************************************************
bool UDP::send(unsigned char * data, int len){

    if(!portOpened)
        return false;

    // copy data to be send in local buffer
    if(len > COM_BUFF_LEN)
        len = COM_BUFF_LEN;
    memcpy(txData, data, len);

    // send data
    qint64 sent = udpSocket.writeDatagram((const char *) txData, (qint64)len,
                        QHostAddress(settings.remAddress),
                        settings.remPort);
    if(sent != -1){
        emit sigSent(this, txData, (int) sent);
    }

    return true;
}

//******************************************************************************
//
//******************************************************************************
void UDP::slotReceive(){

    while(udpSocket.hasPendingDatagrams()) {

        QHostAddress sender;
        quint16 senderPort;

        int count = (int) udpSocket.readDatagram((char *) rxData, COM_BUFF_LEN,
                                &sender, &senderPort);

        DBG_INFO("UDP::Rx Sender:" << sender.toString().toStdString());

        emit sigReceived(this, rxData, count);
    }
}
