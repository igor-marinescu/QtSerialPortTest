//******************************************************************************
// Includes
//******************************************************************************
#include "output.h"
#include "utils.h"
#include "debug_cout.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

//******************************************************************************
Output::Settings::Settings(){
    separateRxTx = false;
    noToutDisplRx = false;
    mode = Mode_Hex;
}
//******************************************************************************
Output::Settings::Settings(const Settings & s){
    separateRxTx = s.separateRxTx;
    noToutDisplRx = s.noToutDisplRx;
    mode = s.mode;
}
//******************************************************************************
Output::Settings& Output::Settings::operator=(const Settings& s){
    separateRxTx = s.separateRxTx;
    noToutDisplRx = s.noToutDisplRx;
    mode = s.mode;
    return *this;
}
//******************************************************************************
bool Output::Settings::operator!=(const Settings & s){
    return ((separateRxTx != s.separateRxTx)
         || (noToutDisplRx != s.noToutDisplRx)
         || (mode != s.mode));
}
//******************************************************************************
void Output::Settings::load(ConfFile & conf){
    int val;
    if(conf.get("Output", "separateRxTx", &val))
        separateRxTx = (val != 0);
    if(conf.get("Output", "noToutDisplRx", &val))
        noToutDisplRx = (val != 0);
    if(conf.get("Output", "mode", &val))
        mode = (Mode) val;
    else
        mode = Mode_Hex;
}
//******************************************************************************
void Output::Settings::save(ConfFile & conf){
    conf.set("Output", "separateRxTx", (int) separateRxTx);
    conf.set("Output", "noToutDisplRx", (int) noToutDisplRx);
    conf.set("Output", "mode", (int) mode);
}

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
Output::Output(QObject *parent) :
    QObject(parent), mWin(NULL), rxCnt(0), txCnt(0) {

}

//******************************************************************************
void Output::setUi(QMainWindow * mw){

    releaseUi();

    mWin = mw;

    MainWindow * w = (MainWindow*)mWin;

    applySettings();

    // connect clear/save buttons
    connect(w->ui->btnOutClear, SIGNAL(clicked()),
            this, SLOT(btnClear_clicked()));
    connect(w->ui->cnbboxOutMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(mode_IndexChanged(int)));
}

//******************************************************************************
void Output::releaseUi(){

    if(!mWin)
        return;

    MainWindow * w = (MainWindow*)mWin;
    disconnect(w->ui->btnOutClear);

    // disconnect all connected signals
    mWin = NULL;
}

//******************************************************************************
void Output::applySettings(Settings * ptr){

    if(ptr && (ptr != &settings))
        settings = *ptr;

    MainWindow * w = (MainWindow*)mWin;
    if(w){
        // Separate outputs for Rx and Tx
        if(settings.separateRxTx){

            w->ui->lblOutL->setText("RX:");
            w->ui->lblOutR->setText("TX:");
            w->ui->frmOutR->setVisible(true);
        }
        // Common output for Rx and Tx
        else{
            w->ui->lblOutL->setText("RX \\ TX:");
            w->ui->frmOutR->setVisible(false);
        }

        w->ui->cnbboxOutMode->setCurrentIndex((int) settings.mode);
        btnClear_clicked();
    }
}

//******************************************************************************
void Output::rx(unsigned char * data, int len){

    ++rxCnt;

    if(settings.mode == Mode_None)
        return;

    if(!mWin)
        return;

    buffMutex.lock();


    if(!settings.separateRxTx){
        ((MainWindow*)mWin)->ui->txtOutL->append("RX[" + QString::number(len) + "]:");
    }

    if(settings.mode == Mode_Ascii){
        // Display in ascii format
        binToAscii(data, len, buffStr, OUT_BUFF_LEN);
    }
    else{
        // Display in binar (hex) format
        binToHexAscii(data, len, buffStr, OUT_BUFF_LEN);
    }

    asciiToTextEdit(((MainWindow*)mWin)->ui->txtOutL, buffStr, OUT_BUFF_LEN);
    updateRxTxCnt();

    buffMutex.unlock();
}

//******************************************************************************
void Output::tx(unsigned char * data, int len){

    ++txCnt;

    if(settings.mode == Mode_None)
        return;

    if(!mWin)
        return;

    buffMutex.lock();

    if(settings.mode == Mode_Ascii){
        // Display in ascii format
        binToAscii(data, len, buffStr, OUT_BUFF_LEN);
    }
    else{
        // Display in binar (hex) format
        binToHexAscii(data, len, buffStr, OUT_BUFF_LEN);
    }

    if(!settings.separateRxTx){
        ((MainWindow*)mWin)->ui->txtOutL->append("TX[" + QString::number(len) + "]:");
        asciiToTextEdit(((MainWindow*)mWin)->ui->txtOutL, buffStr, OUT_BUFF_LEN);
    }
    else{
        ((MainWindow*)mWin)->ui->txtOutR->append(buffStr);
        asciiToTextEdit(((MainWindow*)mWin)->ui->txtOutR, buffStr, OUT_BUFF_LEN);
    }

    updateRxTxCnt();

    buffMutex.unlock();
}

//******************************************************************************
void Output::msg(QString & str){

    if(!mWin)
        return;

    ((MainWindow*)mWin)->ui->txtOutL->append(str);
}

//******************************************************************************
void Output::msg(const char * str){

    if(!mWin)
        return;

    ((MainWindow*)mWin)->ui->txtOutL->append(str);
}

//******************************************************************************
void Output::btnSave_clicked(bool checked){
    checked = checked;
}

//******************************************************************************
void Output::btnClear_clicked(bool checked){
    checked = checked;

    txCnt = rxCnt = 0;

    if(!mWin)
        return;

    ((MainWindow*)mWin)->ui->txtOutL->clear();
    ((MainWindow*)mWin)->ui->txtOutR->clear();

    updateRxTxCnt();
}

//******************************************************************************
void Output::updateRxTxCnt(){

    if(!mWin)
        return;

    if(!settings.separateRxTx){
        ((MainWindow*)mWin)->ui->lblOutL->setText("RX (" + QString::number(rxCnt)
                                           + ") \\ TX (" + QString::number(txCnt) + "):");
    }
    else{
        ((MainWindow*)mWin)->ui->lblOutL->setText("RX (" + QString::number(rxCnt) + "):");
        ((MainWindow*)mWin)->ui->lblOutR->setText("TX (" + QString::number(txCnt) + "):");
    }
}

//******************************************************************************
void Output::mode_IndexChanged(int index){

    if(!mWin)
        return;

    settings.mode = (Mode) index;
}
