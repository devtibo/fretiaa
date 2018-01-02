#ifndef DATASHARER_H
#define DATASHARER_H

#include "qcustomplot/qcustomplot.h"
#include "audioengine.h"
#include <QReadWriteLock>


class DataSharer
{
public:
    DataSharer(QWidget* parent = Q_NULLPTR);
    int rectAnalysisLength;
    float rectAnalysisDuration ;
    float fs;

    //Analysis window
    float t_begin;
    int idx_begin;
    float idx_begin_ratio = 0.5;


    float observationTime;
    int data_length;
    int length_fft_spectrogram;
    int numOctaveFilters=23;
    bool isLiveView = true;
    bool isRectMove = false;
    bool isTrigger = false;
    float g2_lin;
    float g1_lin;
    float sensitivity;
    bool isSpectrogramShow=false;

    QCustomPlot *qPlotSpectrogram;
    QCustomPlot *qPlotOscillogram;
    AudioEngine *m_AudioEngine;

    QVector<double> ReadPoints_x;
    QVector<double> ReadPoints_y;
    QVector<double> rectPointsy;

    QReadWriteLock lock;

    void wrireRectData(QVector<double> p) {
        QWriteLocker wlocker(&lock);
        rectPointsy = p;
    };
    QVector<double> ReadRectData(){
        QReadLocker rlocker(&lock);
        return rectPointsy;
    };

};

#endif // DATASHARER_H
