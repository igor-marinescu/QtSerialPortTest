//******************************************************************************
// Includes
//******************************************************************************
#include "utils.h"

//******************************************************************************
// Functions
//******************************************************************************

//******************************************************************************
// Convert a buffer to Hex formated ascii string
// data - input binary buffer
// data_len - length of input binary buffer
// out - output buffer where hex formated ascii string is stored
// out_len - maximal length of output buffer
//******************************************************************************
void binToHexAscii(unsigned char * data,
                 unsigned long data_len,
                 char * out,
                 unsigned long out_len){

    unsigned char chr;
    char * end = out + out_len;

    for(unsigned long i = 0; (i < data_len) && (i < (out_len - 1)); i++){

        // check if there is enough space for one more byte
        // 7 = FF_...\0
        if((end - out) < 7){
            // not enough space, adding "..."
            *out = '.'; ++out;
            *out = '.'; ++out;
            *out = '.'; ++out;
            break;
        }

        // high digit
        chr = ((*data)>>4) & 0x0F;
        if(chr < 10)
            *out = (char)('0' + chr);
        else
            *out = (char)('A' + (chr - 10));

        ++out;

        // low digit
        chr = (*data) & 0x0F;
        if(chr < 10)
            *out = (char)('0' + chr);
        else
            *out = (char)('A' + (chr - 10));

        // not last byte? insert space
        if(i < (data_len - 1)){
            ++out;
            *out = ' ';
        }

        ++out;
        ++data;
    }

    *out = 0;
}

//******************************************************************************
// Function:
// Description:
//******************************************************************************
bool dig_hex_byte(const char ch, unsigned char * data){

        if((ch >= '0') && (ch <= '9'))
            *data |= (ch - '0');
        else if((ch >= 'A') && (ch <= 'F'))
            *data |= (10 + (ch - 'A'));
        else if((ch >= 'a') && (ch <= 'f'))
            *data |= (10 + (ch - 'a'));
        else
            return false;
    return true;
}

//******************************************************************************
// Convert Hex formated QString to a binary buffer.
// out_buff - buffer where the output binary data is stored
// out_len - maximal length of output binary buffer
// str - input hex formated string
// return: bytes stored in out_buff or -1 in case of hex format error
//******************************************************************************
int hexStringToBin(unsigned char * out_buff, int out_len, const QString & str){

    unsigned char data;
    int idx = 0;
    bool status = true;
    const QChar * ptr = str.constData();
    int str_len = str.length();

    if(str_len == 0)
        return 0;

    while((*ptr != '\0') && (status)){

        data = 0;
        while(*ptr == ' ') ++ptr;

        if(*ptr != '\0'){

            status = dig_hex_byte(ptr->toLatin1(), &data);
            data = data << 4;

            ++ptr;
            status = (*ptr != '\0') ? dig_hex_byte(ptr->toLatin1(), &data) : false;

            ++ptr;

            if(status && (idx < out_len)){
                out_buff[idx++] = data;
            }
        }
    }

    if(status)
        return idx;

    return -1;
}

//******************************************************************************
// Convert a string Hex formated string to a Byte.
// out - pointer where converted byte will be stored
// str - string containing the hex byte to be converted
// returns: true - if a hex byte successfully converted
//          false - in case of conversion errors
//******************************************************************************
bool hexStringToByte(unsigned char * out, const QString & str){

    const QChar * ptr = str.constData();

    *out = 0;

    while(*ptr == ' ') ++ptr;

    if(!dig_hex_byte(ptr->toLatin1(), out))
        return false;

    ++ptr;
    if((*ptr == ' ') || (*ptr == '\0'))
        return true;

    *out = *out << 4;
    if(!dig_hex_byte(ptr->toLatin1(), out))
        return false;

    ++ptr;
    if((*ptr == ' ') || (*ptr == '\0'))
        return true;

    return false;
}

//******************************************************************************
// Function:    bcc
// Description: Calculate bcc for a buffer.
// Parameters:  buff - buffer of data to calculate BCC
//              len - number of elements in buffer
// Returs:      calculated bcc
//******************************************************************************
unsigned char bcc(unsigned char * buff, int len){

    unsigned char bcc = 0;
    for(int i = 0; i < len; ++i)
        bcc ^= buff[i];

    return bcc;
}

//******************************************************************************
// Convert a binary buffer to ascii string
// data - input binary buffer
// data_len - length of input binary buffer
// out - output buffer where ascii text is stored
// out_len - maximal length of output buffer
//******************************************************************************
void binToAscii(unsigned char * data,
                 unsigned long data_len,
                 char * out,
                 unsigned long out_len){

    for(unsigned long i = 0; (i < data_len) && (i < (out_len - 1)); i++){

        if((*data >= 32) || (*data == '\r') || (*data == '\n')){
            *out = (char) *data;
        }
        else{
            *out = '.';
        }

        ++out;
        ++data;
    }

    *out = 0;
}

//******************************************************************************
// Display an ascii string to TextEdit
// out - TextEdit where the ascii is displayed
// in - input ascii string
// max_len - maximal length of input ascii string
//******************************************************************************
void asciiToTextEdit(QTextEdit * out, char * in, int max_len){

    QString txtOut;
    for(int i = 0; i < max_len; i++){
        if(in[i] == 0){
            break;
        }
        else if(in[i] == '\n'){
            out->append(txtOut);
            txtOut.clear();
        }
        else if(in[i] == '\r'){
            // do nothing
        }
        else {
            txtOut += QString(QChar(in[i]));
        }
    }

    if(!txtOut.isEmpty()){
        out->append(txtOut);
    }
}

//******************************************************************************
// Display an binary string to TextEdit in a hex format with ascii field
// out - TextEdit where the ascii is displayed
// in - input binary buffer
// max_len - maximal length of input binary buffer
//******************************************************************************
void binToTextEdit(QTextEdit * out, unsigned char * in, int in_len, bool with_ascii){

    unsigned char chr;
    QString txtOut;
    char chout;

    (void) with_ascii;

    for(int i = 0; i < in_len; i++){

        // high digit
        chr = ((*in)>>4) & 0x0F;
        if(chr < 10)
            chout = (char)('0' + chr);
        else
            chout = (char)('A' + (chr - 10));

        txtOut += QString(QChar(chout));

        // low digit
        chr = (*in) & 0x0F;
        if(chr < 10)
            chout = (char)('0' + chr);
        else
            chout = (char)('A' + (chr - 10));

        txtOut += QString(QChar(chout));

        // not last byte? insert space
        if(i < (in_len - 1)){
            txtOut += QString(" ");
        }

        ++in;
    }

    if(!txtOut.isEmpty()){
        out->append(txtOut);
    }
}
