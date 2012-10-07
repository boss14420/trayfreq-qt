#include <QApplication>
#include <QProcess>
#include <QFileInfo>
#include <QStringList>
#include <QTextStream>
#include "freqtray.h"

bool isRunning(QString const &executable)
{
    QStringList executableList(executable);

    QProcess pgrep;
    pgrep.setReadChannel (QProcess::StandardOutput);
    pgrep.start ("pgrep", executableList);
    pgrep.waitForFinished(100);

    QByteArray pids = pgrep.readAll();
    QTextStream stream (&pids, QIODevice::ReadOnly);

    int pid;
    while ( !(stream >> pid).atEnd() )
        if (pid != qApp->applicationPid())
            return true;

    return false;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (isRunning (QFileInfo (argv[0]).fileName())) 
        return 0;

    FreqTray::initIcon ();

    FreqTray w;
    w.show();
    
    return a.exec();
}
