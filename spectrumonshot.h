#ifndef SPECTRUMONSHOT_H
#define SPECTRUMONSHOT_H

#include <QMainWindow>
#include <QWidget>

#include "datasharer.h"
#include "ffft/FFTReal.h"
#include "qcustomplot/qcustomplot.h"



class SpectrumWinOnShot : public QMainWindow
{
    Q_OBJECT
public:
    explicit SpectrumWinOnShot(QWidget *parent=0);
    void closeEvent(QCloseEvent*);
signals:
    void spectrumIsClosing();
};


class SpectrumOnShot : public QThread
{
    Q_OBJECT
public:
    explicit SpectrumOnShot( DataSharer*,QWidget *parent=0);
    QWidget *mParent;
    SpectrumWinOnShot *mWin;
    DataSharer *m_data;
    void closeEvent( QCloseEvent* );
    ffft::FFTReal <float> *fft_object;
    QCustomPlot *cPlot, *cPlotAngle;
    QVector<double> win;


    QVector<double> freqValues;
    QVector<double> fftValues;
    QVector<double> angleValues;

void unwrap(QVector<double> &p, int N);

public slots:
    void onExportData();
    void updateData();
};

#endif // SPECTRUMONSHOT_H
