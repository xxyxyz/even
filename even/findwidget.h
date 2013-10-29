#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include <QWidget>
#include <QtWidgets>
#include "lineedit.h"

class FindWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FindWidget(QWidget *parent = 0);

signals:
    void findSignal(const QString &needle);
    void replaceSignal(const QString &needle, const QString &replacement);

protected:
    void keyPressEvent(QKeyEvent *e);

public slots:
    void show();

private slots:
    void find();
    void replace();

private:
    LineEdit *findEdit;
    LineEdit *replaceEdit;
};

#endif // FINDWIDGET_H
