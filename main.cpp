#include <QtGui/QApplication>
#include "freqtray.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    FreqTray::initIcon ();

    FreqTray w;
    w.show();
    
    return a.exec();
}
