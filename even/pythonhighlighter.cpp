#include <QtWidgets>
#include "pythonhighlighter.h"

PythonHighlighter::PythonHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    commentFormat.setForeground(QColor(140, 166, 242));
    decoratorFormat.setForeground(Qt::black);
    decoratorFormat.setFontWeight(QFont::Bold);
    selfFormat.setForeground(QColor(0, 0, 153));
    keywordFormat.setForeground(Qt::black);
    keywordFormat.setFontWeight(QFont::Bold);
    identifierFormat.setForeground(QColor(0, 0, 153));
    identifierFormat.setFontWeight(QFont::Bold);
    numberFormat.setForeground(QColor(217, 0, 0));
    stringFormat.setForeground(QColor(25, 51, 229));

    QString comment = "(#.*)";
    QString decorator = "(@[a-zA-Z0-9_.]+)";
    QString self = "\\b(self)\\b";
    QString keyword = ("\\b(False|None|True|and|as|assert|break|continue|del|elif|else|except|exec|finally|for|"
                       "from|global|if|import|in|is|lambda|not|or|pass|print|raise|return|try|while|with|yield)\\b"); // Python 2
//    QString keyword = ("\\b(False|None|True|and|as|assert|break|continue|del|elif|else|except|finally|for|from|"
//                       "global|if|import|in|is|lambda|nonlocal|not|or|pass|raise|return|try|while|with|yield)\\b"); // Python 3
    QString definition = "\\b(def|class)\\b *([a-zA-Z_]\\w*)?";
    QString a = "\\d+(?:\\.\\d*)?(?:[eE][+-]?\\d+[jJ]?|[jJ])";
    QString b = "\\d+\\.(?:\\d+\\b)?|\\.\\d+(?:[eE][+-]?\\d+)?[jJ]?";
    QString i = "(?:[1-9][0-9]*|0[oO]?[0-7]+|0[xX][0-9a-fA-F]+|0[bB][01]+|0)[lL]?";
    QString number = QString("((?: +[+-])?(?:\\b%1\\b|\\b%2\\b|\\b%3\\b))").arg(a).arg(b).arg(i);
    QString string = "[uUbB]?[rR]?('|'''|\"|\"\"\")";

    pattern = QRegExp(comment + "|" + decorator + "|" + self + "|" + keyword + "|" + definition + "|" + number + "|" + string);

    singleSingleQuotes = QRegExp("'");
    tripleSingleQuotes = QRegExp("'''");
    singleDoubleQuotes = QRegExp("\"");
    tripleDoubleQuotes = QRegExp("\"\"\"");
}

bool PythonHighlighter::isComment(const int position)
{
    QTextBlock block = document()->findBlock(position);
    QList<QTextLayout::FormatRange> formats = block.layout()->additionalFormats();
    if (formats.isEmpty()) return false;
    QTextLayout::FormatRange last = formats.last();
    if (last.format == commentFormat && last.start <= (position - block.position())) return true;
    return false;
}

void PythonHighlighter::highlightBlock(const QString &text)
{
    int currentState = previousBlockState();
    int index = 0, length = 0;
    int oldIndex = 0;

    do {
        index += length;

        switch (currentState) {

        case -1:
            index = pattern.indexIn(text, index);
            length = pattern.matchedLength();
            if (index != -1) {

                if (index > 0 && text.at(index - 1) == QChar('\\')) {
                    length = 1;
                    continue;
                }

                if (pattern.pos(1) != -1) {
                    setFormat(index, length, commentFormat);
                }
                else if (pattern.pos(2) != -1) {
                    setFormat(index, length, decoratorFormat);
                }
                else if (pattern.pos(3) != -1) {
                    setFormat(index, length, selfFormat);
                }
                else if (pattern.pos(4) != -1) {
                    setFormat(index, length, keywordFormat);
                }
                else if (pattern.pos(5) != -1) {
                    setFormat(pattern.pos(5), pattern.cap(5).length(), keywordFormat);
                    setFormat(pattern.pos(6), pattern.cap(6).length(), identifierFormat);
                }
                else if (pattern.pos(7) != -1) {
                    setFormat(index, length, numberFormat);
                }
                else {
                    oldIndex = index;
                    int offset = pattern.pos(8);
                    if (text.at(offset) == QChar('\'')) {
                        currentState = index+length-offset == 1 ? 1 : 2;
                    } else {
                        currentState = index+length-offset == 1 ? 3 : 4;
                    }
                }

            }
            continue;

        case 1:
            index = singleSingleQuotes.indexIn(text, index);
            length = singleSingleQuotes.matchedLength();
            break;
        case 2:
            index = tripleSingleQuotes.indexIn(text, index);
            length = tripleSingleQuotes.matchedLength();
            break;
        case 3:
            index = singleDoubleQuotes.indexIn(text, index);
            length = singleDoubleQuotes.matchedLength();
            break;
        case 4:
            index = tripleDoubleQuotes.indexIn(text, index);
            length = tripleDoubleQuotes.matchedLength();
            break;

        }
        if (index != -1) {

            if (index > 0 && text.at(index - 1) == QChar('\\')) {
                length = 1;
                continue;
            }

            setFormat(oldIndex, index + length - oldIndex, stringFormat);
            currentState = -1;
        }

    } while (index != -1);

    if (currentState != -1) {
        setFormat(oldIndex, text.length() - oldIndex, stringFormat);
    }
    if (currentState == 1 || currentState == 3) {
        int end = text.length() - 1;
        int i = end;
        while (i>=0 && text.at(i)==QChar('\\')) i--;
        if ((end - i) % 2 == 0) currentState = -1;
    }

    setCurrentBlockState(currentState);
}
