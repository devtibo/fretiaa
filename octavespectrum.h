#ifndef OCTAVESPECTRUM_H
#define OCTAVESPECTRUM_H

#include <QMainWindow>
#include <QWidget>
#include "datasharer.h"
#include "qcustomplot/qcustomplot.h"
#include "onenoctavefilters.h"

class OctaveSpectrum : public QMainWindow
{
    Q_OBJECT
public:
    explicit OctaveSpectrum(DataSharer*,QWidget *parent=0);
    DataSharer *m_data;
    QCustomPlot *cPlot;
    QCPBars *newBars ;
    OneNOctaveFilters *m_filter;
    void setData(QVector<double> ,int  );
    void closeEvent( QCloseEvent* );
    QVector<double> valuefilt_vec;
    QVector<double> valuefilt_vec3;
    QVector<double> valuefilt_vec2;
    QVector<double> rmsValuesVec;
    QVector<double> xData;
    QVector<QString> labels;
signals:
    void octaveIsClosing();
    void updateTracer();
public slots:
    void onExportData();
};

#endif // OCTAVESPECTRUM_H
