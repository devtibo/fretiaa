#ifndef DATASHARER_H
#define DATASHARER_H

#include "qcustomplot/qcustomplot.h"
#include "audioengine.h"
#include <QReadWriteLock>
#include <QString>
#include <QQueue>
#include "spectrogram.h"
class Spectrogram; // This is necessary because Class Spectrogram include Class DataSharer and vice et versa!

class DataSharer
{
public:
    DataSharer(QWidget* parent = Q_NULLPTR);
    int rectAnalysisLength;
    float rectAnalysisDuration ;
    float fs;

    //Analysis window
    float t_begin = 0;
    int idx_begin = 0;
    float idx_begin_ratio = 0.5;


    float observationTime;
    int data_length;
    int length_fft_spectrogram;
    int numOctaveFilters=23;
    bool isLiveView = true;
    bool isRectMove = false;
    bool isTrigger = false;
    bool isSpectrogramShow=true;
    bool interpretAsVolt = false;



    float g1_Value=0;   QString g1_Unit = "dB"; float g1_lin=1;
    float g2_Value=0;  QString g2_Unit= "dB" ; float g2_lin=1;
    float gSensi_Value=1; QString gSensi_Unit= "V/Pa";float gSensi_lin=1;



    float maxValueInV,maxValueInPa;



    Spectrogram *qPlotSpectrogram;
    QCustomPlot *qPlotOscillogram;
    AudioEngine *m_AudioEngine;



    QVector<double> ReadPoints_x;
    QVector<double> ReadPoints_y;
    QVector<double> rectPointsy;

    QVector<double> frame1;
    QVector<double> frame2;

    QQueue<QVector<double>> framesQueue;
    QReadWriteLock lock, lock2, lock3, lock4;

    void writeRectData(QVector<double> p) {
        QWriteLocker wlocker(&lock);
        rectPointsy = p;
    }

    QVector<double> ReadRectData(){
        QReadLocker rlocker(&lock);
        return rectPointsy;
    }

    void ClearRectData(){
        QReadLocker rlocker(&lock);
        rectPointsy.clear();
    }

    void wrireFrame1Data(QVector<double> p) {
        QWriteLocker wlocker(&lock3);
        frame1.append(p);
    }

    QVector<double> ReadFrame1Data(){
        QReadLocker rlocker(&lock3);
        return frame1;
    }

    void wrireFrame2Data(QVector<double> p) {
        QWriteLocker wlocker(&lock4);
        frame2.append(p);
    }

    QVector<double> ReadFrame2Data(){
        QReadLocker rlocker(&lock4);
        return frame2;
    }

    void writeData(QVector<double> p,QVector<double> q) {
        QWriteLocker wlocker(&lock2);
        ReadPoints_x.append(p);
        ReadPoints_y.append(q);
    }

    void clearData() {
        QWriteLocker wlocker(&lock2);
        ReadPoints_x.clear();
        ReadPoints_y.clear();
    }

    void ReadData(QVector<double> &p,QVector<double> &q) {
        QReadLocker rlocker(&lock2);
        p=ReadPoints_x;
        q=ReadPoints_y;
        ReadPoints_x.clear();
        ReadPoints_y.clear();
    }

};

#endif // DATASHARER_H
