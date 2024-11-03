//******************************************************************************
// Includes
//******************************************************************************
#include "Serial.h"
#include "debug_cout.h"

// Baudrates
const qint32 Serial::baudRates[] = {2400, 4800, 9600, 19200, 38400, 57600, 115200, 125000, 230400};
const int    Serial::baudRatesCnt = 9;

// Parity
const char   Serial::parityText[][10] = {"No", "Even", "Odd", "Space", "Mark"};
const int    Serial::parityVals[] = {
                       QSerialPort::NoParity,
                       QSerialPort::EvenParity,
                       QSerialPort::OddParity,
                       QSerialPort::SpaceParity,
                       QSerialPort::MarkParity
                      };
const int    Serial::parityCnt = 5;

//******************************************************************************
// Get text representation of parity
//******************************************************************************
/*static*/ QString Serial::getParityText(Settings & set){
    for(int i = 0; i < Serial::parityCnt; ++i){
        if(set.parity == parityVals[i])
            return QString(parityText[i]);
    }
    return QString("unknown");
}

//******************************************************************************
// Get description of error
//******************************************************************************
/*static*/ QString Serial::getErrorText(int code){
    QSerialPort::SerialPortError err = (QSerialPort::SerialPortError) code;
    switch(err){
    case QSerialPort::NoError:                  return QString("No error.");
    case QSerialPort::DeviceNotFoundError:      return QString("Device not found.");
    case QSerialPort::PermissionError:          return QString("No enough permissions.");
    case QSerialPort::OpenError:                return QString("Device already opened.");
    case QSerialPort::ParityError:              return QString("Parity error.");
    case QSerialPort::FramingError:             return QString("Framing error.");
    case QSerialPort::BreakConditionError:      return QString("Break condition detected.");
    case QSerialPort::WriteError:               return QString("Write error.");
    case QSerialPort::ReadError:                return QString("Read error.");
    case QSerialPort::ResourceError:            return QString("Resource unavailable.");
    case QSerialPort::UnsupportedOperationError:return QString("Unsupported operation.");
    }
    return QString("Unknown error");
}

//******************************************************************************
// Settings
//******************************************************************************
Serial::Settings::Settings(){
    portName = "COM1";
    baudRate = 115200;
    parity = QSerialPort::NoParity;
}
//******************************************************************************
Serial::Settings::Settings(const Settings & s){
    portName = s.portName;
    baudRate = s.baudRate;
    parity = s.parity;
}
//******************************************************************************
Serial::Settings& Serial::Settings::operator=(const Settings& s){
    portName = s.portName;
    baudRate = s.baudRate;
    parity = s.parity;
    return *this;
}

//******************************************************************************
void Serial::Settings::load(ConfFile & conf){

    string str;
    int val;

    if(conf.get_str("COM", "Port", &str))
        portName = QString(str.c_str());

    if(conf.get("COM", "Baudrate", &val))
        baudRate = (qint32) val;

    if(conf.get("COM", "Parity", &val))
        parity = val;
}

//******************************************************************************
void Serial::Settings::save(ConfFile & conf){

    string txt;
    txt = portName.toStdString();
    conf.set_str("COM", "Port", txt);
    conf.set("COM", "Baudrate", (int) baudRate);
    conf.set("COM", "Parity", parity);
}

//******************************************************************************
// Constructor
//******************************************************************************
Serial::Serial(QObject *parent) :
    ComInterface(parent),
    portOpened(false){

    DBG_ENTER("Serial::Serial");

    connect(&port, SIGNAL(readyRead()),
            this, SLOT(readyRead()));

    DBG_EXIT("Serial::Serial");
}

//******************************************************************************
// Destructor
//******************************************************************************
Serial::~Serial(){

    DBG_ENTER("Serial::~Serial");

    DBG_EXIT("Serial::~Serial");
}

//******************************************************************************
bool Serial::open(){

    DBG_ENTER("Serial::open");

    port.close();

    port.setPortName(settings.portName);
    portOpened = port.open(QIODevice::ReadWrite);

    if(portOpened)
        portOpened = port.setBaudRate(settings.baudRate);

    if(portOpened)
        portOpened = port.setDataBits(QSerialPort::Data8);

    if(portOpened)
        portOpened = port.setParity((QSerialPort::Parity)settings.parity);

    if(portOpened)
        portOpened = port.setStopBits(QSerialPort::OneStop);

    if(portOpened)
        portOpened = port.setFlowControl(QSerialPort::NoFlowControl);

    if(!portOpened){
        DBG_ERROR("Error opening port");
        QSerialPort::SerialPortError err = port.error();
        port.close();
        emit sigError(this, (int)err);
    }
    else{
        emit sigOpened(this);
    }

    DBG_EXIT("Serial::open");

    return portOpened;
}

//******************************************************************************
bool Serial::close(){
    DBG_ENTER("SerialThread::close");

    port.close();
    portOpened = false;
    emit sigClosed(this);

    DBG_EXIT("SerialThread::close");
    return true;
}

//******************************************************************************
bool Serial::send(unsigned char * data, int len){

    if(len < 0)
        return true;

    if(!portOpened)
        return false;

    if(len > COM_BUFF_LEN)
        len = COM_BUFF_LEN;

    txDataLen = len;
    memcpy(txData, data, txDataLen);
    int txLen = port.write((const char *)txData, txDataLen);

    emit sigSent(this, txData, txLen);

    return true;
}

//******************************************************************************
bool Serial::isOpened(){
    return portOpened;
}

//******************************************************************************
void Serial::readyRead(){

    rxDataLen = port.read((char *)rxData, COM_BUFF_LEN);

    emit sigReceived(this, rxData, rxDataLen);
}

//******************************************************************************
/*
        //----------------------------------------------------------------------
        // Data available? (Block for 50ms if no data)
        //----------------------------------------------------------------------
        if(port.waitForReadyRead(30)){
            rxLen = port.read((char *)rxDataTemp, SERIAL_BUFF_LEN);
            if(rxLen > 0){
                // Accumulate data
                if(rxLen <= (SERIAL_BUFF_LEN - rxDataLen)){
                    memcpy(&rxData[rxDataLen], rxDataTemp, rxLen);
                    rxDataLen += rxLen;
                }
                else{
                    // Not enough space in the accumulator buffer
                    // get as much data as possible
                    memcpy(&rxData[rxDataLen], rxDataTemp,
                           (SERIAL_BUFF_LEN - rxDataLen));
                    emit serial->sigReceived(serial, rxData, SERIAL_BUFF_LEN);
                    rxDataLen = 0;
                    // Data overflow error
                    // The rest of rxDataTemp buffer is lost!
                }
            }
        }
        else {
            // Timeout
            // notify user about accumulated data
            if(rxDataLen > 0){
                emit serial->sigReceived(serial, rxData, rxDataLen);
            }
            rxDataLen = 0;
        }
        //----------------------------------------------------------------------
*/
//******************************************************************************
