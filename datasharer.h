#ifndef DATASHARER_H
#define DATASHARER_H

#include "qcustomplot/qcustomplot.h"

class DataSharer
{
public:
    DataSharer(QWidget* parent = Q_NULLPTR);
    int length_fft;
    float fs;
    float t_begin;
    int idx_begin;
    float observationTime;
    int data_length;
    int length_fft_spectrogram;
    int numOctaveFilters=23;
    bool isLiveView = true;
    bool isRectMove = false;
    bool isTrigger = false;
    QCustomPlot *qPlotSpectrogram;
    QCustomPlot *qPlotOscillogram;
};

#endif // DATASHARER_H
