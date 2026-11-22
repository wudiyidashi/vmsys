#ifndef ARROWHEADS_H
#define ARROWHEADS_H

#include  <QQuickPaintedItem>
class ArrowHeads : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
    QML_ELEMENT
    public:
        ArrowHeads(QQuickItem *parent = 0);
        void paint(QPainter *painter);

        const QColor &getColor() const;
        void setColor(const QColor &newColor);

private:
        QColor color;


signals:

        void colorChanged();
};
//! [0]


//class ArrowHeads : public QQuickPaintedItem
//{
//    Q_OBJECT


//public:
//    virtual void paint(QPainter *painter);
//    ArrowHeads(QQuickItem *parent = nullptr);

//signals:

//};

#endif // ARROWHEADS_H
