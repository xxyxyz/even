#include <QKeyEvent>
#include "baseedit.h"

BaseEdit::BaseEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

void BaseEdit::keyPressEvent(QKeyEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {
        switch (e->key()) {
        // Navigation
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
        // File
        case Qt::Key_N:
        case Qt::Key_O:
        case Qt::Key_W:
        case Qt::Key_S:
        case Qt::Key_E:
        // Edit
        case Qt::Key_Z:
        case Qt::Key_X:
        case Qt::Key_C:
        case Qt::Key_V:
        case Qt::Key_A:
        case Qt::Key_F:
        case Qt::Key_G:
        case Qt::Key_L:
        case Qt::Key_BracketLeft:
        case Qt::Key_BracketRight:
        case Qt::Key_Slash:
        case Qt::Key_D:
        // View
        case Qt::Key_0:
        case Qt::Key_9:
        case Qt::Key_Plus:
        case Qt::Key_Minus:
        // Script
        case Qt::Key_R:
            break;
        default:
            return;
        }
    }
    QTextEdit::keyPressEvent(e);
}
