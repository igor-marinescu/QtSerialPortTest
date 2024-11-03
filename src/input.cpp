//******************************************************************************
// Includes
//******************************************************************************
#include "input.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

//******************************************************************************
Input::Settings::Settings(){
    linesCount = 2;
}
//******************************************************************************
Input::Settings::Settings(const Settings & s){
    linesCount = s.linesCount;
}
//******************************************************************************
Input::Settings& Input::Settings::operator=(const Settings& s){
    linesCount = s.linesCount;
    return *this;
}
//******************************************************************************
void Input::Settings::load(ConfFile & conf){
    int val;
    if(conf.get("Input", "count", &val))
        linesCount = val;
}
//******************************************************************************
void Input::Settings::save(ConfFile & conf){
    conf.set("Input", "count", (int) linesCount);
}

//******************************************************************************
//******************************************************************************
//******************************************************************************
//******************************************************************************
Input::Input(QObject *parent) :
    QObject(parent), cyclic(false), telegram(false),
    mWin(NULL){

    tmr1 = new QTimer();

    connect(tmr1, SIGNAL(timeout()),
            this, SLOT(tmr1_timeout()));
}

//******************************************************************************
Input::~Input(){

    tmr1->stop();
    delete tmr1;
}

//******************************************************************************
void Input::setUi(QMainWindow * mw){

    releaseUi();

    mWin = mw;

    MainWindow * w = (MainWindow*)mWin;

    applySettings();

    // connect signals
    connect(w->ui->edtFun1, SIGNAL(returnPressed()),
            this, SLOT(fun1_returnPressed()));
    connect(w->ui->edtFun2, SIGNAL(returnPressed()),
            this, SLOT(fun2_returnPressed()));
    connect(w->ui->edtFun3, SIGNAL(returnPressed()),
            this, SLOT(fun3_returnPressed()));
    connect(w->ui->edtFun4, SIGNAL(returnPressed()),
            this, SLOT(fun4_returnPressed()));
    connect(w->ui->edtFun5, SIGNAL(returnPressed()),
            this, SLOT(fun5_returnPressed()));

    connect(w->ui->edtCmd1, SIGNAL(returnPressed()),
            this, SLOT(edt1_returnPressed()));
    connect(w->ui->edtCmd2, SIGNAL(returnPressed()),
            this, SLOT(edt2_returnPressed()));
    connect(w->ui->edtCmd3, SIGNAL(returnPressed()),
            this, SLOT(edt3_returnPressed()));
    connect(w->ui->edtCmd4, SIGNAL(returnPressed()),
            this, SLOT(edt4_returnPressed()));
    connect(w->ui->edtCmd5, SIGNAL(returnPressed()),
            this, SLOT(edt5_returnPressed()));

    connect(w->ui->btnCmd1, SIGNAL(clicked(bool)),
            this, SLOT(btn1_clicked(bool)));
    connect(w->ui->btnCmd2, SIGNAL(clicked(bool)),
            this, SLOT(btn2_clicked(bool)));
    connect(w->ui->btnCmd3, SIGNAL(clicked(bool)),
            this, SLOT(btn3_clicked(bool)));
    connect(w->ui->btnCmd4, SIGNAL(clicked(bool)),
            this, SLOT(btn4_clicked(bool)));
    connect(w->ui->btnCmd5, SIGNAL(clicked(bool)),
            this, SLOT(btn5_clicked(bool)));

}

//******************************************************************************
void Input::releaseUi(){

    if(!mWin)
        return;

    MainWindow * w = (MainWindow*)mWin;
    disconnect(w->ui->edtFun1);
    disconnect(w->ui->edtFun2);
    disconnect(w->ui->edtFun3);
    disconnect(w->ui->edtFun4);
    disconnect(w->ui->edtFun5);

    disconnect(w->ui->edtCmd1);
    disconnect(w->ui->edtCmd2);
    disconnect(w->ui->edtCmd3);
    disconnect(w->ui->edtCmd4);
    disconnect(w->ui->edtCmd5);

    disconnect(w->ui->btnCmd1);
    disconnect(w->ui->btnCmd2);
    disconnect(w->ui->btnCmd3);
    disconnect(w->ui->btnCmd4);
    disconnect(w->ui->btnCmd5);

    // disconnect all connected signals
    mWin = NULL;
}

//******************************************************************************
void Input::applySettings(Settings * ptr){

    if(ptr && (ptr != &settings))
        settings = *ptr;

    MainWindow * w = (MainWindow*)mWin;
    if(w){
        // adding/removing elements by changing their visibility status
        w->ui->btnCmd1->setVisible(settings.linesCount > 0);
        w->ui->edtCmd1->setVisible(settings.linesCount > 0);
        w->ui->edtFun1->setVisible((settings.linesCount > 0) && telegram);
        w->ui->btnCmd2->setVisible(settings.linesCount > 1);
        w->ui->edtCmd2->setVisible(settings.linesCount > 1);
        w->ui->edtFun2->setVisible((settings.linesCount > 1) && telegram);
        w->ui->btnCmd3->setVisible(settings.linesCount > 2);
        w->ui->edtCmd3->setVisible(settings.linesCount > 2);
        w->ui->edtFun3->setVisible((settings.linesCount > 2) && telegram);
        w->ui->btnCmd4->setVisible(settings.linesCount > 3);
        w->ui->edtCmd4->setVisible(settings.linesCount > 3);
        w->ui->edtFun4->setVisible((settings.linesCount > 3) && telegram);
        w->ui->btnCmd5->setVisible(settings.linesCount > 4);
        w->ui->edtCmd5->setVisible(settings.linesCount > 4);
        w->ui->edtFun5->setVisible((settings.linesCount > 4) && telegram);
    }
}

//******************************************************************************
void Input::load(ConfFile & conf){

    MainWindow * w = (MainWindow*)mWin;
    if(!w)
        return;

    string lastCmd;
    if(conf.get_str("Input", "fun1", &lastCmd))
        w->ui->edtFun1->setText(lastCmd.c_str());
    if(conf.get_str("Input", "fun2", &lastCmd))
        w->ui->edtFun2->setText(lastCmd.c_str());
    if(conf.get_str("Input", "fun3", &lastCmd))
        w->ui->edtFun3->setText(lastCmd.c_str());
    if(conf.get_str("Input", "fun4", &lastCmd))
        w->ui->edtFun4->setText(lastCmd.c_str());
    if(conf.get_str("Input", "fun5", &lastCmd))
        w->ui->edtFun5->setText(lastCmd.c_str());

    if(conf.get_str("Input", "cmd1", &lastCmd))
        w->ui->edtCmd1->setText(lastCmd.c_str());
    if(conf.get_str("Input", "cmd2", &lastCmd))
        w->ui->edtCmd2->setText(lastCmd.c_str());
    if(conf.get_str("Input", "cmd3", &lastCmd))
        w->ui->edtCmd3->setText(lastCmd.c_str());
    if(conf.get_str("Input", "cmd4", &lastCmd))
        w->ui->edtCmd4->setText(lastCmd.c_str());
    if(conf.get_str("Input", "cmd5", &lastCmd))
        w->ui->edtCmd5->setText(lastCmd.c_str());
}

//******************************************************************************
void Input::save(ConfFile & conf){

    MainWindow * w = (MainWindow*)mWin;
    if(!w)
        return;

    string lastCmd;
    lastCmd = w->ui->edtFun1->text().toStdString();
    conf.set_str("Input", "fun1", lastCmd);
    lastCmd = w->ui->edtFun2->text().toStdString();
    conf.set_str("Input", "fun2", lastCmd);
    lastCmd = w->ui->edtFun3->text().toStdString();
    conf.set_str("Input", "fun3", lastCmd);
    lastCmd = w->ui->edtFun4->text().toStdString();
    conf.set_str("Input", "fun4", lastCmd);
    lastCmd = w->ui->edtFun5->text().toStdString();
    conf.set_str("Input", "fun5", lastCmd);

    lastCmd = w->ui->edtCmd1->text().toStdString();
    conf.set_str("Input", "cmd1", lastCmd);
    lastCmd = w->ui->edtCmd2->text().toStdString();
    conf.set_str("Input", "cmd2", lastCmd);
    lastCmd = w->ui->edtCmd3->text().toStdString();
    conf.set_str("Input", "cmd3", lastCmd);
    lastCmd = w->ui->edtCmd4->text().toStdString();
    conf.set_str("Input", "cmd4", lastCmd);
    lastCmd = w->ui->edtCmd5->text().toStdString();
    conf.set_str("Input", "cmd5", lastCmd);
}

//******************************************************************************
void Input::sendString(const QString & fun, const QString & str){
    emit sigSend(fun, str);
}

//******************************************************************************
void Input::fun1_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun1->text(),
                        ((MainWindow*)mWin)->ui->edtCmd1->text());
}
void Input::fun2_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun2->text(),
                        ((MainWindow*)mWin)->ui->edtCmd2->text());
}
void Input::fun3_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun3->text(),
                        ((MainWindow*)mWin)->ui->edtCmd3->text());
}
void Input::fun4_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun4->text(),
                        ((MainWindow*)mWin)->ui->edtCmd4->text());
}
void Input::fun5_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun5->text(),
                        ((MainWindow*)mWin)->ui->edtCmd5->text());
}

//******************************************************************************
void Input::edt1_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun1->text(),
                        ((MainWindow*)mWin)->ui->edtCmd1->text());
}
void Input::edt2_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun2->text(),
                        ((MainWindow*)mWin)->ui->edtCmd2->text());
}
void Input::edt3_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun3->text(),
                        ((MainWindow*)mWin)->ui->edtCmd3->text());
}
void Input::edt4_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun4->text(),
                        ((MainWindow*)mWin)->ui->edtCmd4->text());
}
void Input::edt5_returnPressed(){
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun5->text(),
                        ((MainWindow*)mWin)->ui->edtCmd5->text());
}

//******************************************************************************
void Input::btn1_clicked(bool checked){

    if(cyclic){
        if(checked){
            txCnt[0] = repeat - 1;
            tmr1->start();
        }
        else
            tmr1->stop();


        if(mWin) ((MainWindow*)mWin)->ui->edtCmd1->setEnabled(!checked);
    }
    else{
        if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun1->text(),
                            ((MainWindow*)mWin)->ui->edtCmd1->text());
    }
}
//******************************************************************************
void Input::btn2_clicked(bool checked){
    (void)checked;
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun2->text(),
                        ((MainWindow*)mWin)->ui->edtCmd2->text());
}
//******************************************************************************
void Input::btn3_clicked(bool checked){
    (void)checked;
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun3->text(),
                        ((MainWindow*)mWin)->ui->edtCmd3->text());
}
//******************************************************************************
void Input::btn4_clicked(bool checked){
    (void)checked;
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun4->text(),
                        ((MainWindow*)mWin)->ui->edtCmd4->text());
}
//******************************************************************************
void Input::btn5_clicked(bool checked){
    (void)checked;
    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun5->text(),
                        ((MainWindow*)mWin)->ui->edtCmd5->text());
}
//******************************************************************************
void Input::options(bool cyc, int rep, int period, bool tel){

    if(!cyc){
        if(mWin) ((MainWindow*)mWin)->ui->btnCmd1->setChecked(false);

        tmr1->stop();

        if(mWin) ((MainWindow*)mWin)->ui->edtCmd1->setEnabled(true);
    }

    if(mWin){
        ((MainWindow*)mWin)->ui->btnCmd1->setCheckable(cyc);
    }

    cyclic = cyc;
    repeat = rep;
    telegram = tel;
    applySettings(NULL);    // display FUNction edit boxes based on telegramm
    tmr1->setInterval(period);
}
//******************************************************************************
void Input::tmr1_timeout(){

    if(mWin) sendString(((MainWindow*)mWin)->ui->edtFun1->text(),
                        ((MainWindow*)mWin)->ui->edtCmd1->text());

    if(txCnt[0] > 0)
        txCnt[0]--;
    else if(txCnt[0] == 0){
        tmr1->stop();
        if(mWin) ((MainWindow*)mWin)->ui->btnCmd1->setChecked(false);
        if(mWin) ((MainWindow*)mWin)->ui->edtCmd1->setEnabled(true);
    }
}
//******************************************************************************
void Input::enable(bool state){

    tmr1->stop();

    MainWindow * w = (MainWindow*)mWin;
    if(!w)
        return;

    w->ui->btnCmd1->setChecked(false);
    w->ui->edtCmd1->setEnabled(true);

    w->ui->btnCmd1->setEnabled(state);
    w->ui->btnCmd2->setEnabled(state);
    w->ui->btnCmd3->setEnabled(state);
    w->ui->btnCmd4->setEnabled(state);
    w->ui->btnCmd5->setEnabled(state);
}
