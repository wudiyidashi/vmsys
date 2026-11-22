#ifndef MUSIC_H
#define MUSIC_H

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QObject>

class Music : public QObject
{
    Q_OBJECT
public:
    explicit Music(QObject *parent = nullptr);
    static Music &GetInstance();

    void stopmusic();
    void lowBatteryPlay();
    void blockPlay();
    void errorPlay();
    void rotatePlay();
    void taskfinishedPlay();
    void asternPlay();
    void reachedPlay();
    void turnrightPlay();
    void navigationPlay();
    void collisionPlay();
    void turnleftPlay();

private:
    void play(const QString &filename, int loops);

    QAudioOutput audioOutput;
    QMediaPlayer player;
};

#endif // MUSIC_H
