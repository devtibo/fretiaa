#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <QMainWindow>
#include <QWidget>

#include "datasharer.h"
#include "ffft/FFTReal.h"
#include "qcustomplot/qcustomplot.h"

void unwrap(QVector<double> &p, int N);

class Spectrum : public QMainWindow
{
    Q_OBJECT
public:
    explicit Spectrum(DataSharer*);
    DataSharer *m_data;
    void setData(QVector<double>,int );
    void closeEvent( QCloseEvent* );
    ffft::FFTReal <float> *fft_object;
    QCustomPlot *cPlot, *cPlotAngle;
    QVector<float> hann;


    QVector<double> freqValues;
    QVector<double> fftValues;
    QVector<double> angleValues;


signals:
    void spectrumIsClosing();
    void updateTracer();
public slots:
    void onExportData();
};

#endif // SPECTRUM_H
