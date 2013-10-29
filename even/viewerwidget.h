#ifndef VIEWERWIDGET_H
#define VIEWERWIDGET_H

#include <QWidget>
#include <QtWidgets>
#include "pageitem.h"

class ViewerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewerWidget(QWidget *parent = 0);
    ~ViewerWidget();
    void load(const QByteArray &data);

signals:
    void zoomSignal(qreal scale);

public slots:
    void zoomReset();
    void zoomWidth();
    void zoomIn();
    void zoomOut();

private:
    void zoom(qreal scale);
    void zoomNext(int step);

private:
    QVector<qreal> scales;
    QGraphicsScene *scene;
    QGraphicsView *view;
    fz_context *context;
};

#endif // VIEWERWIDGET_H
