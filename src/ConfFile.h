//******************************************************************************
// Configuration File
//
// Igor Marinescu
//******************************************************************************
#ifndef VARFBOX_H
#define VARFBOX_H

//******************************************************************************
// Includes
//******************************************************************************
#include <map>
#include <iostream>
#include <QString>

using namespace std;

//******************************************************************************
//******************************************************************************
class ConfFile{

public:

    class ConfObj {
    public:

        enum {INTEGER, STRING, BYTEARRAY } type;
        int varInteger;
        string varString;

        // constructors
        ConfObj(){varInteger = 0; type = ConfObj::INTEGER; }
        ConfObj(int var){varInteger = var; type = ConfObj::INTEGER; }
        ConfObj(string str){varString = str; type = ConfObj::STRING; }

        // assigment operator
        ConfObj& operator=(const int & other);
        ConfObj& operator=(const string & other);

        // conversion operator
        operator int();
        operator string();

        friend std::ostream& operator<<(std::ostream& os, const ConfObj & obj);
    };

private:

    typedef map<string, ConfObj> MapNameObj;
    typedef map<string, MapNameObj> MapCatName;
    MapCatName varMap;

public:

    string filename;

    ConfFile(){}
    ConfFile(const ConfFile& other);
    ConfFile& operator=(const ConfFile& other);
    ConfFile(const char * filename);
    virtual ~ConfFile();

    bool exists(const string & cat);
    bool exists(const string & cat, const string & name);

    void set(const string & cat, const string & name, const int val);
    void set_str(const string & cat, const string & name, const string & val);
    void set_qstr(const char * cat, const char * name, const QString & qstr);

    bool get(const string & cat, const string & name, int * val);
    int  get(const string & cat, const string & name, int default_value = 0);

    bool get_str(const string & cat, const string & name, string * val = NULL);
    QString get_qstr(const char * cat, const char * name, const char * def = NULL);

    void clear();
    void clear(const string & cat);
    void clear(const string & cat, const string & name);
};

#endif // VARFBOX_H
