#include "findwidget.h"

FindWidget::FindWidget(QWidget *parent) :
    QWidget(parent)
{
    findEdit = new LineEdit();
    QPushButton *findButton = new QPushButton("Find");
    replaceEdit = new LineEdit();
    QPushButton *replaceButton = new QPushButton("Replace All");

    QHBoxLayout *hbox = new QHBoxLayout(this);
    hbox->setContentsMargins(0, 0, 0, 0);
    hbox->setSpacing(1);
    hbox->addWidget(findEdit);
    hbox->addWidget(findButton);
    hbox->addWidget(replaceEdit);
    hbox->addWidget(replaceButton);

    setLayout(hbox);

    connect(findEdit, SIGNAL(returnPressed()), this, SLOT(find()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(find()));
    connect(replaceEdit, SIGNAL(returnPressed()), this, SLOT(replace()));
    connect(replaceButton, SIGNAL(clicked()), this, SLOT(replace()));
}

void FindWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        e->accept();
        hide();
        return;
    }
    e->ignore();
}

void FindWidget::show()
{
    QWidget::show();
    findEdit->setFocus();
    findEdit->selectAll();
}

void FindWidget::find()
{
    QString needle = findEdit->text();
    if (!needle.isEmpty()) emit findSignal(needle);
    hide();
}

void FindWidget::replace()
{
    QString needle = findEdit->text();
    QString replacement = replaceEdit->text();
    if (!needle.isEmpty()) emit replaceSignal(needle, replacement);
    hide();
}
