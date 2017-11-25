#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <QWidget>
#include "qcustomplot/qcustomplot.h"
#include "datasharer.h"
#include "qfgraph.h"
#include "ffft/FFTReal.h"

class Spectrogram : public QWidget
{
    Q_OBJECT
public:
    explicit Spectrogram(DataSharer*,QWidget *parent=0);
    DataSharer *m_data;
    QCustomPlot *cPlot = new QCustomPlot(this);
    QCPColorMap *colorMap = new QCPColorMap(cPlot->xAxis, cPlot->yAxis);
    ffft::FFTReal <float> *fft_object;
    QVector<double> win;
    void update(QVector<double>);
    int nx, ny;
    QCPMarginGroup *marginGroup = new QCPMarginGroup(cPlot);


    QCPItemRect *rect;
    QFGraph *m_qcfgraph;


public slots:
    void updateXSpectrogramAxes(QCPRange);
    void onExportData();

signals:
    void updateTracer();
};

#endif // SPECTROGRAM_H
