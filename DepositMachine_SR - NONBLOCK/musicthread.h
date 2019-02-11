#ifndef MUSICTHREAD_H
#define MUSICTHREAD_H

#include <QThread>
#include <QMediaPlayer>
#include <QProcess>
#include <QObject>

class MusicThread : public QThread
{
    Q_OBJECT
public:
    explicit MusicThread(QObject *parent = 0);
    ~MusicThread();
    QMediaPlayer* _Player;
    QProcess *proc;
    QProcess *proct;
    QObject* thisPar;
    QUrl curMedia;
    int runFlag;

    void run();
    void stop();
private slots:
    void ChangedStatus(QMediaPlayer::MediaStatus status);
};

#endif // MUSICTHREAD_H
