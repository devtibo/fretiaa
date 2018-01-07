#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include <QThread>
#include "qcustomplot/qcustomplot.h"
#include "datasharer.h"
#include "qfgraph.h"
#include "ffft/FFTReal.h"

#include <QFuture>

void mreplot(QCustomPlot*);

class DataSharer; // This is necessary because Class Spectrogram include Class DataSharer and vice et versa!
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
    void update();
    int nx, ny;
    QCPMarginGroup *marginGroup = new QCPMarginGroup(cPlot);

    QWidget *mParent;
    QCPItemRect *rect;
    QFGraph *m_qcfgraph;

    QFuture<void> f1;
    QVector<double> mSamples;

    void runWithParams(QVector<double>);

    bool lastFrame = 1;
bool shiftmode=0;

    int idx;

    double z;
    float *input_fft;
    float *output_fft;
    //QVector<double> currentFrame;

public slots:
    void updateXSpectrogramAxes(QCPRange);
    void onExportData();
    void run();


signals:
};

#endif // SPECTROGRAM_H
