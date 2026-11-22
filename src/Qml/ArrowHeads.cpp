#include "ArrowHeads.h"

#include <QLinearGradient>
#include <QPainterPath>
#include <QPen>
#include <qpainter.h>


ArrowHeads::ArrowHeads(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
}
//! [0]
//! [1]
void ArrowHeads::paint(QPainter *painter)
{
    QBrush brush(color);
    painter->setBrush(brush);
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    QSizeF itemSize = size();

        const QPointF points[6] = {
            QPointF(itemSize.width(), itemSize.height()*0.5),
            QPointF(itemSize.width()*0.66, itemSize.height()),
            QPointF(0, itemSize.height()),
            QPointF(itemSize.width()*0.4, itemSize.height()*0.5),
            QPointF(0, 0),
            QPointF(itemSize.width()*0.66, 0)
        };
        painter->drawConvexPolygon(points, 6);

}

const QColor &ArrowHeads::getColor() const
{
    return color;
}

void ArrowHeads::setColor(const QColor &newColor)
{
    if (color == newColor)
        return;
    color = newColor;
    emit colorChanged();
}


//ArrowHeads::ArrowHeads(QQuickItem *parent)
//    : QQuickPaintedItem(parent)
//{

//}

//void  ArrowHeads::paint(QPainter *painter)
//{

//    QPainterPath path;
//    path.addRect(20, 20, 60, 60);

//    path.moveTo(0, 0);
//    path.cubicTo(99, 0,  50, 50,  99, 99);
//    path.cubicTo(0, 99,  50, 50,  0, 0);


//    painter->fillRect(0, 0, 100, 100, Qt::white);
//    painter->setPen(QPen(QColor(79, 106, 25), 1, Qt::SolidLine,
//                        Qt::FlatCap, Qt::MiterJoin));
//    painter->setBrush(QColor(122, 163, 39));

//    painter->drawPath(path);

//    QLinearGradient myGradient;
//    QPen myPen;
//    QPolygonF myPolygon;
//    myPolygon << QPointF(10.4, 20.5) << QPointF(20.2, 30.2);
//    QPainterPath myPath;
//    myPath.addPolygon(myPolygon);

//    painter->setBrush(myGradient);
//    painter->setPen(QPen(QColor(79, 106, 25), 1, Qt::SolidLine,
//                        Qt::FlatCap, Qt::MiterJoin));
//    painter->setBrush(QColor(122, 163, 39));

//    painter->drawPath(myPath);
//}

