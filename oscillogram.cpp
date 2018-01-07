#include "oscillogram.h"

#include "xyrangedialog.h"
#include <QMetaType>

Oscillogram::Oscillogram(DataSharer *data, QWidget* parent)
{
    qRegisterMetaType<QCPRange>("QCPRange");

    mparent = parent;
    m_Data = data;

    /* Create Graphic*/
    cPlot = new QCustomPlot();
#ifdef QCUSTOMPLOT_USE_OPENGL
    cPlot->setOpenGl(true);
#endif
    cPlot->xAxis->setLabel("Time (s)");
    cPlot->xAxis->setTickLabels(true);
    cPlot->yAxis->setLabel("Pressure (Pa)");
    cPlot->yAxis->setRange(-1,1);
    cPlot->xAxis->setRange(0,m_Data->observationTime);
    cPlot->setInteraction(QCP::iSelectItems,true);
    cPlot->addGraph();
    cPlot->graph()->setAdaptiveSampling(true);


    /* Selection Rectangle*/
    rect = new QCPItemRect(cPlot);
    rect->setBrush(QBrush(QColor(0,0,255,70) ));
    rect->setSelectable(false);

    rect->topLeft->setCoords(m_Data->observationTime/2-(m_Data->rectAnalysisDuration)/2 ,1);
    rect->bottomRight->setCoords(m_Data->observationTime/2 +(m_Data->rectAnalysisDuration)/2  ,-1);
    m_Data->t_begin =  m_Data->observationTime/2-(m_Data->rectAnalysisDuration)/2;
    m_Data->idx_begin = round(m_Data->t_begin * m_Data->fs);

    /* Trigger */
    triggerLine = new QCPItemLine(cPlot);
    triggerLine->start->setCoords(0,0.5);
    triggerLine->end->setCoords(data->observationTime,0.5);
    triggerLine->setPen(QPen(Qt::gray, 2.0, Qt::DashDotLine));
    triggerLevel = 0.5;
    triggerLine->setSelectable(true);
    triggerLine->setVisible(false);

    /* Add plot to QCFGRaph*/
    m_qcfgraph = new QFGraph(cPlot,cPlot->graph(0));
    m_qcfgraph->setDefaultXYRange(QCPRange(0,m_Data->observationTime), QCPRange(-1,1));
    cPlot->setMinimumWidth(300);
    connect(m_qcfgraph,SIGNAL(exportData()), this, SLOT(onExportData()));

    /* connections*/
    connect(cPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),this,SLOT(updateXSpectrogramAxes(QCPRange )));

}


/** =============================== **/
/** =========== METHODS============ **/
/** =============================== **/

void Oscillogram::updateData(QVector<double> x, QVector<double> y)
{
    /*
    //   xData =x; yData =y; // Use to export Data // FIXME Try to get Data in QcustomPlot Conatiner directly ! Maybe should be only done in STOP Mode


    cPlot->graph(0)->addData(x,y);
    float X = cPlot->graph(0)->dataCount()/m_Data->fs;

    //Initial animation : let the rectangle at the center of the screen but still it full of samples
    if ((X-m_Data->observationTime) < (-m_Data->observationTime/2))
        cPlot->xAxis->setRange(-m_Data->observationTime/2+m_Data->rectAnalysisDuration/2,m_Data->observationTime/2 + m_Data->rectAnalysisDuration/2) ;
    else
        cPlot->xAxis->setRange(X-m_Data->observationTime,X);

    // Update Rectangle positions
    QCPRange mRange = cPlot->xAxis->range();
    m_Data->t_begin = mRange.lower + (((mRange.upper - mRange.lower) * m_Data->idx_begin_ratio) - (m_Data->rectAnalysisDuration)/2);
    m_Data->idx_begin = m_Data->fs * m_Data->t_begin;
    this->setRectPostion(QPointF(m_Data->t_begin,1),QPointF(m_Data->t_begin + (m_Data->rectAnalysisDuration),-1));

    // Update Rectangle values
    if (cPlot->graph()->dataCount()>m_Data->rectAnalysisLength)
    {
        QVector<double> mRectData;
        for (int i=0; i< m_Data->rectAnalysisLength;i++)
        {
            float y = cPlot->graph()->data().data()->at(m_Data->idx_begin+i)->value;
            mRectData.append(y);
        }
        m_Data->wrireRectData(y);
        emit(rectDataAvailable());
    }
    cPlot->replot(QCustomPlot::rpQueuedReplot); // QCustomPlot::rpQueuedReplot means when program have the time todo it
*/
}


void Oscillogram::setRectPostion(QPointF topLeft, QPointF topRight)
{

    rect->topLeft->setCoords(topLeft);
    rect->bottomRight->setCoords(topRight);
}


/** =============================== **/
/** ======= SETTERS/GETTERS ======= **/
/** =============================== **/
void Oscillogram::setYlabel(QString str)
{
    cPlot->yAxis->setLabel(str);
}


/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/


void Oscillogram::run()
{
    m_Data->ReadData(xData,yData);
    cPlot->graph(0)->addData(xData,yData);
    float X = cPlot->graph(0)->dataCount()/m_Data->fs;

    //Initial animation : let the rectangle at the center of the screen but still it full of samples
    if ((X-m_Data->observationTime) < (-m_Data->observationTime/2))
        cPlot->xAxis->setRange(-m_Data->observationTime/2+m_Data->rectAnalysisDuration/2,m_Data->observationTime/2 + m_Data->rectAnalysisDuration/2) ;
    else
        cPlot->xAxis->setRange(X-m_Data->observationTime,X);

    // Update Rectangle positions
    QCPRange mRange = cPlot->xAxis->range();
    m_Data->t_begin = mRange.lower + (((mRange.upper - mRange.lower) * m_Data->idx_begin_ratio) - (m_Data->rectAnalysisDuration)/2);
    m_Data->idx_begin = m_Data->fs * m_Data->t_begin;
    this->setRectPostion(QPointF(m_Data->t_begin,1),QPointF(m_Data->t_begin + (m_Data->rectAnalysisDuration),-1));

    // Update Rectangle values
    m_Data->rectPointsy.clear();
    for (int i=0; i< m_Data->rectAnalysisLength;i++)
    {
        float y = cPlot->graph()->data().data()->at(m_Data->idx_begin+i)->value;
        m_Data->rectPointsy.append(y);
    }
    emit(rectDataAvailable());
    cPlot->replot(QCustomPlot::rpQueuedReplot); // QCustomPlot::rpQueuedReplot means when program have the time todo it
    //cPlot->repaint();

}

void Oscillogram::updateXSpectrogramAxes(QCPRange mRange)
{
    if(m_Data->isSpectrogramShow) // TODO :: connect or disconnect signal instead
    {
        //m_Data->qPlotSpectrogram->xAxis->setRange(cPlot->xAxis->range());

        m_Data->qPlotSpectrogram->cPlot->xAxis->setRange(mRange);// Try this way
        m_Data->qPlotSpectrogram->colorMap->data()->setRange(mRange, QCPRange(0, float(m_Data->fs/2000)));
        //m_Data->qPlotSpectrogram->cPlot->gra
        if(m_Data->isLiveView==0)
        m_Data->qPlotSpectrogram->cPlot->replot();
    }
}


void Oscillogram::onExportData()
{
    if (m_Data->isLiveView)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not export in Live Mode!");
        return;
    }



    QString fname  = QFileDialog::getSaveFileName(mparent, "Save file", "", ".csv");
    if ( !fname.isEmpty())
    {
        QFile f( fname );
        f.open( QIODevice::WriteOnly );
        QTextStream stream(&f);

        for (int i=0; i<cPlot->graph()->dataCount(); i++)
            stream << cPlot->graph()->data().data()->at(i)->value << "\n";

        f.close();
    }
}

void Oscillogram::onMouseMove(QMouseEvent* event)
{
    if (m_Data->isRectMove)
    {
        if(event->buttons() == Qt::MouseButton::LeftButton) // if left mouse button pressed
        {
            double sel;
            double x,y;
            float windowsTime = m_Data->rectAnalysisDuration;
            sel=rect->selectTest(event->pos(),false,0);

            if (sel <= cPlot->selectionTolerance() || stillMove) // if in rectangle or already mouse button press
            {
                stillMove =true; // allow to make quick displacement without staying mouse cursor inside the rectangle
                cPlot->graph()->pixelsToCoords(event->pos().x(),event->pos().y(),x,y); // get mouse cursor position

                /** Move Retangle **/
                if ((x+windowsTime/2.0)>m_Data->observationTime){ // Right Limit
                    //Move highlight rectangle
                    rect->topLeft->setCoords(m_Data->observationTime- windowsTime,1);
                    rect->bottomRight->setCoords(m_Data->observationTime ,-1);
                    // Update SharedData
                    m_Data->t_begin = m_Data->observationTime- windowsTime;
                } else if ((x- windowsTime/2.0) < 0){ // Left limit
                    //Move highlight rectangle
                    rect->topLeft->setCoords(0 ,1);
                    rect->bottomRight->setCoords(windowsTime ,-1);
                    // Update SharedData
                    m_Data->t_begin = 0;
                }
                else // Nominal mode
                {
                    //Move highlight rectangle
                    rect->topLeft->setCoords(x- windowsTime/2.0 ,1);
                    rect->bottomRight->setCoords(x+windowsTime/2.0 ,-1);
                    // Update SharedData
                    m_Data->t_begin = x- windowsTime/2.0 ;
                }
            }
        }
        else
            stillMove = false;

        // Update SharedData
        m_Data->idx_begin = round(m_Data->t_begin * m_Data->fs);
        m_Data->idx_begin = m_Data->fs * m_Data->t_begin;
        QCPRange mRange = cPlot->xAxis->range();
        int idxViewBegin = mRange.lower * m_Data->fs;
        int idxViewEnd= mRange.upper * m_Data->fs;
        m_Data->idx_begin_ratio=  (float)(m_Data->idx_begin - idxViewBegin) / (float)(idxViewEnd-idxViewBegin);

        cPlot->replot(QCustomPlot::rpQueuedReplot); //only necessaary if live view is off
        emit update(); // Signal to update other graph which using rectangle (levelMeter, spectrum, octave)
    }


    if (m_Data->isTrigger)
    {
        // HERE CHANGE MOUSE CURSOR!!!

        double sel;

        /* Change Mouse cursor ** NOT WORKING */
        sel=triggerLine->selectTest(event->pos(),false,0);
        if (sel <= cPlot->selectionTolerance())
            cPlot->setCursor(Qt::SizeVerCursor);
        else
            cPlot->setCursor(Qt::ArrowCursor);



        if(event->buttons() == Qt::MouseButton::LeftButton) // if left mouse button pressed
        {
            sel=triggerLine->selectTest(event->pos(),false,0);
            if (sel <= cPlot->selectionTolerance() || stillMove)
            {
                stillMove =true; // allow to make quick displacement without staying mouse cursor inside the rectangle
                double x,y;
                cPlot->graph()->pixelsToCoords(event->pos().x(),event->pos().y(),x,y); // get mouse cursor position
                triggerLine->start->setCoords(0,y);
                triggerLine->end->setCoords(m_Data->observationTime,y);
                triggerLevel = y;
            }
        }
        else
            stillMove = false;
    }
}
