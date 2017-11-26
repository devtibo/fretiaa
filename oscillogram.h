#ifndef OSCILLOGRAM_H
#define OSCILLOGRAM_H

#include <QWidget>


#include "qcustomplot/qcustomplot.h"
#include "datasharer.h"
#include "qfgraph.h"

class Oscillogram : public QWidget
{
    Q_OBJECT
public:

    QWidget *m_parent;
    //explicit Oscillogram(QWidget *parent = 0);
    Oscillogram( DataSharer*,QWidget *parent=0);
    ///![] Oscillogram
    QCustomPlot *cPlot;
    //float observationTime;
    void updateData(QVector<double>, QVector<double>);

    void setYlabel(QString);

    QCPItemRect *rect;
    bool stillMove = false;

    float windowsTime;

    DataSharer *m_Data = new DataSharer;

    void updateRect();

    //QPoint contextPos;
    //void resizeAxisDialog(QCPAxis *, QPoint);


    QVector<double> xData;
    QVector<double> yData;

    double triggerLevel = 0.5;
    //! /*Trigger*/
    QCPItemLine *triggerLine;



    QFGraph *m_qcfgraph;

protected:

signals:

    void update(void);
    void updateTracer();
public slots:
    void onMouseMove(QMouseEvent*);
    void onExportData();
    void updateXSpectrogramAxes(QCPRange);
};

#endif // OSCILLOGRAM_H
