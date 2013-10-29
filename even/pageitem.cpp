#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>
#include "pageitem.h"

PageItem::PageItem(fz_context *context, fz_display_list *list, qreal width, qreal height)
{
    ctx = context;
    lst = list;
    pageBounds = QRectF(0, 0, width, height);
}

PageItem::~PageItem()
{
    fz_drop_display_list(ctx, lst);
}

QRectF PageItem::boundingRect() const
{
    return pageBounds;
}

void PageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget)

    const qreal scale = scene()->views().at(0)->transform().m11();
    const qreal inv = 1.0 / scale;
    const fz_matrix transform = {(float)scale, 0, 0, (float)scale, 0, 0};
    const QRectF dirty = option->exposedRect;

    fz_rect bounds;
    bounds.x0 = dirty.x() * scale;
    bounds.y0 = dirty.y() * scale;
    bounds.x1 = bounds.x0 + dirty.width() * scale;
    bounds.y1 = bounds.y0 + dirty.height() * scale;

    fz_irect bbox;
    fz_round_rect(&bbox, &bounds);

    fz_pixmap *pix = fz_new_pixmap_with_bbox(ctx, fz_device_bgr(ctx), &bbox);
    fz_clear_pixmap_with_value(ctx, pix, 255);

    fz_device *dev = fz_new_draw_device(ctx, pix);
    fz_run_display_list(lst, dev, &transform, &bounds, NULL);
    fz_free_device(dev);

    QImage image(pix->samples, bbox.x1 - bbox.x0, bbox.y1 - bbox.y0, QImage::Format_ARGB32);
    painter->setClipRect(boundingRect());
    painter->drawImage(QRectF(bbox.x0 * inv, bbox.y0 * inv, (bbox.x1 - bbox.x0) * inv, (bbox.y1 - bbox.y0) * inv), image);

    fz_drop_pixmap(ctx, pix);
}
