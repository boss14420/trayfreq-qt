#ifndef FREQTRAY_H
#define FREQTRAY_H

#include <QSystemTrayIcon>
#include <QAction>

#include <cstdlib>

class QActionGroup;
class QMenu;
class QTimer;

class FreqTray : public QSystemTrayIcon
{
    Q_OBJECT

    static unsigned int ncpu;
    static QIcon FreqIcon[5];
public:
    explicit FreqTray(QSystemTrayIcon *parent = 0);
    ~FreqTray();

    static void initIcon();

private:
    void initMenu();
//    void updateMenu();
    bool event (QEvent *event);

    static unsigned int getNumCpu();


    QActionGroup *actions;
    QMenu *menu;
    QTimer *updateIconTimer;

private slots:
    void updateIcon();
    void newPolicySelected(QAction*);

};

class SetFreqAction : public QAction {
    Q_OBJECT

    unsigned long freq;

public:
    explicit SetFreqAction(unsigned long, QObject *parent = 0);
    unsigned long getFreq() { return freq; }
};

class SetGovernorAction : public QAction {
    Q_OBJECT

    char *governor;
public:
    explicit SetGovernorAction(char const*, QObject *parent = 0);
    ~SetGovernorAction() { std::free(governor); }

    char* govr() { return governor; }
};

#endif // FREQTRAY_H
