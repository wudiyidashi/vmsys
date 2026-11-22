#ifndef MAPUNIT_H
#define MAPUNIT_H

#include  <QQuickPaintedItem>
#include  <QQuickItem>
#include  <QMouseEvent>
#include  <QPainter>
#include  <QSettings>
#include <Robokit/Map/MapManager.h>

class  MapUnit :  public  QQuickPaintedItem
{
    Q_OBJECT
        Q_PROPERTY(int mapwidth READ mapwidth WRITE setMapwidth NOTIFY mapwidthChanged)
        Q_PROPERTY(int mapheight READ mapheight WRITE setMapheight NOTIFY mapheightChanged)
public:
    MapUnit(QQuickItem* parent = nullptr);
    ~MapUnit();
    void DrawArcPath(QPainter* painter, const QList<ArcPath>& path_list);

    void DrawAdvancedPoint(QPainter* painter, const QList<AdvancedPoint>& point_list);

    void DrawNormalPosList(QPainter* painter, const QList<MapPos>& point_list);

    //void DrawAgvPosition(QPainter* painter, const QList<maprecourse::agv::Agv>& agv_list);
    void DrawAgvPosition(QPainter* painter, const AGVPosition& pos);

    void DrawStraightPath(QPainter* painter, const QList<StraightPath>& path_list);

    // override paint() for actual painting.
    virtual   void  paint(QPainter* painter);
    // override contans() for shape mask, Q_INVOKABLE for QML use.
//    Q_INVOKABLE  virtual   bool  contains( const  QPointF &point)  const ;

     virtual   void  mousePressEvent(QMouseEvent *event);
     virtual   void  mouseMoveEvent(QMouseEvent *event);
     virtual   void  mouseReleaseEvent(QMouseEvent *event);
     virtual void  wheelEvent(QWheelEvent *event);
     virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void touchEvent(QTouchEvent *event);

     int mapwidth() const;
     void setMapwidth(int newMapwidth);
     int mapheight() const;
     void setMapheight(int newMapheight);


public slots:
    void drawRectangle(QPointF& point, QPainter* painter);
    void slot_Update();
    void slot_SaveTransformParm();
private:
    QPointer< QSettings> settingsfile;
    QString displayedMap;

    QString m_name;
    QColor m_color;

    int m_mapwidth = 0;
    int m_mapheight = 0;

    QTransform move_transform;
    QTransform mouse_move_transform;
    QTransform scale_transfrom;

    QTransform global_transform;

    QPointF mouse_press;
    QPointF mouse_release;

   QPointF point4;
//    void drawBeser(QPainter *painter);
    void drawBeser(QPointF &startPoint, QPointF &endPoint, QPointF &c1, QPointF &c2, QPainter *painter);
    QPointF pointTransfrom(QPointF &point);
    QPointF mouseTransfromPoint(QPointF &point);

    void DrawBezierPath(QPainter* painter, const QList<BezierPath>& path_list);

    void dealMultiTouch(QPointF p1, QPointF p2, QPointF p3, QPointF p4);
    bool touchBeginEventProcess(QEvent *event);
      bool touchUpdateEventProcess(QEvent *event);
      bool touchEndEventProcess(QEvent *event);
protected:
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
public slots:
    void positionChange();

    void clickToMove(QPointF pos);
    bool initMap();
    void updateMapPos();
signals:
    void SingleClick(QPointF pos);
    void mapwidthChanged();
    void mapheightChanged();
};

#endif // MAPUNIT_H
