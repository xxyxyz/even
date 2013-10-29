#include "movingsplitter.h"

MovingSplitter::MovingSplitter(QWidget *parent) :
    QSplitter(parent)
{
}

void MovingSplitter::moveEvent(QMoveEvent *e)
{
    QWidget::moveEvent(e);
    emit moved(e);
}
