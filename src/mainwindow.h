#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//******************************************************************************
#include <QMainWindow>
#include "AppInterface.h"
#include "Serial.h"
#include "udp.h"
#include "output.h"
#include "options.h"
#include "input.h"

//******************************************************************************
namespace Ui {
//******************************************************************************
class MainWindow;
}

//******************************************************************************
class MainWindow : public QMainWindow
{
    Q_OBJECT
    QTimer * rxToutTmr;

public:
    explicit MainWindow(int argc, char *argv[], QWidget *parent = 0);
    ~MainWindow();

    Ui::MainWindow *ui;

private slots:

    void serErrorEvent(ComInterface * ifc, int code);
    void serChangeEvent(ComInterface * ifc);
    void serRxEvent(ComInterface * ifc, unsigned char * data, int len);
    void serTxEvent(ComInterface * ifc, unsigned char * data, int len);

    void on_btnOpen_clicked();

    void on_btnSettings_clicked();

    void on_comboBoxIfc_currentIndexChanged(int index);

    void sendString(const QString & fun, const QString & str);
    void optionCyclicChanged();
    void optionTelegramChanged();

    void rxToutTmr_timeout();

private:

    AppInterface * appInterface;

    bool opened;
    Serial serial;
    Serial::Settings serSettingsNew;        // New serial settings

    UDP udp;
    UDP::Settings udpSettingsNew;           // New UDP settings

    Output output;
    Input input;
    Options options;

    unsigned char txBuff[COM_BUFF_LEN];
    unsigned char rxBuff[COM_BUFF_LEN];
    int rxBuffLen;

    void showActSettings();
};

//******************************************************************************
#endif // MAINWINDOW_H
