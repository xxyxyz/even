#ifndef MOVINGSPLITTER_H
#define MOVINGSPLITTER_H

#include <QSplitter>

class MovingSplitter : public QSplitter
{
    Q_OBJECT
public:
    explicit MovingSplitter(QWidget *parent = 0);

signals:
    void moved(QMoveEvent *);

protected:
    void moveEvent(QMoveEvent *e);
};

#endif // MOVINGSPLITTER_H
