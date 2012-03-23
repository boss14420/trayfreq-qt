#include "freqtray.h"

#include <QActionGroup>
#include <QMenu>
#include <QTimer>
#include <QEvent>

#include <QMenuBar>
#include <QMainWindow>

#include <cpufreq.h>
#include <cstring>


unsigned int FreqTray::ncpu = getNumCpu();
QIcon FreqTray::FreqIcon[5];

FreqTray::FreqTray(QSystemTrayIcon *parent) :
    QSystemTrayIcon(parent),
    updateIconTimer(new QTimer(this))
//    ui(new Ui::FreqTray)
{
//    ui->setupUi(this);

    initMenu();
//    updateMenu();
    setContextMenu(menu);

//    QMainWindow *mw = new QMainWindow();
//    menu->setTitle ("menu");
//    mw->menuBar ()->addMenu(menu);
//    mw->setVisible (true);
//    mw->show ();

    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(newPolicySelected(QAction*)));

    updateIcon();

    // Auto update icon
    connect(updateIconTimer, SIGNAL(timeout()), this, SLOT(updateIcon()));
    updateIconTimer->setInterval (1500);
    updateIconTimer->start ();

    setVisible(true);
}

FreqTray::~FreqTray() {
    menu->deleteLater ();
}

void FreqTray::initIcon() {
    FreqIcon[0] = QIcon(":/freq/res/cpufreq-0.png");
    FreqIcon[1] = QIcon(":/freq/res/cpufreq-25.png");
    FreqIcon[2] = QIcon(":/freq/res/cpufreq-50.png");
    FreqIcon[3] = QIcon(":/freq/res/cpufreq-75.png");
    FreqIcon[4] = QIcon(":/freq/res/cpufreq-100.png");
}

void FreqTray::updateIcon () {
    static int lastIconIndex = -1;

    unsigned long cpufreqmin, cpufreqmax;
    cpufreq_get_hardware_limits (0, &cpufreqmin,&cpufreqmax);

    unsigned long averagefreq = 0;
    for(unsigned int cpu = 0; cpu < ncpu; ++cpu) {
       averagefreq += cpufreq_get_freq_kernel (cpu);
    }
    averagefreq /= ncpu;

    float times = (float) averagefreq / cpufreqmax;
    int newIndex;
    if(times < .125) newIndex = 0;
    else if(times < .375) newIndex = 1;
    else if(times < .625) newIndex = 2;
    else if(times < .875) newIndex = 3;
    else	newIndex = 4;

    if(newIndex != lastIconIndex)
        setIcon (FreqIcon[lastIconIndex = newIndex]);
}

void FreqTray::initMenu() {
    menu = new QMenu();

// Avail freq
    struct cpufreq_available_frequencies *freq, *frqtmp;
    frqtmp = freq = cpufreq_get_available_frequencies(0);
    do {
        menu->addAction(new SetFreqAction(frqtmp->frequency, menu));
        frqtmp = frqtmp->next;
    } while(frqtmp);
    cpufreq_put_available_frequencies(freq);

    menu->addSeparator()->setText (tr("Governor"));

// Governor
    actions = new QActionGroup(this);
    actions->setExclusive (true);

    struct cpufreq_available_governors *govr, *gvrtmp;
    gvrtmp	= govr = cpufreq_get_available_governors(0);
    QAction *act;
    do {
        act = new SetGovernorAction(gvrtmp->governor, menu);
        actions->addAction(act);
        menu->addAction(act);

        act->setCheckable (true);
        if(!std::strcmp(cpufreq_get_policy (0)->governor, gvrtmp->governor))
            act->setChecked (true);

        gvrtmp = gvrtmp->next;
    } while(gvrtmp);
    cpufreq_put_available_governors(govr);
}

//void FreqTray::updateMenu() {
    //static QList< menuActions

void FreqTray::newPolicySelected(QAction *act) {
    SetFreqAction *sfa;
    if((sfa = dynamic_cast<SetFreqAction*>(act)) != NULL) {
        for(unsigned int cpu = 0; cpu < ncpu; ++cpu)
            cpufreq_set_frequency (cpu, sfa->getFreq());
    } else {
        SetGovernorAction *sga = dynamic_cast<SetGovernorAction*>(act);
        for(unsigned int cpu = 0; cpu < ncpu; ++cpu)
            cpufreq_modify_policy_governor (cpu, sga->govr());
    }
}

bool FreqTray::event (QEvent *event) {
    static const unsigned long THOUNSAND = 1000;
    static const float MILLION = 1000.0*THOUNSAND;

    if(event->type () == QEvent::ToolTip) {
        unsigned long freq = cpufreq_get_freq_kernel (0);
        QString toolTipText = QString("CPU 1: %1 %2")
                    .arg ((freq < MILLION) ? freq/THOUNSAND : freq/MILLION)
                    .arg ((freq < MILLION) ? "MHz" : "GHz");

        for(unsigned int cpu = 1; cpu < ncpu; ++cpu) {
            unsigned long freq = cpufreq_get_freq_kernel (cpu);
            toolTipText += QString("\nCPU %1: %2 %3").arg (cpu+1)
                    .arg ((freq < MILLION) ? freq/THOUNSAND : freq/MILLION)
                    .arg ((freq < MILLION) ? "MHz" : "GHz");
        }
        setToolTip (toolTipText);
    }
    return QSystemTrayIcon::event (event);
}

unsigned int FreqTray::getNumCpu() {
    int n = 0;
    while(!cpufreq_cpu_exists(n)) ++n;
    return n;
}

SetFreqAction::SetFreqAction(unsigned long newfreq, QObject *parent /*= 0*/)
    : QAction(parent), freq(newfreq)
{
    if(freq < 1000000) {
        setText (QString::number (freq/1000) + " MHz");
    } else if(freq < 1000000000) {
        setText (QString::number (freq/1000000.0) + " GHz");
    }
}

SetGovernorAction::SetGovernorAction(char const *newgvr, QObject *parent /*= 0*/)
    :  QAction(parent), governor(strdup (newgvr))
{
    setText (QString(governor));
}
