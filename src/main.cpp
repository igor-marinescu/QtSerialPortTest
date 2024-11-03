#include "mainwindow.h"
#include <QApplication>

// Detect memory leaks
/*
#if defined(WIN32) && defined(_DEBUG)
   #define _CRTDBG_MAP_ALLOC
   #include <stdlib.h>
   #include <crtdbg.h>
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif
*/

int main(int argc, char *argv[])
{

// Detect memory leaks
/*
#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
*/

    QApplication a(argc, argv);
    MainWindow w(argc, argv);
    w.show();

    return a.exec();
}
