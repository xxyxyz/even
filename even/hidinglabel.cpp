#include "hidinglabel.h"

HidingLabel::HidingLabel(QWidget *parent) :
    QLabel(parent)
{
    timer = 0;
}

bool HidingLabel::isShowing()
{
    return timer != 0;
}

void HidingLabel::setText(const QString &text)
{
    if (timer != 0) killTimer(timer);
    timer = startTimer(2500);
    QLabel::setText(text);
    if (isHidden()) show();
}

void HidingLabel::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e)

    killTimer(timer);
    timer = 0;
    hide();
}
