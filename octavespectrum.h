#ifndef OCTAVESPECTRUM_H
#define OCTAVESPECTRUM_H

#include <QMainWindow>
#include <QWidget>
#include "datasharer.h"
#include "qcustomplot/qcustomplot.h"
#include "onenoctavefilters.h"

class OctaveSpectrum : public QThread
{
    Q_OBJECT
public:
    explicit OctaveSpectrum(DataSharer*,QWidget *parent=0);
    DataSharer *m_data;
    QCustomPlot *cPlot;
    QCPBars *newBars ;
    OneNOctaveFilters *m_filter;
    void setData(QVector<double>  );
    void closeEvent( QCloseEvent* );
    QVector<double> valuefilt_vec;
    QVector<double> valuefilt_vec3;
    QVector<double> valuefilt_vec2;
    QVector<double> rmsValuesVec;
    QVector<double> xData, mSamples;
    QVector<QString> labels;

    QWidget *mParent;
    QMainWindow *mWin;


    void runWithParams(QVector<double>  );
signals:
    void octaveIsClosing();

public slots:
    void onExportData();
    void run();
};

#endif // OCTAVESPECTRUM_H
