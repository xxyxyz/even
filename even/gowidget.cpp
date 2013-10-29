#include "gowidget.h"

GoWidget::GoWidget(QWidget *parent) :
    QWidget(parent)
{
    goEdit = new LineEdit();
    QPushButton *goButton = new QPushButton("Go");

    QHBoxLayout *hbox = new QHBoxLayout(this);
    hbox->setContentsMargins(0, 0, 0, 0);
    hbox->setSpacing(1);
    hbox->addWidget(goEdit);
    hbox->addWidget(goButton);

    setLayout(hbox);

    connect(goEdit, SIGNAL(returnPressed()), this, SLOT(submit()));
    connect(goButton, SIGNAL(clicked()), this, SLOT(submit()));
}

void GoWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        e->accept();
        hide();
        return;
    }
    e->ignore();
}

void GoWidget::show()
{
    QWidget::show();
    goEdit->setFocus();
    goEdit->selectAll();
}

void GoWidget::submit()
{
    int line = goEdit->text().toInt();
    if (line > 0) emit goSignal(line);
    hide();
}
