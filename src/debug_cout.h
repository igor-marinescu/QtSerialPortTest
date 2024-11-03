//******************************************************************************
// Compiler:        Microsoft Visual C++ 2010 Express
//
// Description:     Class used for debuging.
//                  to a file. This is useful for debuging.
// Notes:
//                  How to use it:
//                  --------------
//                  To debug some info use LOG(x) statement, ex:
//                  DBG_INFO("Hello world");
//                  DBG_INFO("x+y="<<std::dec<<(x+y));
//
//                  In order to activate de debugging, use DEBUGON define.
//                  #define DEBUGON
//                  or, for QT define the following in the .pro file:
//                      DEFINES += DEBUGON
//
//                  The log goes to output stream (std::cout).
//                  To redirect the output stream to a file (debug.txt)
//                  use DEBUGCOUT define:
//                  #define DEBUGCOUT
//                  or, for QT define the following in the .pro file:
//                      DEFINES += DEBUGCOUT
//
// Marinescu Igor
// 16.09.2013
//******************************************************************************
#ifndef DEBUG_COUT_H
#define DEBUG_COUT_H

//******************************************************************************
// Includes
//******************************************************************************
#include <iostream>     // std::streambuf, std::cout
#include <fstream>      // std::ofstream

//******************************************************************************
// Defines
//******************************************************************************
#ifdef DEBUGON
    #define DBG_ENTER(y)        std::cout<<"[enter] "<<y<<std::endl
    #define DBG_EXIT(y)         std::cout<<"[exit]  "<<y<<std::endl
    #define DBG_ERROR(y)        std::cout<<"[error] "<<y<<std::endl
    #define DBG_INFO(y)         std::cout<<"[info]  "<<y<<std::endl
#else
    #define DBG_ENTER(y)
    #define DBG_EXIT(y)
    #define DBG_ERROR(y)
    #define DBG_INFO(y)
#endif

//******************************************************************************
class DebugCout{
private:
    std::ofstream dfile;        // debug file
    std::streambuf *dout_buf;   // debug output buffer
    std::streambuf *cout_buf;   // cout output buffer
public:
    DebugCout();
    virtual ~ DebugCout();
};

//******************************************************************************
#endif // DEBUG_COUT_H
