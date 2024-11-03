#include "SettingsDialog.h"
#include "ui_settingsdialog.h"
#include <QtSerialPort/QSerialPortInfo>
#include "AppInterface.h"

//******************************************************************************
// Constructor
//******************************************************************************
SettingsDialog::SettingsDialog(QWidget *parent,
                               AppInterface::Settings * app,
                               Serial::Settings * ser,
                               UDP::Settings * udp,
                               int selIfc,
                               Output::Settings * out,
                               Input::Settings * in) :
    QDialog(parent),
    ui(new Ui::SettingsDialog){

    int selected, idx;

    ui->setupUi(this);
    inConstructor = true;

    // Copy application settings
    appSettingsPtr = app;
    appSettingsNew = *app;

    // Copy input settings
    inSettingsPtr = in;
    inSettingsNew = *in;

    // Copy output settings
    outSettingsPtr = out;
    outSettingsNew = *out;

    // Copy serial settings
    serSettingsPtr = ser;
    serSettingsNew = *ser;
    serSettingsChanged = false;

    // Copy UDP settings
    udpSettingsPtr = udp;
    udpSettingsNew = *udp;
    udpSettingsChanged = false;

    // Select Tab based on selected interface
    ui->tabWidget->setCurrentIndex(selIfc);

    //--------------------------------------------------------------------------
    // Serial
    //--------------------------------------------------------------------------

    // Show available ports
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
        ui->cmbboxPort->insertItem(0, info.portName()/* + (info.isBusy()?" (busy)":"")*/);
    }

    // Select currently used port
    for(idx = 0; idx < ui->cmbboxPort->count(); ++idx){
        if(ui->cmbboxPort->itemText(idx) == serSettingsNew.portName){
            ui->cmbboxPort->setCurrentIndex(idx);
            break;
        }
    }

    // If curently used port couldn't be selected (not active anymore)
    if(idx >= ui->cmbboxPort->count()
    && (ui->cmbboxPort->currentIndex() >= 0)){
        // then first port is selected, set that setting changes has been made
        serSettingsChanged = true;
        serSettingsNew.portName = ui->cmbboxPort->itemText(ui->cmbboxPort->currentIndex());
    }

    // Show available baudrates
    selected = -1;
    for(idx = 0; idx < Serial::baudRatesCnt; ++idx){
        ui->cmbboxBaudrate->addItem(QString::number(Serial::baudRates[idx]));

        if(serSettingsNew.baudRate == Serial::baudRates[idx])
            selected = idx;
    }

    // Select currently used baudrate
    if(selected > 0){
        ui->cmbboxBaudrate->setCurrentIndex(selected);
    }

    // Parity
    selected = -1;
    for(idx = 0; idx < Serial::parityCnt; ++idx){
        ui->cmbboxParity->addItem(Serial::parityText[idx]);

        if(serSettingsNew.parity == Serial::parityVals[idx])
            selected = idx;
    }

    // Select currently used parity
    if(selected > 0){
        ui->cmbboxParity->setCurrentIndex(selected);
    }

    //--------------------------------------------------------------------------
    // UDP
    //--------------------------------------------------------------------------
    ui->edtDstAddr->setText(udpSettingsNew.remAddress);
    ui->edtDstPort->setText(QString::number(udpSettingsNew.remPort));
    ui->edtSrcPort->setText(QString::number(udpSettingsNew.locPort));

    //--------------------------------------------------------------------------
    // General
    //--------------------------------------------------------------------------
    ui->chkboxOutRxTx->setChecked(outSettingsNew.separateRxTx);
    ui->chkboxRxDispTout->setChecked(outSettingsNew.noToutDisplRx);
    ui->cmbboxNrInput->setCurrentIndex(inSettingsNew.linesCount);

    inConstructor = false;
}

//******************************************************************************
// Destructor
//******************************************************************************
SettingsDialog::~SettingsDialog(){
    delete ui;
}

//******************************************************************************
// accept
//******************************************************************************
void SettingsDialog::on_buttonBox_accepted(){
    *serSettingsPtr = serSettingsNew;
    *udpSettingsPtr = udpSettingsNew;

    *appSettingsPtr = appSettingsNew;

    outSettingsNew.separateRxTx = ui->chkboxOutRxTx->isChecked();
    outSettingsNew.noToutDisplRx = ui->chkboxRxDispTout->isChecked();
    *outSettingsPtr = outSettingsNew;

    inSettingsNew.linesCount = ui->cmbboxNrInput->currentIndex();
    *inSettingsPtr = inSettingsNew;
}

//******************************************************************************
// reject
//******************************************************************************
void SettingsDialog::on_buttonBox_rejected(){

}

//******************************************************************************
void SettingsDialog::on_cmbboxPort_currentTextChanged(const QString &arg1){
    if(!inConstructor){
        serSettingsNew.portName = arg1;
        serSettingsChanged = true;
    }
}

//******************************************************************************
void SettingsDialog::on_cmbboxBaudrate_currentIndexChanged(int index){
    if(!inConstructor){
        if((index >= 0) && (index < Serial::baudRatesCnt))
            serSettingsNew.baudRate = Serial::baudRates[index];
        serSettingsChanged = true;
    }
}

//******************************************************************************
void SettingsDialog::on_cmbboxParity_currentIndexChanged(int index){
    if(!inConstructor){
        if((index >= 0) && (index < Serial::parityCnt))
            serSettingsNew.parity = Serial::parityVals[index];
        serSettingsChanged = true;
    }
}

//******************************************************************************
void SettingsDialog::on_edtDstAddr_editingFinished(){

    QString str = ui->edtDstAddr->text();

    if(udpSettingsNew.remAddress != str)
        udpSettingsChanged = true;

    udpSettingsNew.remAddress = str;
}

//******************************************************************************
void SettingsDialog::on_edtDstPort_editingFinished(){

    bool status;
    int val = ui->edtDstPort->text().toInt(&status);

    if(!status){
        AppInterface::MsgErr("Invalid remote Port value:", ui->edtDstPort->text());
        ui->edtDstPort->setText(QString::number(udpSettingsNew.remPort));
    }
    else{
        udpSettingsNew.remPort = val;
        udpSettingsChanged = true;
    }
}

//******************************************************************************
void SettingsDialog::on_edtSrcPort_editingFinished(){

    bool status;
    int val = ui->edtSrcPort->text().toInt(&status);

    if(!status){
        AppInterface::MsgErr("Invalid local Port value:", ui->edtSrcPort->text());
        ui->edtSrcPort->setText(QString::number(udpSettingsNew.locPort));
    }
    else{
        udpSettingsNew.locPort = val;
        udpSettingsChanged = true;
    }
}
