#ifndef QFGRAPH_H
#define QFGRAPH_H

#include <QWidget>
#include "qcustomplot/qcustomplot.h"
#include "xyrangedialog.h"
#include "qcpitemtracerbars.h"
#include "qcpitemtracercolormap.h"


class QFGraph : public QWidget
{
    Q_OBJECT

public:
    explicit QFGraph(QCustomPlot*, QCPBars*, QWidget *parent = 0);
    explicit QFGraph(QCustomPlot*, QCPGraph*, QWidget *parent = 0);
    explicit QFGraph(QCustomPlot*, QCPColorMap*, QWidget *parent = 0);

    void setDefaultXYRange(QCPRange, QCPRange);

    QCustomPlot *cPlot;// = new QCustomPlot;
    QCPGraph *mGraph;
    QCPBars *mBars;
    QCPColorMap *mColorMap;

    QCPRange defaultX = QCPRange(0,1);
    QCPRange defaultY = QCPRange(-1,1);

    void setToolBarVisible(bool);
    bool ToolBarVisible();
private :
    void resizeAxisDialog(QCPAxis*, QPoint );
    QPoint contextPos;

    QCPItemTracer *phaseTracer;
    QCPItemTracerBars *phaseTracerBars;
    QCPItemTracerColorMap *phaseTracerColorMap;
    QCPItemText *phaseTracerText ;
    QShortcut *sCArrowsL ;
    QShortcut *sCArrowsR;
    QShortcut *sCArrowsUP;
    QShortcut *sCArrowsDOWN;
    void SetTracerAt(double);
    void SetTracerAt(double x, double y);
    bool isToolBarVisible=true;

    QGridLayout *layout;
    QToolBar *m_toolBar;
    QSharedPointer <QCPGraphDataContainer> graphData ;
    QSharedPointer <QCPBarsDataContainer> graphDataBars ;
    QCPColorMapData *graphDataColorMap ;

    void init(QCustomPlot*);

    int graphTypes;
private :
    QVector<double> xValues;
    QVector<double> yValues;
    int isXYValueMode = false;
signals:
    void exportData();
public slots:

private slots:
    void onCustomContextMenuRequested(QPoint);
    void onXAxisResize();
    void onYAxisResize();
    void onExportData();
    void onResetAxis();
    void onAxisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*);

    void onInteractionRectZoomAction(bool);
    void onInteractionDragAxisAction(bool);
    void onInteractionMouseWheelZoomAction(bool);
    void onInteractionDefaultAxis(bool);
    void onInteractionXYPoints(bool);
    void onInteractionAutoZoom(bool);

    void onMousePress(QMouseEvent*);
    void onKeyLPress();
    void onKeyRPress();
    void onKeyUPPress();
    void onKeyDOWNPress();
    void updateTracerText();

};

#endif // QFGRAPH_H
