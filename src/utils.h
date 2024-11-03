#ifndef UTILS_H
#define UTILS_H
//******************************************************************************
// Includes
//******************************************************************************
#ifndef __wtypes_h__
#include <wtypes.h>
#endif

#ifndef __WINDEF_
#include <windef.h>
#endif

#include <QString>
#include <QWidget>
#include <QTextEdit>

//******************************************************************************
// Defines
//******************************************************************************
//---------------------------------------------------------------------------
// Widget Settings
//---------------------------------------------------------------------------
typedef struct {
    int Top;
    int Left;
    int Width;
    int Height;
    enum {
        WS_NORMAL,
        WS_MINIMIZED,
        WS_MAXIMIZED
    } State;
} WidgetSettings;

//******************************************************************************
// Exported functions
//******************************************************************************
void binToHexAscii(unsigned char * data, unsigned long data_len,
                 char * out, unsigned long out_len);
bool dig_hex_byte(const char ch, unsigned char * data);
int hexStringToBin(unsigned char * out_buff, int out_len, const QString & str);
bool hexStringToByte(unsigned char * out, const QString & str);

unsigned char bcc(unsigned char * buff, int len);

void binToAscii(unsigned char * data, unsigned long data_len,
                 char * out, unsigned long out_len);

void asciiToTextEdit(QTextEdit * out, char * in, int in_max);
void binToTextEdit(QTextEdit * out, char * in, int in_max, bool with_ascii);

//******************************************************************************
#endif // UTILS_H
