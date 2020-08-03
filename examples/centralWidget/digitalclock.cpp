#include "digitalclock.h"

#include <QTime>
#include <QTimer>

CDigitalClock::CDigitalClock(QWidget *parent)
    : QLCDNumber(parent)
{
    setDigitCount(8);
    setSegmentStyle(Filled);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CDigitalClock::showTime);
    timer->start(1000);

    showTime();

    setWindowTitle(tr("Digital Clock"));
    resize(150, 60);
}

void CDigitalClock::showTime()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm:ss");
    display(text);
}
