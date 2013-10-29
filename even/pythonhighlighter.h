#ifndef PYTHONHIGHLIGHTER_H
#define PYTHONHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class PythonHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit PythonHighlighter(QTextDocument *parent = 0);
    bool isComment(const int position);

protected:
    void highlightBlock(const QString &text);

private:
    QTextCharFormat commentFormat;
    QTextCharFormat decoratorFormat;
    QTextCharFormat selfFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat identifierFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat stringFormat;

    QRegExp pattern;

    QRegExp singleSingleQuotes;
    QRegExp tripleSingleQuotes;
    QRegExp singleDoubleQuotes;
    QRegExp tripleDoubleQuotes;
};

#endif // PYTHONHIGHLIGHTER_H
