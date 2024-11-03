#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "debug_cout.h"
#include "utils.h"
#include "SettingsDialog.h"

//******************************************************************************
MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){

    DBG_ENTER("MainWindow::MainWindow");

    ui->setupUi(this);

    // Create application interface
    if(argc > 1)
        appInterface = new AppInterface(argv[1], this);
    else
        appInterface = new AppInterface("config.cfg", this);
    appInterface->loadWidgetGeometry(this, "WinPos");

    // Output
    output.settings.load(appInterface->confFile);
    output.setUi(this);

    // Options
    options.settings.load(appInterface->confFile);
    options.setUi(this);

    // Input
    input.settings.load(appInterface->confFile);
    input.setUi(this);
    input.load(appInterface->confFile);
    input.options(options.settings.cyclic,
                  options.settings.cyclicRepeat,
                  options.settings.cyclicPeriod,
                  options.settings.telegram);
    input.enable(false);

    //--------------------------------------------------------------------------
    // Connect signals
    //--------------------------------------------------------------------------

    // Options
    connect(&options, SIGNAL(sigCyclicChanged()),
            this, SLOT(optionCyclicChanged()));
    connect(&options, SIGNAL(sigTelegramChanged()),
            this, SLOT(optionTelegramChanged()));

    // Input
    connect(&input, SIGNAL(sigSend(const QString &, const QString &)),
            this, SLOT(sendString(const QString &, const QString &)));

    // Error
    connect(&serial, SIGNAL(sigError(ComInterface *, int)),
            this, SLOT(serErrorEvent(ComInterface *, int)));

    // Opened
    connect(&serial, SIGNAL(sigOpened(ComInterface *)),
            this, SLOT(serChangeEvent(ComInterface *)));
    connect(&udp, SIGNAL(sigOpened(ComInterface *)),
            this, SLOT(serChangeEvent(ComInterface *)));

    // Closed
    connect(&serial, SIGNAL(sigClosed(ComInterface *)),
            this, SLOT(serChangeEvent(ComInterface *)));
    connect(&udp, SIGNAL(sigClosed(ComInterface *)),
            this, SLOT(serChangeEvent(ComInterface *)));

    // Data Received
    connect(&serial, SIGNAL(sigReceived(ComInterface *, unsigned char *, int)),
            this, SLOT(serRxEvent(ComInterface *, unsigned char *, int)));
    connect(&udp, SIGNAL(sigReceived(ComInterface *, unsigned char *, int)),
            this, SLOT(serRxEvent(ComInterface *, unsigned char *, int)));

    // Data Sent
    connect(&serial, SIGNAL(sigSent(ComInterface *, unsigned char *, int)),
            this, SLOT(serTxEvent(ComInterface *, unsigned char *, int)));
    connect(&udp, SIGNAL(sigSent(ComInterface *, unsigned char *, int)),
            this, SLOT(serTxEvent(ComInterface *, unsigned char *, int)));

    //--------------------------------------------------------------------------

    // Initial port settings
    serSettingsNew.load(appInterface->confFile);
    serial.settings = serSettingsNew;

    udpSettingsNew.load(appInterface->confFile);
    udp.settings = udpSettingsNew;
    opened = false;
    ui->btnOpen->setText("&Open");

    // RX Timeout Timer
    rxToutTmr = new QTimer();
    rxToutTmr->setSingleShot(true);
    connect(rxToutTmr, SIGNAL(timeout()), this, SLOT(rxToutTmr_timeout()));
    rxBuffLen = 0;

    // Select default interface
    if(appInterface->settings.defaultIfc >= ui->comboBoxIfc->count())
        appInterface->settings.defaultIfc = ui->comboBoxIfc->count() - 1;
    ui->comboBoxIfc->setCurrentIndex(appInterface->settings.defaultIfc);
    showActSettings();

    DBG_EXIT("MainWindow::MainWindow");
}

//******************************************************************************
MainWindow::~MainWindow(){

    DBG_ENTER("MainWindow::~MainWindow");

    rxToutTmr->stop();
    delete rxToutTmr;

    // Options
    options.settings.save(appInterface->confFile);
    options.releaseUi();

    // Output
    output.settings.save(appInterface->confFile);
    output.releaseUi();

    // Input
    input.settings.save(appInterface->confFile);
    input.save(appInterface->confFile);
    input.releaseUi();

    // Save settings
    serSettingsNew.save(appInterface->confFile);
    udpSettingsNew.save(appInterface->confFile);

    // Destroy application interface
    appInterface->saveWidgetGeometry(this, "WinPos");
    delete appInterface;

    delete ui;

    DBG_EXIT("MainWindow::~MainWindow");
}

//******************************************************************************
void MainWindow::on_btnOpen_clicked(){

    // Open
    if(!opened){
        // COM
        if(ui->comboBoxIfc->currentIndex() == 0){
            serial.settings = serSettingsNew;
            if(!serial.open())
                AppInterface::MsgWrn("Cannot open COM port.",
                                     "Check COM port settings.");
        }
        // UDP
        else{
            udp.settings = udpSettingsNew;
            if(!udp.open())
                AppInterface::MsgWrn("Cannot open UDP connection.",
                                     "Check UDP connection settings.");
        }
    }
    // Close
    else{
        // COM
        if(ui->comboBoxIfc->currentIndex() == 0)
            serial.close();
        // UDP
        else
            udp.close();
    }
}

//******************************************************************************
void MainWindow::sendString(const QString & fun, const QString & str){

    int len;
    unsigned char * txPtr = txBuff;

    //--------------------------------------------------------------------------
    // If rx timer still active, then show the previous RX packet
    // before sending new one
    if(rxToutTmr->isActive()){
        rxToutTmr->stop();
        rxToutTmr_timeout();    // This will display received rx buffer
    }
    //--------------------------------------------------------------------------

    // Telegram?
    if(options.settings.telegram){
        // Destionation address
        txPtr[0] = (unsigned char) options.settings.telDstAddr;
        // Source address
        txPtr[1] = (unsigned char) options.settings.telSrcAddr;
        // Length (filled later)
        // txPtr[2] = ???
        // Function
        if(hexStringToBin(&txPtr[3], 1, fun) < 1)
            txPtr[3] = 0;
        txPtr += 4;
    }

    // Decode string into bin
    len = hexStringToBin(txPtr, (options.settings.telegram ? COM_BUFF_LEN - 4 : COM_BUFF_LEN ), str);
    if(len < 0){
        AppInterface::MsgErr("Error", "Incorrect hex text format");
        return;
    }

    // Check if not empty
    if((len == 0) && !options.settings.telegram)
        return;

    // Telegram? Insert length and bcc
    if(options.settings.telegram){
        txBuff[2] = len + 4;

        if((len + 4) < (COM_BUFF_LEN - 1)){
            txBuff[len + 4] = bcc(txBuff, len + 4);
            len += 5; // 4 Header + 1 bcc
        }
    }
    // apend BCC if required
    else if(options.settings.bcc){
        if(len < (COM_BUFF_LEN - 1)){
            txBuff[len] = bcc(txBuff, len);
            len++;
        }
    }

    // Send data on the selected interface
    ComInterface * ifc;
    if(ui->comboBoxIfc->currentIndex() == 0)
        ifc = &serial;
    else
        ifc = &udp;

    // check if interface opened
    if(ifc->isOpened() == false){
        AppInterface::MsgErr("Error", "Port closed");
        return;
    }
    ifc->send(txBuff, len);
}

//******************************************************************************
void MainWindow::optionCyclicChanged(){

    input.options(options.settings.cyclic,
                  options.settings.cyclicRepeat,
                  options.settings.cyclicPeriod,
                  options.settings.telegram);
}

//******************************************************************************
void MainWindow::optionTelegramChanged(){

    input.options(options.settings.cyclic,
                  options.settings.cyclicRepeat,
                  options.settings.cyclicPeriod,
                  options.settings.telegram);
}

//******************************************************************************
void MainWindow::on_btnSettings_clicked(){

    Output::Settings outSettings = output.settings;
    Input::Settings inSettings = input.settings;

    // Show settings dialog
    SettingsDialog settings(this,
                            &appInterface->settings,
                            &serSettingsNew,
                            &udpSettingsNew,
                            ui->comboBoxIfc->currentIndex(),
                            &outSettings,
                            &inSettings);

    settings.setModal(true);
    settings.exec();

    // If settings changed
    if(settings.result() == QDialog::Accepted){
        // Serial settings
        if(settings.serSettingsChanged){
            // Check if port is opened
            if(serial.isOpened()){
                // Port is opened, the new settings will become active
                // when the port is reopened
                AppInterface::MsgWrn("COM is already opened.",
                    "Reconnect (Close then Open) to apply the new settings.");
            }
            else
                serial.settings = serSettingsNew;
        }
        // UDP settings changed
        if(settings.udpSettingsChanged){
            // Check if UDP is opened
            if(udp.isOpened()){
                // UDP is opened, the new settings will become active
                // when the port is reopened
                AppInterface::MsgWrn("UDP is already opened.",
                    "Reconnect (Close then Open) to apply the new settings.");
            }
            else
                udp.settings = udpSettingsNew;
        }
        output.applySettings(&outSettings);
        input.applySettings(&inSettings);
    }
    showActSettings();
}

//******************************************************************************
// Signal: Data received
//******************************************************************************
void MainWindow::serRxEvent(ComInterface * ifc, unsigned char * data, int len){
    ifc = ifc;

    // Store received data in a buffer
    if((rxBuffLen + len) > COM_BUFF_LEN){
        len = COM_BUFF_LEN - rxBuffLen;
    }

    if(len > 0){
        memcpy(&rxBuff[rxBuffLen], data, len);
        rxBuffLen += len;
    }

    // Decide if to display received data now
    // or to wait a timeout for more data to come
    if(output.settings.noToutDisplRx){
        output.rx(rxBuff, rxBuffLen);
        rxBuffLen = 0;
    }
    else{
        // Start timer for 100 ms in case if more data is about to come
        // (as a part of the same packet)
        rxToutTmr->start(100);
    }
}

//******************************************************************************
void MainWindow::rxToutTmr_timeout(){

    output.rx(rxBuff, rxBuffLen);
    rxBuffLen = 0;
}

//******************************************************************************
// Signal: Data sent
//******************************************************************************
void MainWindow::serTxEvent(ComInterface * ifc, unsigned char * data, int len){
    ifc = ifc;
    output.tx(data, len);
}

//******************************************************************************
// Signal: Error occured
//******************************************************************************
void MainWindow::serErrorEvent(ComInterface * ifc, int code){
    ifc = ifc;
    QString txt = Serial::getErrorText(code);
    AppInterface::MsgErr("Error", txt);
}

//******************************************************************************
// Signal: Port connected/disconnected
//******************************************************************************
void MainWindow::serChangeEvent(ComInterface * ifc){

    ifc = ifc;

    if(serial.isOpened()){
        ui->comboBoxIfc->setCurrentIndex(0);
        opened = true;
    }
    else if(udp.isOpened()){
        ui->comboBoxIfc->setCurrentIndex(1);
        opened = true;
    }
    else
        opened = false;

    // Update controls status
    ui->btnSettings->setEnabled(!opened);
    ui->comboBoxIfc->setEnabled(!opened);
    ui->btnOpen->setText(opened? "&Close" : "&Open");
    output.msg(opened ? "[Connected]" : "[Disconnected]");
    input.enable(opened);
}

//******************************************************************************
void MainWindow::showActSettings(){

    QString text;

    if(ui->comboBoxIfc->currentIndex() == 0){
        text  = serial.settings.portName;
        text += " | Baudrate: " + QString::number(serial.settings.baudRate);
        text += " | Parity: " + Serial::getParityText(serial.settings);
    }
    else{
        text  = "UDP | Port: " + QString::number(udp.settings.locPort);
        text += " | Remote: " + udp.settings.remAddress;
        text += " : " + QString::number(udp.settings.remPort);
    }
    //statusBar()->showMessage(text);
    ui->label->setText(text);
}

//******************************************************************************
void MainWindow::on_comboBoxIfc_currentIndexChanged(int index){
    appInterface->settings.defaultIfc = index;
    showActSettings();
}
