#ifndef COMINTERFACE_H
#define COMINTERFACE_H

//******************************************************************************
// Includes
//******************************************************************************
#include <QObject>

//******************************************************************************
// Com Interface
//******************************************************************************
#define COM_BUFF_LEN    1024

//******************************************************************************
// Com Interface
//******************************************************************************
class ComInterface : public QObject{
    Q_OBJECT

public:
    explicit ComInterface(QObject *parent = 0);

    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual bool send(unsigned char * data, int len) = 0;
    virtual bool isOpened() = 0;

signals:

    void sigOpened(ComInterface * ifc);
    void sigClosed(ComInterface * ifc);
    void sigError(ComInterface * ifc, int code);
    void sigReceived(ComInterface * ifc, unsigned char * data, int len);
    void sigSent(ComInterface * ifc, unsigned char * data, int len);

public slots:

};

//******************************************************************************
#endif // COMINTERFACE_H
