#include <QApplication>
#include <QProcess>
#include <QFileInfo>
#include <QStringList>
#include "freqtray.h"

bool isRunning(QString const &executable)
{
    QStringList executableList(executable);

    QProcess pgrep;
    pgrep.setReadChannel (QProcess::StandardOutput);
    pgrep.start ("pgrep", executableList);
    pgrep.waitForFinished(300);

    QByteArray pid = pgrep.readAll();

    return !pid.isEmpty();
}

int main(int argc, char *argv[])
{
    if (isRunning (QFileInfo (argv[0]).fileName())) 
        return 0;

    QApplication a(argc, argv);

    FreqTray::initIcon ();

    FreqTray w;
    w.show();
    
    return a.exec();
}
