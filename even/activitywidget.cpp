#include <QColor>
#include <QPainter>
#include "activitywidget.h"

ActivityWidget::ActivityWidget(QWidget *parent) :
    QWidget(parent)
{
    done = true;
    timer = 0;
    angle = 0;
}

bool ActivityWidget::isDone()
{
    return done;
}

void ActivityWidget::startAnimating()
{
    if (isHidden()) show();
    if (timer == 0) timer = startTimer(40);
    done = false;
    angle = 0;
    update();
}

void ActivityWidget::stopAnimating(bool forced)
{
    done = !forced;
    if (done) hide();
    if (timer == 0) return;
    killTimer(timer);
    timer = 0;
}

void ActivityWidget::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e)
    angle = (angle + 30) % 360;
    update();
}

void ActivityWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    int side = qMin(width(), height()) - 2;
    qreal width = side / 4.0;
    qreal height = side / 20.0;
    QRectF tick = QRectF(width, -height, width, 2.0*height);
    QColor fill = QColor(Qt::black);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    for (int i=0; i<12; i++) {
        fill.setAlpha(255 - (i*255)/12);
        painter.save();
        painter.translate(rect().center());
        painter.rotate(angle - i*30);
        painter.fillRect(tick, fill);
        painter.restore();
    }
}
