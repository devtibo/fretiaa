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
    cPlot->xAxis->setRange(-m_Data->observationTime/2,m_Data->observationTime/2);
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

/*void Oscillogram::updateData(QVector<double> x, QVector<double> y)
{

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

}*/


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


    // Update Triger Line i
    if (m_Data->isTrigger)
    {
        triggerLine->start->setCoords( mRange.lower,triggerLevel);
        triggerLine->end->setCoords(mRange.upper,triggerLevel);
    }


    // Update Rectangle values
    //m_Data->rectPointsy.clear();
    //m_Data->ClearRectData();
    QVector<double> p;
    for (int i=0; i< m_Data->rectAnalysisLength;i++)
    {
        float y = cPlot->graph()->data().data()->at(m_Data->idx_begin+i)->value;
        p.append(y);
    }

    m_Data->writeRectData(p);
    emit(rectDataAvailable());
    //cPlot->replot(); // Cause CRASH !!!!
    cPlot->replot(QCustomPlot::rpQueuedReplot); // QCustomPlot::rpQueuedReplot means when program have the time todo it
    //cPlot->repaint();

}

void Oscillogram::updateXSpectrogramAxes(QCPRange mRange)
{

    if(m_Data->isSpectrogramShow) // TODO :: connect or disconnect signal instead
    {
        //m_Data->qPlotSpectrogram->xAxis->setRange(cPlot->xAxis->range());
        // QCPRange mRange = cPlot->xAxis->range();
        m_Data->qPlotSpectrogram->cPlot->xAxis->setRange(QCPRange(mRange.lower + ((float)m_Data->length_fft_spectrogram/2.0)/m_Data->fs, mRange.upper-((float)m_Data->length_fft_spectrogram/2.0)/m_Data->fs));// Try this way
        m_Data->qPlotSpectrogram->colorMap->data()->setRange(mRange, QCPRange(0, float(m_Data->fs/2000)));

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



    QString fname  = QFileDialog::getSaveFileName(mparent, "Save file", "", "Comma Separated Values(*.csv)");
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

    // Solution 1 : try to use pixels instead of x coordinates which may change during rectangle displacement

    if (m_Data->isRectMove) // Manage displacement of Analyse Rectangle
    {
        if(event->buttons() == Qt::MouseButton::LeftButton) // if left mouse button pressed
        {
            double sel;
            double x,y;
            sel=rect->selectTest(event->pos(),false,0);

            if (sel <= cPlot->selectionTolerance() || stillMove) // if mouse is in rectangle or already mouse button press
            {
                stillMove =true; // allow to make quick displacement without staying mouse cursor inside the rectangle
                
                cPlot->graph()->pixelsToCoords(event->pos().x(),event->pos().y(),x,y); // get mouse cursor position

                QPoint pts_tl = cPlot->axisRect()->topLeft();

                int x_pixels = event->pos().x();
                // Remove half width of rectangle so that the mouse always point te middle of the rectangle
                // int rectWidth = round(rect->bottomRight->pixelPosition().x()- rect->topLeft->pixelPosition().x());

                float rectPosRatio = (float)(x_pixels-(pts_tl.x()))/(float)cPlot->axisRect()->width();

                if(rectPosRatio < 0)
                    rectPosRatio = 0;

                if(rectPosRatio > 1)
                    rectPosRatio = 1;

                m_Data->idx_begin_ratio = rectPosRatio;
                QCPRange mRange = cPlot->xAxis->range();
                m_Data->t_begin = mRange.lower + (((mRange.upper - mRange.lower) * m_Data->idx_begin_ratio) - (m_Data->rectAnalysisDuration)/2);

                if (m_Data->t_begin < mRange.lower)
                    m_Data->t_begin = mRange.lower;

                if (m_Data->t_begin > (mRange.upper-m_Data->rectAnalysisDuration))
                    m_Data->t_begin = (mRange.upper-m_Data->rectAnalysisDuration);

                m_Data->idx_begin = m_Data->fs * m_Data->t_begin;
                this->setRectPostion(QPointF(m_Data->t_begin,1),QPointF(m_Data->t_begin + (m_Data->rectAnalysisDuration),-1));

                /** Move Retangle *
                if ((x+windowsTime/2.0)>m_Data->observationTime){ // Right Limit    int idx_begin;
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
                }*/


            }
        }
        else
            stillMove = false;

        /*
        // WARNING : NEED TO BE CHANGED !!!!!
        // Update SharedData
        m_Data->idx_begin = round(m_Data->t_begin * m_Data->fs);
        m_Data->idx_begin = m_Data->fs * m_Data->t_begin;
        QCPRange mRange = cPlot->xAxis->range();
        int idxViewBegin = mRange.lower * m_Data->fs;
        int idxViewEnd= mRange.upper * m_Data->fs;
        m_Data->idx_begin_ratio=  (float)(m_Data->idx_begin - idxViewBegin) / (float)(idxViewEnd-idxViewBegin);
        // !!!!
*/

        cPlot->replot(QCustomPlot::rpQueuedReplot); //only necessaary if live view is off


        // Update Rectangle values
        m_Data->rectPointsy.clear();
        for (int i=0; i< m_Data->rectAnalysisLength;i++)
        {
            float y = cPlot->graph()->data().data()->at(m_Data->idx_begin+i)->value;
            m_Data->rectPointsy.append(y);
        }
        emit(rectDataAvailable());
    }


    if (m_Data->isTrigger) // Managed Trigger Action
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

                QCPRange mRange = cPlot->xAxis->range();

                stillMove =true; // allow to make quick displacement without staying mouse cursor inside the rectangle
                double x,y;
                cPlot->graph()->pixelsToCoords(event->pos().x(),event->pos().y(),x,y); // get mouse cursor position
                triggerLine->start->setCoords(mRange.lower,y);
                triggerLine->end->setCoords(mRange.upper,y);
                triggerLevel = y;

                cPlot->replot(QCustomPlot::rpQueuedReplot); //only necessaary if live view is off

            }
        }
        else
            stillMove = false;
    }
}
