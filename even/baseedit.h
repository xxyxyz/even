#ifndef BASEEDIT_H
#define BASEEDIT_H

#include <QTextEdit>

class BaseEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit BaseEdit(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // BASEEDIT_H
