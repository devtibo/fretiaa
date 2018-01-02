#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <QThread>
#include "qcustomplot/qcustomplot.h"
#include "datasharer.h"
#include "qfgraph.h"
#include "ffft/FFTReal.h"

#include <QFuture>

void mreplot(QCustomPlot*);

class Spectrogram : public QThread
{
    Q_OBJECT
public:
    explicit Spectrogram(DataSharer*,QWidget *parent=0);
    DataSharer *m_data;
    QCustomPlot *cPlot = new QCustomPlot();
    QCPColorMap *colorMap = new QCPColorMap(cPlot->xAxis, cPlot->yAxis);
    ffft::FFTReal <float> *fft_object;
    QVector<double> win;
    void update(QVector<double>);
    int nx, ny;
    QCPMarginGroup *marginGroup = new QCPMarginGroup(cPlot);

    QWidget *mParent;
    QCPItemRect *rect;
    QFGraph *m_qcfgraph;

    QFuture<void> f1;
    QVector<double> mSamples;

    void runWithParams(QVector<double>);
public slots:
    void updateXSpectrogramAxes(QCPRange);
    void onExportData();
    void run();


signals:
};

#endif // SPECTROGRAM_H
