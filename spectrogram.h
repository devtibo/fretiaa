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
    QCustomPlot* graphViewer();
    ffft::FFTReal <float> *fft_object;
    QVector<double> hann;
    void update(QVector<double>);
    int nx, ny;
    QCPMarginGroup *marginGroup = new QCPMarginGroup(cPlot);
    //int begin_ctr=0;

    // QPoint contextPos;
    // void resizeAxisDialog(QCPAxis *, QPoint);

    QCPItemRect *rect;
    QFGraph *m_qcfgraph;
signals:
    void updateTracer();
public slots:
    void updateXSpectrogramAxes(QCPRange);
    //  void onCustomContextMenuRequested(QPoint);
    //     void onAxisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*);
    //   void onXAxisResize();
    //  void onYAxisResize();
    void onExportData();
    // void onResetAxis();
};

#endif // SPECTROGRAM_H
