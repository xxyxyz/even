#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEdit(QWidget *parent = 0);

signals:
    void undoAvailable(bool available);
    void redoAvailable(bool available);
    void copyAvailable(bool yes);

private:
    bool undoAvailableState;
    bool redoAvailableState;
    bool copyAvailableState;

public slots:
    void textEdited(const QString &text);
    void selectionChanged();
};

#endif // LINEEDIT_H
