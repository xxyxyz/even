#ifndef GOWIDGET_H
#define GOWIDGET_H

#include <QWidget>
#include <QtWidgets>
#include "lineedit.h"

class GoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GoWidget(QWidget *parent = 0);

signals:
    void goSignal(int line);

protected:
    void keyPressEvent(QKeyEvent *e);

public slots:
    void show();

private slots:
    void submit();

private:
    LineEdit *goEdit;
};

#endif // GOWIDGET_H
