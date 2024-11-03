//******************************************************************************
// Includes
//******************************************************************************
#include "options.h"
#include "AppInterface.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"

//******************************************************************************
Options::Settings::Settings(){
    cyclic = false;
    cyclicRepeat = 0;
    cyclicPeriod = 500;

    telegram = false;
    telDstAddr = 0x10;
    telSrcAddr = 0x01;

    bcc = true;
}
//******************************************************************************
Options::Settings::Settings(const Settings & s){
    cyclic = s.cyclic;
    cyclicRepeat = s.cyclicRepeat;
    cyclicPeriod = s.cyclicPeriod;

    telegram = s.telegram;
    telDstAddr = s.telDstAddr;
    telSrcAddr = s.telSrcAddr;

    bcc = s.bcc;
}
//******************************************************************************
Options::Settings& Options::Settings::operator=(const Settings& s){
    cyclic = s.cyclic;
    cyclicRepeat = s.cyclicRepeat;
    cyclicPeriod = s.cyclicPeriod;

    telegram = s.telegram;
    telDstAddr = s.telDstAddr;
    telSrcAddr = s.telSrcAddr;

    bcc = s.bcc;

    return *this;
}
//******************************************************************************
void Options::Settings::load(ConfFile & conf){
    int val;

    if(conf.get("Options", "cyclic", &val))
        cyclic = (val != 0);

    if(conf.get("Options", "cyclicRepeat", &val))
        cyclicRepeat = val;

    if(conf.get("Options", "cyclicPeriod", &val))
        cyclicPeriod = val;

    if(conf.get("Options", "telegram", &val))
        telegram = (val != 0);

    if(conf.get("Options", "telDstAddr", &val))
        telDstAddr = (val & 0xFF);

    if(conf.get("Options", "telSrcAddr", &val))
        telSrcAddr = (val & 0xFF);

    if(conf.get("Options", "bcc", &val))
        bcc = (val != 0);
}
//******************************************************************************
void Options::Settings::save(ConfFile & conf){
    conf.set("Options", "cyclic", (int) cyclic);
    conf.set("Options", "cyclicRepeat", cyclicRepeat);
    conf.set("Options", "cyclicPeriod", cyclicPeriod);
    conf.set("Options", "telegram", (int) telegram);
    conf.set("Options", "telDstAddr", telDstAddr);
    conf.set("Options", "telSrcAddr", telSrcAddr);
    conf.set("Options", "bcc", (int) bcc);
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
Options::Options(QObject *parent) :
    QObject(parent),
    mWin(NULL){

}

//******************************************************************************
void Options::setUi(QMainWindow * mw){

    releaseUi();

    mWin = mw;

    MainWindow * w = (MainWindow*)mWin;

    applySettings();

    // connect control signals
    connect(w->ui->chkboxCyclic, SIGNAL(stateChanged (int)),
            this, SLOT(cyclic_stateChanged(int)));
    connect(w->ui->chkboxTelegram, SIGNAL(stateChanged (int)),
            this, SLOT(telegram_stateChanged(int)));
    connect(w->ui->chkboxBCC, SIGNAL(stateChanged (int)),
            this, SLOT(bcc_stateChanged(int)));

    connect(w->ui->edtCycRepeat, SIGNAL(editingFinished()),
            this, SLOT(cycRepeat_editingFinished()));
    connect(w->ui->edtCycPeriod, SIGNAL(editingFinished()),
            this, SLOT(cycPeriod_editingFinished()));
    connect(w->ui->edtDstAddr, SIGNAL(editingFinished()),
            this, SLOT(telDstAddr_editingFinished()));
    connect(w->ui->edtSrcAddr, SIGNAL(editingFinished()),
            this, SLOT(telSrcAddr_editingFinished()));
}

//******************************************************************************
void Options::releaseUi(){

    if(!mWin)
        return;

    MainWindow * w = (MainWindow*)mWin;
    disconnect(w->ui->chkboxCyclic);
    disconnect(w->ui->chkboxTelegram);
    disconnect(w->ui->chkboxBCC);

    disconnect(w->ui->edtCycRepeat);
    disconnect(w->ui->edtCycPeriod);
    disconnect(w->ui->edtDstAddr);
    disconnect(w->ui->edtSrcAddr);

    // disconnect all connected signals
    mWin = NULL;
}

//******************************************************************************
void Options::applySettings(Settings * ptr){

    if(ptr && (ptr != &settings))
        settings = *ptr;

    MainWindow * w = (MainWindow*)mWin;
    if(w){
        // cyclic
        w->ui->chkboxCyclic->setChecked(settings.cyclic);
        cyclic_stateChanged(0);
        w->ui->edtCycRepeat->setText(QString::number(settings.cyclicRepeat));
        w->ui->edtCycPeriod->setText(QString::number(settings.cyclicPeriod));

        // telegram
        w->ui->chkboxTelegram->setChecked(settings.telegram);
        telegram_stateChanged(0);
        w->ui->edtDstAddr->setText(((settings.telDstAddr < 0x10) ? "0" : "") +
                                    QString::number(settings.telDstAddr, 16));
        w->ui->edtSrcAddr->setText(((settings.telSrcAddr < 0x10) ? "0" : "") +
                                    QString::number(settings.telSrcAddr, 16));

        // bcc
        w->ui->chkboxBCC->setChecked(settings.bcc);
    }
}

//******************************************************************************
void Options::cyclic_stateChanged(int state){
    (void)state;

    MainWindow * w = (MainWindow*)mWin;
    if(w){
        settings.cyclic = w->ui->chkboxCyclic->isChecked();
        w->ui->lblCycRepeat->setVisible(settings.cyclic);
        w->ui->edtCycRepeat->setVisible(settings.cyclic);
        w->ui->lblCycPeriod->setVisible(settings.cyclic);
        w->ui->edtCycPeriod->setVisible(settings.cyclic);
        emit sigCyclicChanged();
    }
}

//******************************************************************************
void Options::cycRepeat_editingFinished(){
    int val;
    bool ok;

    MainWindow * w = (MainWindow*)mWin;
    if(w){
        val = w->ui->edtCycRepeat->text().toInt(&ok);
        if(!ok || (val < 0)){
            AppInterface::MsgErr("Invalid value for Repeat:", w->ui->edtCycRepeat->text());
            w->ui->edtCycRepeat->setText(QString::number(settings.cyclicRepeat));
            return;
        }
        settings.cyclicRepeat = val;
        emit sigCyclicChanged();
    }
}

//******************************************************************************
void Options::cycPeriod_editingFinished(){
    int val;
    bool ok;

    MainWindow * w = (MainWindow*)mWin;
    if(w){
        val = w->ui->edtCycPeriod->text().toInt(&ok);
        if(!ok || (val < 0)){
            AppInterface::MsgErr("Invalid value for Period:", w->ui->edtCycPeriod->text());
            w->ui->edtCycPeriod->setText(QString::number(settings.cyclicPeriod));
            return;
        }
        settings.cyclicPeriod = val;
        emit sigCyclicChanged();
    }
}

//******************************************************************************
void Options::telegram_stateChanged(int state){
    (void)state;

    MainWindow * w = (MainWindow*)mWin;
    if(w){
        settings.telegram = w->ui->chkboxTelegram->isChecked();
        w->ui->lblDstAddr->setVisible(settings.telegram);
        w->ui->edtDstAddr->setVisible(settings.telegram);
        w->ui->lblSrcAddr->setVisible(settings.telegram);
        w->ui->edtSrcAddr->setVisible(settings.telegram);
        w->ui->chkboxBCC->setEnabled(!settings.telegram);
        emit sigTelegramChanged();
    }
}

//******************************************************************************
void Options::telDstAddr_editingFinished(){
    unsigned char val;

    MainWindow * w = (MainWindow*)mWin;
    if(w){
        if(!hexStringToByte(&val, w->ui->edtDstAddr->text())){
            AppInterface::MsgErr("Invalid value for Destination Address:",
                                 w->ui->edtDstAddr->text());
        }
        else{
            settings.telDstAddr = (int) val;
        }
        w->ui->edtDstAddr->setText(((settings.telDstAddr < 0x10) ? "0" : "") +
                                   QString::number(settings.telDstAddr, 16));
    }
}

//******************************************************************************
void Options::telSrcAddr_editingFinished(){
    unsigned char val;

    MainWindow * w = (MainWindow*)mWin;
    if(w){
        if(!hexStringToByte(&val, w->ui->edtSrcAddr->text())){
            AppInterface::MsgErr("Invalid value for Source Address:",
                                 w->ui->edtSrcAddr->text());
        }
        else{
            settings.telSrcAddr = (int) val;
        }
        w->ui->edtSrcAddr->setText(((settings.telSrcAddr < 0x10) ? "0" : "") +
                                   QString::number(settings.telSrcAddr, 16));
    }
}

//******************************************************************************
void Options::bcc_stateChanged(int state){
    (void)state;
    if(mWin)
        settings.bcc = ((MainWindow*)mWin)->ui->chkboxBCC->isChecked();
}

