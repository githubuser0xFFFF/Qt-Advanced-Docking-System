#ifndef DIGITALCLOCK_H
#define DIGITALCLOCK_H

#include <QLCDNumber>

class CDigitalClock : public QLCDNumber
{
    Q_OBJECT

public:
    CDigitalClock(QWidget *parent = nullptr);

private slots:
    void showTime();
};

#endif // DIGITALCLOCK_H
