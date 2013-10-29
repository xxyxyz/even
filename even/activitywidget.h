#ifndef ACTIVITYWIDGET_H
#define ACTIVITYWIDGET_H

#include <QWidget>

class ActivityWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ActivityWidget(QWidget *parent = 0);
    bool isDone();

public slots:
    void startAnimating();
    void stopAnimating(bool forced);

protected:
    void timerEvent(QTimerEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    bool done;
    int timer;
    int angle;
};

#endif // ACTIVITYWIDGET_H
