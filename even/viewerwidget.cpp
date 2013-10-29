#include "viewerwidget.h"

#define SPACING 8

ViewerWidget::ViewerWidget(QWidget *parent) :
    QWidget(parent)
{
    scales << 0.0625 << 0.125 << 0.25 << 0.5 << 1.0 << 2.0 << 4.0 << 8.0 << 16.0;

    scene = new QGraphicsScene(this);

    view = new QGraphicsView(scene, this);
    view->setFocusPolicy(Qt::NoFocus);
    view->setFrameShape(QFrame::NoFrame);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->viewport()->setAutoFillBackground(false);
    view->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(view);
    setLayout(vbox);

    context = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
}

ViewerWidget::~ViewerWidget()
{
    scene->clear();
    fz_free_context(context);
}

void ViewerWidget::load(const QByteArray &data)
{
    scene->clear();
    QList<QGraphicsItem *> items;
    QPixmap pixmap;
    if (pixmap.loadFromData(data)) {
        items << new QGraphicsPixmapItem(pixmap);
    }
    else if (data.startsWith("%PDF")) {
        fz_stream *stream = fz_open_memory(context, (unsigned char *)data.constData(), data.length());
        fz_document *doc = fz_open_document_with_stream(context, ".pdf", stream);
        fz_close(stream);
        int pagecount = fz_count_pages(doc);
        for (int i = 0; i < pagecount; i++) {
            fz_page *page = fz_load_page(doc, i);
            fz_rect bounds;
            fz_bound_page(doc, page, &bounds);
            fz_display_list *list = fz_new_display_list(context);
            fz_device *dev = fz_new_list_device(context, list);
            fz_run_page(doc, page, dev, &fz_identity, NULL);
            fz_free_device(dev);
            fz_free_page(doc, page);
            PageItem *item = new PageItem(context, list, bounds.x1 - bounds.x0, bounds.y1 - bounds.y0);
            item->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
            items << item;
        }
        fz_close_document(doc);
    } else {
        scene->setSceneRect(0, 0, 0, 0);
        return;
    }
    int top = 0;
    QPen outline(Qt::lightGray, 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
    outline.setCosmetic(true);
    foreach (QGraphicsItem *item, items) {
        QGraphicsRectItem *rim = new QGraphicsRectItem(item->boundingRect());
        item->setPos(0, top);
        rim->setPos(0, top);
        rim->setPen(outline);
        rim->setBrush(Qt::NoBrush);
        scene->addItem(rim);
        scene->addItem(item);
        top += item->boundingRect().height() + SPACING;
    }
    scene->setSceneRect(scene->itemsBoundingRect().adjusted(-8, -8, 8, 8));
}

void ViewerWidget::zoomReset()
{
    zoom(1.0);
}

void ViewerWidget::zoomWidth()
{
    zoom(view->width() / scene->width());
}

void ViewerWidget::zoomIn()
{
    zoomNext(+1);
}

void ViewerWidget::zoomOut()
{
    zoomNext(-1);
}

void ViewerWidget::zoom(qreal scale)
{
    if (scene->items().isEmpty()) return;
    if (qFuzzyCompare(scale, view->transform().m11())) return;
    QGraphicsPixmapItem *item = qgraphicsitem_cast<QGraphicsPixmapItem *>(scene->items().first());
    if (item) {
        Qt::TransformationMode mode;
        if (qFuzzyCompare(scale, 1.0) || scale > 1.0) {
            mode = Qt::FastTransformation;
        } else {
            mode = Qt::SmoothTransformation;
        }
        if (mode != item->transformationMode()) {
            item->setTransformationMode(mode);
        }
    }
    view->setTransform(QTransform::fromScale(scale, scale));
    emit zoomSignal(scale);
}

void ViewerWidget::zoomNext(int step)
{
    if (scene->items().isEmpty()) return;
    qreal scale = view->transform().m11();
    qreal distance = qAbs(scale - scales.at(0));
    int index = 0;
    for (int i = 1; i < scales.count(); i++) {
        qreal d = qAbs(scale - scales.at(i));
        if (d < distance) {
            distance = d;
            index = i;
        }
    }
    if (!qFuzzyCompare(scale, scales.at(index))) {
        if ((step < 0) == (scale > scales.at(index))) {
            step = 0;
        }
    }
    if (index + step < 0 || index + step == scales.count()) return;
    scale = scales.at(index + step);
    zoom(scale);
}
