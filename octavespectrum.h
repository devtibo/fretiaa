#ifndef OCTAVESPECTRUM_H
#define OCTAVESPECTRUM_H

#include <QMainWindow>
#include <QWidget>
#include "datasharer.h"
#include "qcustomplot/qcustomplot.h"
#include "onenoctavefilters.h"

class OctaveSpectrumWin : public QMainWindow
{
    Q_OBJECT
public:
    explicit OctaveSpectrumWin(QWidget *parent=0);
    void closeEvent(QCloseEvent*);
signals:
    void octaveIsClosing();
};


class OctaveSpectrum : public QThread
{
    Q_OBJECT
public:
    explicit OctaveSpectrum(DataSharer*,QWidget *parent=0);
    DataSharer *m_data;
    QCustomPlot *cPlot;
    QCPBars *newBars ;
    OneNOctaveFilters *m_filter;

/*    QVector<double> valuefilt_vec;
    QVector<double> valuefilt_vec3;
    QVector<double> valuefilt_vec2;*/
    QVector<double> rmsValuesVec;
    QVector<double> xData, mSamples;
    QVector<QString> labels;

    QWidget *mParent;
    OctaveSpectrumWin *mWin;


    float computeOneBand(int &i, QVector<double> &samples);
   // void runWithParams(QVector<double>  );


public slots:
    void onExportData();
    void run();
};



#endif // OCTAVESPECTRUM_H
