#include "Music.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>

Music &Music::GetInstance()
{
    static Music instance;
    return instance;
}

Music::Music(QObject *parent)
    : QObject(parent)
{
    player.setAudioOutput(&audioOutput);
    connect(&player, &QMediaPlayer::errorOccurred, this,
            [this](QMediaPlayer::Error, const QString &message) {
        qWarning() << "Could not play" << player.source() << message;
    });
}

void Music::play(const QString &filename, int loops)
{
    const QString localPath = QCoreApplication::applicationDirPath() + "/music/" + filename;
    const QUrl source = QFileInfo(localPath).isFile()
        ? QUrl::fromLocalFile(localPath)
        : QUrl("qrc:/music/" + filename);

    player.stop();
    player.setSource(source);
    player.setLoops(loops);
    player.play();
}

void Music::stopmusic()
{
    player.setSource(QUrl());
}

void Music::errorPlay()
{
    play(QStringLiteral("error.wav"), 3);
}

void Music::rotatePlay()
{
    play(QStringLiteral("rotate.wav"), 3);
}

void Music::lowBatteryPlay()
{
    play(QStringLiteral("lowBattery.wav"), 3);
}

void Music::blockPlay()
{
    play(QStringLiteral("block.wav"), QMediaPlayer::Infinite);
}

void Music::taskfinishedPlay()
{
    play(QStringLiteral("taskfinished.wav"), 3);
}

void Music::asternPlay()
{
    play(QStringLiteral("astern.wav"), 3);
}

void Music::reachedPlay()
{
    play(QStringLiteral("reached.wav"), QMediaPlayer::Once);
}

void Music::turnrightPlay()
{
    play(QStringLiteral("turnright.wav"), 3);
}

void Music::navigationPlay()
{
    play(QStringLiteral("navigation.wav"), QMediaPlayer::Infinite);
}

void Music::collisionPlay()
{
    play(QStringLiteral("collision.wav"), 3);
}

void Music::turnleftPlay()
{
    play(QStringLiteral("turnleft.wav"), 3);
}
