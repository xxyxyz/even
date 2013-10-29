#include <QtWidgets>
#include "pythonedit.h"

PythonEdit::PythonEdit(QWidget *parent) :
    BaseEdit(parent)
{
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(positionChanged()));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    highlighter = new PythonHighlighter(document());
}

void PythonEdit::keyPressEvent(QKeyEvent *e)
{
    if (e == QKeySequence::MoveToStartOfLine || e == QKeySequence::SelectStartOfLine) {
        QTextCursor cursor = textCursor();
        QString text = cursor.block().text();
        QRegExp pattern("^ *");
        pattern.indexIn(text);
        const bool move = e == QKeySequence::MoveToStartOfLine;
        int position = cursor.positionInBlock();
        int index = pattern.matchedLength();
        int offset = 0 < position && position <= index ? 0 : index;
        cursor.setPosition(cursor.block().position() + offset, move ? QTextCursor::MoveAnchor : QTextCursor::KeepAnchor);
        setTextCursor(cursor);
        return;
    }
    else if (e == QKeySequence::MoveToPreviousLine || e == QKeySequence::MoveToNextLine) {
        if (textCursor().hasSelection()) {
            QTextCursor cursor = textCursor();
            cursor.setPosition(e == QKeySequence::MoveToPreviousLine ? cursor.selectionStart() : cursor.selectionEnd());
            setTextCursor(cursor);
            return;
        }
    }
    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return: {
        QTextCursor cursor = textCursor();
        QString text = cursor.block().text();
        text.resize(cursor.positionInBlock());
        QString indent = "\n";
        if (QRegExp("[^#]*[:(\[{] *(#|$)").exactMatch(text)) {
            indent.append("    ");
        }
        QRegExp pattern("^ *");
        pattern.indexIn(text);
        cursor.insertText(indent + pattern.cap());
        ensureCursorVisible();
        return;
    }
    case Qt::Key_Tab: {
        QTextCursor cursor = textCursor();
        cursor.insertText(QString(4 - cursor.positionInBlock() % 4, ' '));
        return;
    }
    case Qt::Key_Left:
    case Qt::Key_Backspace:
    case Qt::Key_Right:
    case Qt::Key_Delete: {
        QTextCursor cursor = textCursor();
        if (cursor.hasSelection() && !(e->modifiers() & Qt::ShiftModifier)) {
            BaseEdit::keyPressEvent(e);
            return;
        }
        const bool left = e->key() == Qt::Key_Left || e->key() == Qt::Key_Backspace;
        int index = cursor.positionInBlock();
        int count = left ? index % 4 : 4 - index % 4;
        if (left && count == 0) count = 4;
        index += cursor.block().position();
        int i = 0;
        for (; i < count; i++) {
            int offset = left ? index - i - 1 : index + i;
            if (document()->characterAt(offset) != ' ') break;
            BaseEdit::keyPressEvent(e);
        }
        if (i == 0) BaseEdit::keyPressEvent(e);
        return;
    }
    }
    BaseEdit::keyPressEvent(e);
}

void PythonEdit::insertFromMimeData(const QMimeData *source)
{
    if (source->hasText()) {
        QString text = source->text();
        NORMALIZE(text);
        textCursor().insertText(text);
        return;
    }
    BaseEdit::insertFromMimeData(source);
}

void PythonEdit::paintEvent(QPaintEvent *e)
{
    if (!needle.isEmpty()) {
        QPainter painter(viewport());
        QRectF geometry(viewport()->geometry());
        const int dx = horizontalScrollBar()->value();
        const int dy = verticalScrollBar()->value();
        QTextBlock block = firstVisibleBlock();
        while (block.isValid()) {
            QPointF position = block.layout()->position();
            if (position.y() - dy > geometry.height()) break;
            QTextLine line = block.layout()->lineAt(0);
            QString text = block.text();
            int index = qMax(0, line.xToCursor(dx) - needle.length());
            index = text.indexOf(needle, index, Qt::CaseSensitive);
            while (index != -1) {
                QRectF r = line.naturalTextRect();
                r.setLeft(line.cursorToX(index));
                r.setRight(line.cursorToX(index + needle.length()));
                r.translate(position);
                r.translate(-dx, -dy);
                if (r.left() > geometry.right()) break;
                painter.fillRect(r, Qt::yellow);
                index = text.indexOf(needle, index + needle.length(), Qt::CaseSensitive);
            }
            block = block.next();
        }
    }
    BaseEdit::paintEvent(e);
}

QTextBlock PythonEdit::firstVisibleBlock() const
{
    int count = document()->blockCount();
    int offset = verticalScrollBar()->value();
    int height = document()->size().height();
    return document()->findBlockByNumber(count * offset / height);
}

void PythonEdit::dedent()
{
    insertOrRemove(false, QString(""), QRegExp("^  ? ? ?"));
}

void PythonEdit::indent()
{
    insertOrRemove(true, QString("    "), QRegExp());
}

void PythonEdit::comment()
{
    QTextBlock start, end;
    findSelectedBlocks(start, end);

    int width = 10000;
    bool comment = false;
    QRegExp pattern("[^ ]");
    QTextBlock block = start;
    while (true) {
        QString text = block.text();
        int index = pattern.indexIn(text);
        if (index == -1) {
            comment = true;
            width = qMin(width, text.length());
        } else {
            if (text.at(index) != '#') comment = true;
            width = qMin(width, index);
        }
        if (block == end) break;
        block = block.next();
    }
    insertOrRemove(comment, QString("# "), QRegExp("# ?"), width);
}

void PythonEdit::duplicate()
{
    QTextBlock start, end;
    findSelectedBlocks(start, end);

    QTextCursor cursor = textCursor();
    int anchor = cursor.anchor();
    int position = cursor.position();
    cursor.setPosition(start.position());
    cursor.setPosition(end.position(), QTextCursor::KeepAnchor);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    QString text = cursor.selectedText();
    cursor.clearSelection();
    cursor.insertText("\n" + text);
    cursor.setPosition(anchor);
    cursor.setPosition(position, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
}

void PythonEdit::positionChanged()
{
    QList<QTextEdit::ExtraSelection> selections;
    if (extraSelections().count()) {
        setExtraSelections(selections);
    }
    if (textCursor().hasSelection()) {
        return;
    }
    int pos = textCursor().position();
    QChar bracket = document()->characterAt(pos);
    if (bracket != '(' && bracket != ')' && bracket != '[' && bracket != ']' && bracket != '{' && bracket != '}') {
        pos--;
        bracket = document()->characterAt(pos);
    }
    QChar opposite;
    int direction;
    switch (bracket.toLatin1()) {
    case '(': opposite = ')'; direction = 1; break;
    case ')': opposite = '('; direction = -1; break;
    case '[': opposite = ']'; direction = 1; break;
    case ']': opposite = '['; direction = -1; break;
    case '{': opposite = '}'; direction = 1; break;
    case '}': opposite = '{'; direction = -1; break;
    default: return;
    }
    if (highlighter->isComment(pos)) return;
    int end = -1, count = 0;
    int stop = direction == 1 ? document()->characterCount() : -1;
    for (int i=pos; i != stop; i += direction) {
        QChar ch = document()->characterAt(i);
        if (ch == bracket && !highlighter->isComment(i)) {
            count++;
        } else if (ch == opposite && !highlighter->isComment(i)) {
            count--;
            if (count == 0) {
                end = i;
                break;
            }
        }
    }
    if (end != -1) {
        QTextEdit::ExtraSelection sel;
        sel.format.setBackground(QColor(Qt::yellow));
        sel.cursor = textCursor();
        sel.cursor.setPosition(pos);
        sel.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        selections.append(sel);
        sel.cursor.setPosition(end);
        sel.cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        selections.append(sel);
        setExtraSelections(selections);
    }
}

void PythonEdit::selectionChanged()
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection() && cursor.block() == document()->findBlock(cursor.anchor())) {
        needle.replace(0, needle.length(), cursor.selectedText());
    } else {
        needle.clear();
    }
    viewport()->update();
}

void PythonEdit::findSelectedBlocks(QTextBlock &start, QTextBlock &end)
{
    QTextCursor cursor = textCursor();
    start = document()->findBlock(cursor.selectionStart());
    end = document()->findBlock(cursor.selectionEnd());
    if (cursor.hasSelection() && end.position() == cursor.selectionEnd()) {
        end = end.previous();
    }
}

void PythonEdit::insertOrRemove(const bool insert, const QString &text, const QRegExp &pattern, const int offset)
{
    QTextBlock start, end;
    findSelectedBlocks(start, end);

    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    QTextBlock block = start;
    while (true) {
        if (insert) {
            cursor.setPosition(block.position() + offset);
            cursor.insertText(text);
        } else {
            QString text = block.text();
            int index = pattern.indexIn(text);
            if (index != -1) {
                cursor.setPosition(block.position() + index);
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, pattern.matchedLength());
                cursor.removeSelectedText();
            }
        }
        if (block == end) break;
        block = block.next();
    }
    cursor.endEditBlock();
}
