#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <QMainWindow>
#include <QWidget>

#include "datasharer.h"
#include "ffft/FFTReal.h"
#include "qcustomplot/qcustomplot.h"

void unwrap(QVector<double> &p, int N);

class SpectrumWin : public QMainWindow
{
    Q_OBJECT
public:
    explicit SpectrumWin(QWidget *parent=0);
    void closeEvent(QCloseEvent*);
signals:
    void spectrumIsClosing();
};


class Spectrum : public QThread
{
    Q_OBJECT
public:
    explicit Spectrum( DataSharer*,QWidget *parent=0);
    QWidget *mParent;
    SpectrumWin *mWin;
    DataSharer *m_data;
    void closeEvent( QCloseEvent* );
    ffft::FFTReal <float> *fft_object;
    QCustomPlot *cPlot, *cPlotAngle;
    QVector<double> win;


    QVector<double> freqValues;
    QVector<double> fftValues;
    QVector<double> angleValues;



public slots:
    void onExportData();
    void updateData();
};

#endif // SPECTRUM_H
