#ifndef HIDINGLABEL_H
#define HIDINGLABEL_H

#include <QLabel>

class HidingLabel : public QLabel
{
    Q_OBJECT
public:
    explicit HidingLabel(QWidget *parent = 0);
    bool isShowing();

public slots:
    void setText(const QString &text);

protected:
    void timerEvent(QTimerEvent *e);

private:
    int timer;
};

#endif // HIDINGLABEL_H
