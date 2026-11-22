#include "Mission/MissionManager.h"
#include <Robokit/Map/MapUnit.h>

#include<Robokit/RobokitApi/RobotStatus.h>
#include  <QPainter>
#include  <QPainterPathStroker>
#include  <QDebug>
#include <QSGNode>
#include <QSGRectangleNode>
#include <QSGSimpleRectNode>
#include <QPointF>
#include <QElapsedTimer>
#include <algorithm>
#include <cmath>
#include <Misc/QIniConfig.h>
#include <Mission/MissionManager.h>



MapUnit::MapUnit(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton
                            | Qt::RightButton
                            | Qt::MiddleButton);
    setFlag(ItemAcceptsInputMethod, true);
    settingsfile = new QSettings(QCoreApplication::applicationDirPath() + "/map_config.ini",
                                 QSettings::IniFormat, this);
    auto &map = SeerrMap::GetInstance();
    connect(&map, &SeerrMap::PositionChange, this, &MapUnit::positionChange);
    connect(&map, &SeerrMap::MapLoadFinished, this, &MapUnit::initMap);
    connect(&map, &SeerrMap::mapReadyChanged, this, &MapUnit::positionChange);
    connect(this, &MapUnit::SingleClick, this, &MapUnit::clickToMove);
}

MapUnit::~MapUnit()
{

}
void MapUnit::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    if (displayedMap.isEmpty())
        initMap();
}

bool MapUnit::initMap()
{
    const auto &header = SeerrMap::GetInstance().MapHeader;
    if (header.mapName.isEmpty() || width() <= 0 || height() <= 0)
        return false;

    if (displayedMap != header.mapName) {
        settingsfile->beginGroup("MAP");
        const auto savedMap = settingsfile->value("map_transform.name").toString();
        const double savedScale = settingsfile->value("map_transform.scale").toDouble();
        const double dx = settingsfile->value("map_transform.dx").toDouble();
        const double dy = settingsfile->value("map_transform.dy").toDouble();
        settingsfile->endGroup();

        if ((savedMap == header.mapName || (savedMap.isEmpty() && displayedMap.isEmpty()))
            && std::isfinite(savedScale) && savedScale != 0
            && std::isfinite(dx) && std::isfinite(dy)) {
            // Older settings used the scale sign to encode the map's orientation.
            move_transform = savedScale < 0 ? QTransform::fromScale(1, -1)
                                            : QTransform::fromScale(-1, 1);
            scale_transfrom = QTransform::fromScale(std::abs(savedScale), std::abs(savedScale));
            mouse_move_transform = QTransform::fromTranslate(dx, dy);
        } else {
            const double mapWidth = std::max(1.0, header.maxPos.x - header.minPos.x);
            const double mapHeight = std::max(1.0, header.maxPos.y - header.minPos.y);
            const double scale = std::min(width() * 0.85 / mapWidth, height() * 0.85 / mapHeight);
            const double midX = (header.minPos.x + header.maxPos.x) / 2;
            const double midY = (header.minPos.y + header.maxPos.y) / 2;
            move_transform = QTransform::fromScale(1, -1);
            scale_transfrom = QTransform::fromScale(scale, scale);
            mouse_move_transform = QTransform::fromTranslate(width() / 2 - midX * scale,
                                                            height() / 2 + midY * scale);
        }
        displayedMap = header.mapName;
    }

    updateMapPos();
    update();
    return true;
}

void MapUnit::slot_SaveTransformParm()
{
    if (displayedMap.isEmpty())
        return;

    settingsfile->beginGroup("MAP");
    settingsfile->setValue("map_transform.name", displayedMap);
    settingsfile->setValue("map_transform.dx", mouse_move_transform.dx());
    settingsfile->setValue("map_transform.dy", mouse_move_transform.dy());
    const double scale = scale_transfrom.m22();
    settingsfile->setValue("map_transform.scale", move_transform.m11() > 0 ? -scale : scale);
    settingsfile->endGroup();
}

//用来改变qml中滚动条位置
void MapUnit::updateMapPos()
{
    SeerrMap& map= SeerrMap::GetInstance();
    QPointF maxPos = QPointF(map.MapHeader.maxPos.x,map.MapHeader.maxPos.y);
    maxPos = pointTransfrom(maxPos);
    QPointF minPos = QPointF(map.MapHeader.minPos.x,map.MapHeader.minPos.y);
    minPos = pointTransfrom(minPos);
    setMapwidth(std::abs(maxPos.x() - minPos.x()));
    setMapheight(std::abs(maxPos.y() - minPos.y()));
}
void MapUnit::drawBeser(QPointF& startPoint,QPointF& endPoint,QPointF& c1,QPointF& c2,QPainter *painter) {
    QPainterPath pathTest;
//    painter->drawEllipse(startPoint, 2, 2);
//    painter->drawEllipse(endPoint, 2, 2);
//    painter->drawEllipse(c1, 2, 2);
//    painter->drawEllipse(c2, 2, 2);
    pathTest.moveTo(startPoint);
    pathTest.cubicTo(c1, c2, endPoint);

//    pointPercent = pathTest.pointAtPercent(_percent);
//    painter->drawEllipse(pointPercent, 5, 5);
    QPen ellipsePen;
    ellipsePen.setWidth(6);
    ellipsePen.setColor(QColor("#409F41"));
    ellipsePen.setStyle(Qt::DashLine);
    painter->setPen(ellipsePen);

    painter->drawPath(pathTest);
}

void MapUnit::drawRectangle(QPointF& point,QPainter *painter) {
    qreal width = 1*scale_transfrom.m11();
    qreal height = 1*scale_transfrom.m11();
    qreal x =point.x()-width/2;
    qreal y =point.y()-height/2;
    QRect rec(x,y,width,height);
    QPen ellipsePen;
    ellipsePen.setWidth(6);
    ellipsePen.setColor(QColor("#409F41"));
    ellipsePen.setStyle(Qt::DashLine);
    painter->setPen(ellipsePen);
    painter->drawRect(rec);
    painter->fillRect(rec,QColor("#CCFF99"));

}

int MapUnit::mapwidth() const
{
    return m_mapwidth;
}

void MapUnit::setMapwidth(int newMapwidth)
{
    if (m_mapwidth == newMapwidth)
        return;
    m_mapwidth = newMapwidth;
    emit mapwidthChanged();
}

int MapUnit::mapheight() const
{
    return m_mapheight;
}

void MapUnit::setMapheight(int newMapheight)
{
    if (m_mapheight == newMapheight)
        return;
    m_mapheight = newMapheight;
    emit mapheightChanged();
}

void MapUnit::slot_Update()
{
	this->update();
}

QPointF MapUnit::pointTransfrom(QPointF& point)
{
	return mouse_move_transform.map(scale_transfrom.map(move_transform.map(point)));
}

QPointF MapUnit::mouseTransfromPoint(QPointF& point)
{
	return move_transform.inverted().map(scale_transfrom.inverted().map(mouse_move_transform.inverted().map(point)));
}

void MapUnit::DrawBezierPath(QPainter* painter, const QList<BezierPath>& path_list)
{
	painter->resetTransform();
	//绘制贝塞尔曲线
	foreach(BezierPath bzPath, path_list) {
		//         qDebug()<<bzPath.instanceName;

		//        qDebug()<<bzPath.startPos.pos.x<<":"<<bzPath.startPos.pos.y<<"贝塞尔点";
		//        qDebug()<<bzPath.endPos.pos.x<<":"<<bzPath.endPos.pos.y<<"贝塞尔结束点";

		QPointF starPoint(bzPath.startPos.pos.x, bzPath.startPos.pos.y);
		starPoint = pointTransfrom(starPoint);
		QPointF endPoint(bzPath.endPos.pos.x, bzPath.endPos.pos.y);
		endPoint = pointTransfrom(endPoint);
		QPointF c1(bzPath.controlPos1.x, bzPath.controlPos1.y);
		c1 = pointTransfrom(c1);
		QPointF c2(bzPath.controlPos2.x, bzPath.controlPos2.y);
		c2 = pointTransfrom(c2);
		//        painter->setPen(Qt::black);
		//        painter->setFont(QFont("Arial",14));
		//        QRect rec(QPoint(starPoint.x(),starPoint.y()),QSize(100,100));
		//        painter->drawText(rec,Qt::AlignHCenter | Qt::AlignVCenter,bzPath.instanceName);
		drawBeser(starPoint, endPoint, c1, c2, painter);
	}
}
void MapUnit::DrawArcPath(QPainter* painter, const QList<ArcPath>& path_list)
{
	painter->resetTransform();
	//绘制普通曲线
	foreach(ArcPath arcPath, path_list) {
		QPointF starPoint(arcPath.startPos.pos.x, arcPath.startPos.pos.y);
		starPoint = pointTransfrom(starPoint);
		QPointF endPoint(arcPath.endPos.pos.x, arcPath.endPos.pos.y);
		endPoint = pointTransfrom(endPoint);
		QPointF c1(arcPath.controlPos1.x, arcPath.controlPos1.y);
		c1 = pointTransfrom(c1);

		QPainterPath pathTest;
		pathTest.moveTo(starPoint);
		pathTest.quadTo(c1, endPoint);

		QPen ellipsePen;
		ellipsePen.setWidth(6);
		ellipsePen.setColor(QColor("#409F41"));
		ellipsePen.setStyle(Qt::DashLine);
		painter->setPen(ellipsePen);
		painter->drawPath(pathTest);
	}
	//qDebug()<<"普通直线耗时"<<ElapsedTimer.elapsed()<<"毫秒";
}
void MapUnit::DrawStraightPath(QPainter* painter, const QList<StraightPath>& path_list)
{
	painter->resetTransform();
	//绘制直线
	foreach(StraightPath straightPath, path_list) {
		QPointF starPoint(straightPath.startPos.pos.x, straightPath.startPos.pos.y);
		starPoint = pointTransfrom(starPoint);
		QPointF endPoint(straightPath.endPos.pos.x, straightPath.endPos.pos.y);
		endPoint = pointTransfrom(endPoint);
		painter->drawLine(starPoint, endPoint);
	}
	//qDebug()<<"直线耗时"<<ElapsedTimer.elapsed()<<"毫秒";
}
void MapUnit::DrawAdvancedPoint(QPainter* painter, const QList<AdvancedPoint>& point_list)
{

	painter->resetTransform();
	//绘制站点
	foreach(AdvancedPoint advpoint, point_list)
	{
		//        qDebug()<<advpoint.pos.x,advpoint.pos.y;
		QPointF point(advpoint.pos.x, advpoint.pos.y);

		point = pointTransfrom(point);

		//          drawRectangle(point,painter);
		painter->translate(point.x(), point.y());
		painter->rotate(-advpoint.dir / 3.1415926 * 180);
		qreal _width = 0.7 * scale_transfrom.m11();
		qreal _height = 0.7 * scale_transfrom.m11();
		qreal x = -_width / 2;
		qreal y = -_height / 2;
		QRect rec(x, y, _width, _height);


		if (advpoint.className == "LocationMark")
		{
			painter->fillRect(rec, QColor("#C9F970"));
		}
		else if (advpoint.className == "ActionPoint")
		{
			painter->fillRect(rec, QColor("#F2A577"));
		}

        if (fabs(scale_transfrom.m11()) > 66)
		{
			//绘制边框虚线
			QPen ellipsePen;
			ellipsePen.setWidth(6);
			ellipsePen.setColor(QColor("#409F41"));
			ellipsePen.setStyle(Qt::DashLine);
			painter->setPen(ellipsePen);
			painter->drawRect(rec);

			//绘制站点名
			painter->rotate(advpoint.dir / 3.1415926 * 180);//反转 保证站点名正面朝界面
			painter->setPen(Qt::black);
            painter->setFont(QFont("Arial", 20));
			painter->drawText(rec, Qt::AlignHCenter | Qt::AlignVCenter, advpoint.instanceName);
//			QRect rec2(x, y - _height, _width, _height);
//			painter->drawText(rec2, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(advpoint.vertex));
		}
		else
		{
			QPen ellipsePen;
			ellipsePen.setWidth(1);
			ellipsePen.setColor(QColor("#409F41"));
			ellipsePen.setStyle(Qt::DashLine);
			painter->setPen(ellipsePen);

			painter->drawRect(rec);
		}
		painter->resetTransform();
	}
	//qDebug()<<"站点耗时"<<ElapsedTimer.elapsed()<<"毫秒";
}
void MapUnit::DrawNormalPosList(QPainter* painter, const QList<MapPos>& point_list)
{
	//地图障碍点绘制
	QColor color = QColor("#696969");
	//耗时100ms
	for (int var = 0; var < point_list.count(); var += 5) {

		QPointF point(point_list[var].x, point_list[var].y);
		point = pointTransfrom(point);
		QRect rec(point.x(), point.y(), 3, 3);
		painter->fillRect(rec, color);
	}

	painter->resetTransform();
}

void MapUnit::DrawAgvPosition(QPainter* painter, const AGVPosition& pos)
{
	//绘制车位置

		//qDebug()<< agv.x<<"+"<< agv.y;
		QPointF point(pos.pos.x, pos.pos.y);
		
		point = pointTransfrom(point);

		qreal width = 0.7 * scale_transfrom.m11();
		qreal height = 0.7 * scale_transfrom.m11();
		qreal x = -width / 2;
		qreal y = -height / 2;
		//        qreal x =point.x()-width/2;
		//        qreal y =point.y()-height/2;
		painter->translate(point.x(), point.y());
        painter->rotate(-pos.angle / 3.1415926 * 180);
		QRect rec(x, y, width, height);
		painter->fillRect(rec, QColor(0, 0, 255, 160));

		//painter->setPen(Qt::white);
		//painter->setFont(QFont("Arial", 14));
		//painter->drawText(rec, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(agv.id));
		const QPointF points[6] = {
             QPointF(-width / 4,0),
        QPointF(0,-height / 4),
        QPointF(0,height / 4)

		};
		QBrush brush(QColor("#ff00ff"));
		painter->setBrush(brush);
		painter->drawConvexPolygon(points, 3);

		painter->resetTransform();
	
}

void  MapUnit::paint(QPainter* painter)
{
	//#define DEBUG
#ifdef DEBUG
	QElapsedTimer ElapsedTimer;
	ElapsedTimer.start();
	/*耗时操作......*/
	//    qDebug()<<"开始耗时"<<ElapsedTimer.elapsed()<<"毫秒";
#endif // DEBUG
	//防止图形走样
	painter->setRenderHint(QPainter::Antialiasing, true);
	SeerrMap& map = SeerrMap::GetInstance();
	DrawBezierPath(painter, map.BezierPathList);
#ifdef DEBUG
	qDebug() << "贝塞尔耗时" << ElapsedTimer.elapsed() << "毫秒";
#endif // DEBUG
	DrawArcPath(painter, map.ArcPathList);
#ifdef DEBUG
	qDebug() << "普通曲线" << ElapsedTimer.elapsed() << "毫秒";
#endif // DEBUG
	DrawStraightPath(painter, map.StraightPathList);
#ifdef DEBUG
	qDebug() << "直线曲线" << ElapsedTimer.elapsed() << "毫秒";
#endif // DEBUG
	DrawAdvancedPoint(painter, map.AdvancedPointList);
#ifdef DEBUG
	qDebug() << "高级点" << ElapsedTimer.elapsed() << "毫秒";
#endif // DEBUG
	DrawNormalPosList(painter, map.normalPosList);
#ifdef DEBUG
	qDebug() << "普通点" << ElapsedTimer.elapsed() << "毫秒";
#endif // DEBUG
	if (map.isMapReady() && map.hasPosition())
		DrawAgvPosition(painter, map.curpos);
#ifdef DEBUG
	qDebug() << "AGV" << ElapsedTimer.elapsed() << "毫秒";
#endif // DEBUG  


	//绘制移动参数
	QSizeF itemSize = size();

	qreal _width = 120;
	qreal _height = 20;
	qreal x = -_width / 2;
	qreal y = -_height / 2;
	QRect rec(20, itemSize.height() - _height, _width, _height);
	QRect rec2(20, itemSize.height() - 2 * _height, _width, _height);
	QRect rec3(20, itemSize.height() - 3 * _height, _width, _height);
	//绘制站点名
	painter->setPen(Qt::black);
	painter->setFont(QFont("Arial", 14));
	//平移参数x
	painter->drawText(rec, Qt::AlignHCenter | Qt::AlignVCenter, "x " + QString::number(mouse_move_transform.dx()));
	//平移参数y
	painter->drawText(rec2, Qt::AlignHCenter | Qt::AlignVCenter, "y " + QString::number(mouse_move_transform.dy()));
	//缩放参数
	painter->drawText(rec3, Qt::AlignHCenter | Qt::AlignVCenter, "scale " + QString::number(scale_transfrom.m22()));



}


void  MapUnit::mousePressEvent(QMouseEvent* event)
{
	event->accept();
	mouse_press = event->localPos();
}

void  MapUnit::mouseMoveEvent(QMouseEvent* event)
{

	event->accept();
	QPointF pos = event->localPos();
	mouse_move_transform.translate(pos.x() - mouse_press.x(), pos.y() - mouse_press.y());
	mouse_press = event->localPos();
	update();
}

void  MapUnit::mouseReleaseEvent(QMouseEvent* event)
{
	event->accept();
	QPointF pos = event->localPos();

	//在析构函数和退出函数中执行保存参数无效，于是放在此处
	slot_SaveTransformParm();
	update();
}

extern double WINDOWSSCALEFACTOR;
void MapUnit::mouseDoubleClickEvent(QMouseEvent* event)
{
	event->accept();
	QPointF pos = event->localPos();

	pos.setX(WINDOWSSCALEFACTOR * pos.x());
	pos.setY(WINDOWSSCALEFACTOR * pos.y());
	emit SingleClick(pos);
}


void MapUnit::dealMultiTouch(QPointF p1, QPointF p2, QPointF p3, QPointF p4) {

if (p1 == p2 || p3 == p4) {



} else {

double delta1 = QLineF(p1, p2).length();

double delta2 = QLineF(p3, p4).length();

double delta = delta1;

if (delta1 < delta2) {

delta = delta1;

} else {

delta = delta2;

}

double x1 = p2.x() - p1.x();

double y1 = p2.y() - p1.y();

double x2 = p4.x() - p3.x();

double y2 = p4.y() - p3.y();

double aaa = x1 * x2 + y1 * y2;

double bbb = sqrt((pow(x1, 2) + pow(y1, 2)) * (pow(x2, 2) + pow(y2, 2)));

double cosA = aaa / bbb;

if (cosA < 0) {

//通过向量算出两者之间的夹角的余弦值，因为范围是（0，180，所以大于90度，余弦值必定小于0

double endL = QLineF(p2, p4).length();

double startL = QLineF(p1, p3).length();

QPointF pos = QPointF((p1.x()+p2.x())/2,(p1.y()+p2.y())/2);

//然后来判断是放大还是缩小操作算出两指起始点以及终止点之间的距离，如果起始点间的距离大于

//终止点间的距离则是缩小，反正放大

if (endL > startL)

{

//放大操作
    //鼠标点从视图点转逻辑点位
    QPointF invert_pos = mouseTransfromPoint(pos);
    scale_transfrom.scale(1.05, 1.05);
    QPointF scale_1_2_pos = pointTransfrom(invert_pos);
    mouse_move_transform.translate(-(scale_1_2_pos.x() - pos.x()), -(scale_1_2_pos.y() - pos.y()));

}


else {
//缩小操作
        QPointF invert_pos = mouseTransfromPoint(pos);
        scale_transfrom.scale(pow(1.05, -1), pow(1.05, -1));
        QPointF scale_n_1_2_pos = pointTransfrom(invert_pos);
        mouse_move_transform.translate(pos.x() - scale_n_1_2_pos.x(), pos.y() - scale_n_1_2_pos.y());

}

update();//此处更新控件使之放大缩小

}
}
}


bool MapUnit::touchBeginEventProcess(QEvent *event)
{
    QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
    QList<QTouchEvent::TouchPoint> touchStartPoints = touchEvent->touchPoints();
    mouse_press = touchStartPoints.at(0).screenPos().toPoint();  //  touch开始触控的点坐标

    touchEvent->accept();
    return true;
}

bool MapUnit::touchUpdateEventProcess(QEvent *event)
{
    QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
    QList<QTouchEvent::TouchPoint> touchUpdatePoints = touchEvent->touchPoints();

    //  设置单指触控，还是多指触控
    if (1 == touchUpdatePoints.count()) {

    //  假设此时场景，手指滑动了
        QPoint nowPoint = touchUpdatePoints.at(0).screenPos().toPoint();  // screenPos 当前坐标的位置
        QPoint lastPoint = touchUpdatePoints.at(0).lastScreenPos().toPoint();  //  上一次坐标的位置
        QPoint pos =nowPoint;
        mouse_move_transform.translate(pos.x() - mouse_press.x(), pos.y() - mouse_press.y());
        mouse_press = nowPoint;
        update();
        //endPoint = nowPoint;  //  手指滑动后，记录一下最后松开的 坐标位置
    }
    return true;
}

bool MapUnit::touchEndEventProcess(QEvent *event)
{
    QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);

    //  这里或者 touchUpdateEventProcess 当中去处理，根据初始和结束坐标，做其它处理

    /* 比如上述touch空白处退出窗口的需求，根据前后坐标，满足设定的阈值(10 - 20 像素就好)，就释放退出信号*/
    touchEvent->accept();
    return true;
}


void MapUnit::touchEvent(QTouchEvent *event)
{
    event->accept();
    switch (event->type())
            {
                case QEvent::TouchBegin:
                     touchBeginEventProcess(event);
        break;
                case QEvent::TouchUpdate:
                     touchUpdateEventProcess(event);
        break;
                case QEvent::TouchEnd:
                     touchEndEventProcess(event);
        break;
   }
    QList<QTouchEvent::TouchPoint> touchPoints = event->touchPoints();

    if (touchPoints.count() == 1) {

    //判断单点触控
        QPointF p1 = touchPoints.at(0).startPos();

        QPointF p2 = touchPoints.at(0).lastPos();


    }

    if (touchPoints.count() == 2) {

    //判断两点也就是双指缩放动作
        QPointF p1 = touchPoints.at(0).startPos();

        QPointF p2 = touchPoints.at(0).lastPos();

        QPointF p3 = touchPoints.at(1).startPos();

        QPointF p4 = touchPoints.at(1).lastPos();
        dealMultiTouch(p1,p2,p3,p4);

        }


}





//滚轮事件
void MapUnit::wheelEvent(QWheelEvent* event)
{
	event->accept();
	QPointF pos = event->position();
	QPoint sroll = event->angleDelta();
	//放大
	if (sroll.y() > 0)
	{
		//鼠标点从视图点转逻辑点位
		QPointF invert_pos = mouseTransfromPoint(pos);
		scale_transfrom.scale(1.2, 1.2);
		QPointF scale_1_2_pos = pointTransfrom(invert_pos);
		mouse_move_transform.translate(-(scale_1_2_pos.x() - pos.x()), -(scale_1_2_pos.y() - pos.y()));
	}
	else//缩小
	{

		QPointF invert_pos = mouseTransfromPoint(pos);
		scale_transfrom.scale(pow(1.2, -1), pow(1.2, -1));
		QPointF scale_n_1_2_pos = pointTransfrom(invert_pos);
		mouse_move_transform.translate(pos.x() - scale_n_1_2_pos.x(), pos.y() - scale_n_1_2_pos.y());
	}
	update();

}

void MapUnit::positionChange()
{
	update();
}

//单击站点 执行导航
void MapUnit::clickToMove(QPointF pos)
{
	SeerrMap& map = SeerrMap::GetInstance();
	if (!map.isMapReady())
		return;

	foreach(AdvancedPoint advpoint, map.AdvancedPointList)
	{
		QPointF point(advpoint.pos.x, advpoint.pos.y);


		point = pointTransfrom(point);

		qreal width = 1 * scale_transfrom.m11();
		qreal height = 1 * scale_transfrom.m11();
		qreal x = point.x() - width / 2;
		qreal y = point.y() - height / 2;
		QRect rec(x, y, width, height);

		if (rec.contains(pos.toPoint()))
		{
			qDebug() << advpoint.instanceName;
			MissionManager::instance().publishMoveTask(advpoint.instanceName);
			
		}

	}
}
