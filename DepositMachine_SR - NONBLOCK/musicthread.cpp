#include "musicthread.h"
#include "global.h"

MusicThread::MusicThread(QObject *parent) :
    QThread(parent)
{
    runFlag = 1;
    curMedia = p_Main->setMedia;
    thisPar = parent;
    proct = new QProcess();
    connect(proct, SIGNAL(finished(int)),proct,SLOT(deleteLater()));
    proct->start(QString("killall \"omxplayer.bin\""));
    proct->waitForFinished();
    this->start();
}

MusicThread::~MusicThread()
{
    exit(0);
}

void MusicThread::run()
{
    qDebug() << "started another media" << p_Main->setMedia;
//    _Player->setMedia(curMedia);
//    _Player->play();
//    qDebug() << "stop process media" << p_Main->setMedia << proc->atEnd();

//    proc->start("kill $(pgrep omxplayer)");

    if(p_Main->setMedia.toLocalFile() != ""){
        proc = new QProcess();
        connect(proc, SIGNAL(finished(int)),proc,SLOT(deleteLater()));
        proc->start(QString("omxplayer --no-keys %1 &").arg(p_Main->setMedia.toLocalFile()));
        proc->waitForFinished();
    }

//    proc->start(QString("omxplayer %1").arg(p_Main->setMedia.toLocalFile()));
//    msleep(500);
//    qDebug() << "played media" << p_Main->setMedia;

//    while(runFlag){
//        msleep(1000);
//        qDebug() << proc->state();
//        if(proc->state() == QProcess::NotRunning){
//            qDebug() << "QProcess NotRunning";
//            break;
//        }

//    }


//    delete _Player;

    qDebug() << "Music thread killed";
    exit(0);
}

void MusicThread::stop()
{
    qDebug() << "Music thread killed";
    exit(0);
}

void MusicThread::ChangedStatus(QMediaPlayer::MediaStatus status)
{
    if(status == QMediaPlayer::EndOfMedia){
        runFlag = 2;
    }
}
