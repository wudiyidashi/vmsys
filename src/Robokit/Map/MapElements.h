#ifndef MAPELEMENTS_H
#define MAPELEMENTS_H

#include <QString>
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
struct CurrentPosition
{
    MapPos pos;
    double angle;
};
#endif // MAPELEMENTS_H
