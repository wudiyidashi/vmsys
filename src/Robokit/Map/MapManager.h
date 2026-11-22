#ifndef SEERRMAP_H
#define SEERRMAP_H

#include <QElapsedTimer>
#include <QJsonObject>
#include <QObject>
#include <QVariant>
struct MapProperty
{
    QString key;
    QString type;
    QString value; //先转string再bas64编码
    QVariant Value;
};

struct MapPos
{
    double x;
    double y;
};

struct MapHeader_S
{
    QString mapType;
    QString mapName;
    MapPos minPos;
    MapPos maxPos;
    double resolution;
};

struct AdvancedPoint
{
    QString className;
    QString instanceName;
    MapPos pos;
    double dir; // (double) 方向(rad)
    MapProperty property;
    bool ignoreDir;// (bool) 是否忽略方向

    //extern propotie
    int vertex;
};

struct BezierPath
{
    QString className;
    QString instanceName;
    AdvancedPoint startPos;
    AdvancedPoint endPos;
    MapPos controlPos1;
    MapPos controlPos2;
    MapProperty property[2];

    //extern propotie
    double lenth;

};

struct ArcPath
{
    QString className;
    AdvancedPoint startPos;
    AdvancedPoint endPos;
    MapPos controlPos1;
    MapProperty property[1];
};

struct StraightPath
{
    QString className;
    QString instanceName;
    AdvancedPoint startPos;
    AdvancedPoint endPos;
    MapProperty property[1];
};

//custum
struct AGVPosition
{
    MapPos pos;
    double angle;
};

MapPos BezierPoint(double t, MapPos p0, MapPos p1, MapPos p2, MapPos p3);
double getBezier_length(BezierPath bzpath, int pointCount);
namespace robokit { class RobotConfig; }

class SeerrMap : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString mapName READ activeMapName NOTIFY activeMapChanged)
    Q_PROPERTY(bool mapReady READ isMapReady NOTIFY mapReadyChanged)
public:
    static SeerrMap& GetInstance();

    MapHeader_S MapHeader{};

    QList<AdvancedPoint> AdvancedPointList;
    //AdvancedCurveList
    QList<BezierPath> BezierPathList;
    QList<ArcPath> ArcPathList;
    QList<StraightPath> StraightPathList;
    QList<MapPos>  normalPosList;
    QList<MapPos>  rssiPosList;

    AGVPosition curpos{};
    QString GetCurMapName() const;
    QString activeMapName() const { return requestedMap; }
    bool isMapReady() const { return mapReady; }
    bool hasPosition() const { return positionAvailable; }
    void setCurrentMap(const QString &name, const QString &revision);
    void invalidateMap();
    void clearPosition();
    void refreshMap(robokit::RobotConfig &config);
signals:
    void MapLoadFinished();
    void PositionChange();
    void activeMapChanged();
    void mapReadyChanged();
public slots:
    void UpdateSignlePosition(double x, double y, double angle);
private:
    SeerrMap();
    SeerrMap(const SeerrMap& map);
    const SeerrMap& operator=(const SeerrMap& map);

    QString requestedMap;
    QString requestedRevision;
    QElapsedTimer lastDownload;
    quint64 downloadGeneration = 0;
    bool mapReady = false;
    bool positionAvailable = false;

    void saveMapFile(const QString &name, const QJsonObject &map);
    void parseMap(const QJsonObject &map);
};
#endif // SEERRMAP_H
