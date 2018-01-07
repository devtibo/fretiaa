#ifndef INPUTAUDIOREADTHREAD_H
#define INPUTAUDIOREADTHREAD_H

#include <QThread>
#include "datasharer.h"

#include <QFile>
#include <QString>

class InputAudioReadThread : public QThread
{
    Q_OBJECT

public:
    InputAudioReadThread(DataSharer*, QObject * parent = 0);
    //  ~InputAudioReadThread();
    void run();
    void processFrame1_inFLy();
    void processFrame2_inFLy();

    DataSharer *m_data;

    QVector<double> points_x;
    QVector<double> points_y;

    uchar *dataSound;
    qint64 nbreOfSamples; // chnage to int64

    // Framming
    QVector<double> win;
    int framesCpt1=0, framesCpt2=0;
    QVector<double> frame1;
    QVector<double> frame2;
    QVector<double> frame1Leaving;
    QVector<double> frame2Leaving;


    bool wichFrame;

signals :
    void spectrogramFrameReady();

   // QString fname;
  //  QFile fid;

};

#endif // INPUTAUDIOREADTHREAD_H
