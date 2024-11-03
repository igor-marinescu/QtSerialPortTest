#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

//******************************************************************************
// Includes
//******************************************************************************
#include <QDialog>
#include "Serial.h"
#include "udp.h"
#include "AppInterface.h"
#include "output.h"
#include "input.h"

namespace Ui {
class SettingsDialog;
}

//******************************************************************************
// Class
//******************************************************************************
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:

    // true when serial settings changed
    bool serSettingsChanged;
    bool udpSettingsChanged;

    explicit SettingsDialog(QWidget *parent,
                            AppInterface::Settings * app,
                            Serial::Settings * ser,
                            UDP::Settings * udp,
                            int selIfc,
                            Output::Settings * out,
                            Input::Settings * in
                            );
    ~SettingsDialog();

private slots:

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void on_cmbboxPort_currentTextChanged(const QString &arg1);
    void on_cmbboxBaudrate_currentIndexChanged(int index);
    void on_cmbboxParity_currentIndexChanged(int index);

    void on_edtDstAddr_editingFinished();
    void on_edtDstPort_editingFinished();
    void on_edtSrcPort_editingFinished();

private:
    Ui::SettingsDialog *ui;

    // Serial settings
    Serial::Settings *  serSettingsPtr;
    Serial::Settings    serSettingsNew;

    // UDP settings
    UDP::Settings * udpSettingsPtr;
    UDP::Settings   udpSettingsNew;

    // Application settings
    AppInterface::Settings * appSettingsPtr;
    AppInterface::Settings   appSettingsNew;

    // Output settings
    Output::Settings * outSettingsPtr;
    Output::Settings   outSettingsNew;

    // Input settings
    Input::Settings * inSettingsPtr;
    Input::Settings   inSettingsNew;

    bool inConstructor;
};

//******************************************************************************
#endif // SETTINGSDIALOG_H
