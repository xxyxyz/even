#include "lineedit.h"

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    setAttribute(Qt::WA_MacShowFocusRect, false);

    connect(this, SIGNAL(textEdited(QString)), this, SLOT(textEdited(QString)));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    undoAvailableState = false;
    redoAvailableState = false;
    copyAvailableState = false;
}

void LineEdit::textEdited(const QString &text)
{
    Q_UNUSED(text)
    if (undoAvailableState != isUndoAvailable()) {
        undoAvailableState = isUndoAvailable();
        emit undoAvailable(undoAvailableState);
    }
    if (redoAvailableState != isRedoAvailable()) {
        redoAvailableState = isRedoAvailable();
        emit redoAvailable(redoAvailableState);
    }
}

void LineEdit::selectionChanged()
{
    if (copyAvailableState != hasSelectedText()) {
        copyAvailableState = hasSelectedText();
        emit copyAvailable(copyAvailableState);
    }
}
