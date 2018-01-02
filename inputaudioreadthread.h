#ifndef INPUTAUDIOREADTHREAD_H
#define INPUTAUDIOREADTHREAD_H

#include <QThread>
#include "datasharer.h"

class InputAudioReadThread : public QThread
{
    Q_OBJECT

public:
    InputAudioReadThread(DataSharer*, QObject * parent = 0);
    //  ~InputAudioReadThread();
    void run();

    DataSharer *m_data;

    QVector<double> points_x;
    QVector<double> points_y;
    uchar *dataSound;

    double nbreOfSamples =0;
};

#endif // INPUTAUDIOREADTHREAD_H
