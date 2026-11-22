#include "MapManager.h"

#include <Robokit/RobokitApi/RobotConfig.h>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSaveFile>

namespace {
MapPos readPosition(const QJsonObject &value)
{
    return {value.value("x").toDouble(), value.value("y").toDouble()};
}

MapProperty readProperty(const QJsonArray &properties, qsizetype index)
{
    const auto object = index < properties.size() ? properties.at(index).toObject() : QJsonObject();
    const auto type = object.value("type").toString();
    return {object.value("key").toString(), type, object.value("value").toString(),
            object.value(type + "Value").toVariant()};
}

AdvancedPoint readPoint(const QJsonObject &point)
{
    AdvancedPoint result{};
    result.className = point.value("className").toString();
    result.instanceName = point.value("instanceName").toString();
    result.pos = readPosition(point.value("pos").toObject());
    result.dir = point.value("dir").toDouble();
    result.property = readProperty(point.value("property").toArray(), 0);
    return result;
}
}

SeerrMap::SeerrMap() = default;

SeerrMap &SeerrMap::GetInstance()
{
    static SeerrMap instance;
    return instance;
}

QString SeerrMap::GetCurMapName() const
{
    return MapHeader.mapName;
}

void SeerrMap::setCurrentMap(const QString &name, const QString &revision)
{
    const bool nameChanged = requestedMap != name;
    if (!nameChanged && (revision.isEmpty() || requestedRevision == revision))
        return;

    requestedMap = name;
    requestedRevision = revision;
    invalidateMap();
    if (nameChanged) {
        clearPosition();
        emit activeMapChanged();
    }
}

void SeerrMap::invalidateMap()
{
    ++downloadGeneration;
    lastDownload.invalidate();
    if (mapReady) {
        mapReady = false;
        emit mapReadyChanged();
    }
}

void SeerrMap::refreshMap(robokit::RobotConfig &config)
{
    constexpr int downloadTimeoutMs = 15000;
    if (mapReady || requestedMap.isEmpty() || !config.checkConnection()
        || (lastDownload.isValid() && !lastDownload.hasExpired(downloadTimeoutMs))) {
        return;
    }

    lastDownload.start();
    const auto generation = downloadGeneration;
    const auto name = requestedMap;
    config.robot_config_downloadmap_req_get({{QStringLiteral("map_name"), name}},
        [this, generation, name](uint16_t, QByteArray &response) {
            if (generation != downloadGeneration || mapReady)
                return;

            const auto map = QJsonDocument::fromJson(response).object();
            const auto error = map.value("ret_code");
            const auto header = map.value("header").toObject();
            if ((!error.isUndefined() && error.toInt(-1) != 0)
                || header.value("mapName").toString() != name) {
                qWarning() << "Could not load map" << name << map.value("err_msg").toString();
                return;
            }

            parseMap(map);
            mapReady = true;
            emit MapLoadFinished();
            emit mapReadyChanged();
            saveMapFile(name, map);
        });
}

void SeerrMap::saveMapFile(const QString &name, const QJsonObject &map)
{
    if (name.contains('/') || name.contains('\\') || name.contains(':'))
        return;

    const QDir appDir(QCoreApplication::applicationDirPath());
    if (!appDir.mkpath("maps"))
        return;

    QSaveFile file(appDir.filePath("maps/" + name + ".smap"));
    const auto data = QJsonDocument(map).toJson();
    if (!file.open(QIODevice::WriteOnly) || file.write(data) != data.size() || !file.commit())
        qWarning() << "Could not save map" << file.fileName() << file.errorString();
}

void SeerrMap::parseMap(const QJsonObject &map)
{
    const auto header = map.value("header").toObject();
    MapHeader = {header.value("mapType").toString(), header.value("mapName").toString(),
                 readPosition(header.value("minPos").toObject()),
                 readPosition(header.value("maxPos").toObject()),
                 header.value("resolution").toDouble()};

    AdvancedPointList.clear();
    BezierPathList.clear();
    ArcPathList.clear();
    StraightPathList.clear();
    normalPosList.clear();
    rssiPosList.clear();

    const auto points = map.value("advancedPointList").toArray();
    AdvancedPointList.reserve(points.size());
    for (const auto &point : points)
        AdvancedPointList.append(readPoint(point.toObject()));

    const auto curves = map.value("advancedCurveList").toArray();
    for (const auto &value : curves) {
        const auto curve = value.toObject();
        const auto type = curve.value("className").toString();
        const auto name = curve.value("instanceName").toString();
        const auto start = readPoint(curve.value("startPos").toObject());
        const auto end = readPoint(curve.value("endPos").toObject());
        const auto control1 = readPosition(curve.value("controlPos1").toObject());
        const auto properties = curve.value("property").toArray();

        if (type == "BezierPath") {
            const auto control2 = readPosition(curve.value("controlPos2").toObject());
            BezierPathList.append({type, name, start, end, control1, control2,
                                   {readProperty(properties, 0), readProperty(properties, 1)}, 0});
        } else if (type == "ArcPath") {
            ArcPathList.append({type, start, end, control1, {readProperty(properties, 0)}});
        } else if (type == "StraightPath") {
            StraightPathList.append({type, name, start, end, {readProperty(properties, 0)}});
        }
    }

    const auto obstacles = map.value("normalPosList").toArray();
    normalPosList.reserve(obstacles.size());
    for (const auto &point : obstacles)
        normalPosList.append(readPosition(point.toObject()));
}

void SeerrMap::UpdateSignlePosition(double x, double y, double angle)
{
    if (positionAvailable && curpos.pos.x == x && curpos.pos.y == y && curpos.angle == angle)
        return;

    curpos = {{x, y}, angle};
    positionAvailable = true;
    emit PositionChange();
}

void SeerrMap::clearPosition()
{
    if (!positionAvailable)
        return;

    positionAvailable = false;
    emit PositionChange();
}
