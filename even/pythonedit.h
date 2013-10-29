#ifndef PYTHONEDIT_H
#define PYTHONEDIT_H

#include "baseedit.h"
#include "pythonhighlighter.h"

#define NORMALIZE(string) string.replace("\r", "\n").replace("\t", "    ")

class PythonEdit : public BaseEdit
{
    Q_OBJECT
public:
    explicit PythonEdit(QWidget *parent = 0);

protected:
    void keyPressEvent(QKeyEvent *e);
    void insertFromMimeData(const QMimeData *source);
    void paintEvent(QPaintEvent *e);
    QTextBlock firstVisibleBlock() const;

public slots:
    void dedent();
    void indent();
    void comment();
    void duplicate();

private slots:
    void positionChanged();
    void selectionChanged();

private:
    void findSelectedBlocks(QTextBlock &start, QTextBlock &end);
    void insertOrRemove(bool insert, const QString &text, const QRegExp &pattern, const int offset = 0);

private:
    QString needle;
    PythonHighlighter *highlighter;
};

#endif // PYTHONEDIT_H
