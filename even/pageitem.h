#ifndef PAGEITEM_H
#define PAGEITEM_H

#include <QGraphicsItem>
#include <QPainter>

extern "C" {
    #include <mupdf/fitz.h>
}

class PageItem : public QGraphicsItem
{
public:
    PageItem(fz_context *context, fz_display_list *list, qreal width, qreal height);
    ~PageItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    enum { Type = UserType + 1 };
    int type() const
    {
        return Type;
    }

private:
    fz_context *ctx;
    QRectF pageBounds;
    fz_display_list *lst;
};

#endif // PAGEITEM_H
