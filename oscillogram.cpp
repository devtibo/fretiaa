#include "oscillogram.h"
#include "xyrangedialog.h"

//Oscillogram::Oscillogram(QWidget *parent) : QWidget(parent)
Oscillogram::Oscillogram(DataSharer *data)
{
    m_Data = data;


    cPlot = new QCustomPlot();





    cPlot->addGraph();

    cPlot->xAxis->setLabel("Time (s)");
    cPlot->yAxis->setLabel("Pressure (Pa)");
    cPlot->yAxis->setRange(-1,1);
    cPlot->xAxis->setRange(0,m_Data->observationTime);

    cPlot->setInteraction(QCP::Interaction::iSelectItems);
    cPlot->graph()->setAdaptiveSampling(true);

    rect = new QCPItemRect(cPlot);
    rect->setBrush(QBrush(QColor(0,0,255,70) ));
    rect->setSelectable(false);

    rect->topLeft->setCoords(m_Data->observationTime-(m_Data->length_fft/m_Data->fs) ,1);
    rect->bottomRight->setCoords(m_Data->observationTime ,-1);







    // connect(cPlot,SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(testMove(QMouseEvent*)));
    /*
    cPlot->setInteractions(QCP::iSelectAxes);
    connect(cPlot,SIGNAL(           axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),this,SLOT(onAxisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)));


    cPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect (cPlot, SIGNAL(customContextMenuRequested(QPoint)), this,SLOT( onCustomContextMenuRequested(QPoint)));
*/
    //connect(cPlot,SIGNAL(beforeReplot()),this, SLOT(updateXSpectrogramAxes()));)
    connect(cPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),this,SLOT(updateXSpectrogramAxes(QCPRange)));




    //! /*Trigger*/
    triggerLine = new QCPItemLine(cPlot);
    triggerLine->start->setCoords(0,0.5);
    triggerLine->end->setCoords(data->observationTime,0.5);

    triggerLine->setPen(QPen(Qt::gray, 2.0, Qt::DashDotLine));

    //triggeredMode = true;
    cPlot->setInteraction(QCP::iSelectItems,true);
    triggerLevel = 0.5;
    triggerLine->setSelectable(true);
    //connect(triggerLine,SIGNAL())
    triggerLine->setVisible(false);



    m_qcfgraph = new QFGraph(cPlot,cPlot->graph(0));
    m_qcfgraph->setDefaultXYRange(QCPRange(0,m_Data->observationTime), QCPRange(-1,1));
    connect(m_qcfgraph,SIGNAL(exportData()), this, SLOT(onExportData()));

    connect(this,SIGNAL(updateTracer()),m_qcfgraph,SLOT(updateTracerText())); // NOT VERY GOOD, Should be in QFGraph

}


void Oscillogram::updateData(QVector<double> x, QVector<double> y)
{
    xData =x; yData =y; // Use to export Data // FIXME Try to get Data in QcustomPlot Conatiner directly ! Maybe should be only done in STOP Mode
    cPlot->graph(0)->setData(x,y);
    cPlot->replot();

    emit updateTracer(); // NOT VERY GOOD, Should be in QFGraph
}

QCustomPlot* Oscillogram::graphViewer()
{
    return cPlot;
}

void Oscillogram::setYlabel(QString str)
{
    cPlot->yAxis->setLabel(str);
}

void Oscillogram::onMouseMove(QMouseEvent* event)
{
    if (m_Data->isRectMove)
    {
        if(event->buttons() == Qt::MouseButton::LeftButton) // if left mouse button pressed
        {
            double sel;
            double x,y;
            float windowsTime = m_Data->length_fft *1.0 / m_Data->fs;
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

        cPlot->replot(); //only necessaary if live view is off
        emit update();
    }


    if (m_Data->isTrigger)
    {
        double sel;
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



void Oscillogram::updateRect()
{
    float windowsTime = m_Data->length_fft *1.0 / m_Data->fs;
    rect->topLeft->setCoords(m_Data->observationTime- windowsTime ,1);
    rect->bottomRight->setCoords(m_Data->observationTime ,-1);
}





//! ========= CHANGE AXES FUNCTIONS AND SLOTS
//!
//!

/*
void Oscillogram::resizeAxisDialog(QCPAxis * m_axis, QPoint pos)
{
    // Create Dialog
    XYRangeDialog *dialog = new XYRangeDialog(this,m_axis->range().lower,m_axis->range().upper);

    // Place the dialog box near the mouse pointer
    dialog->move(pos);

    // Process when OK button is clicked
    if (dialog->exec() == QDialog::Accepted) {
        m_axis->setRange(dialog->val1, dialog->val2);
        cPlot->replot();
    }
}

// FIXME FIND A WAY TO PASS VALUES (QCPAxis to SLOTs)
void  Oscillogram::onAxisDoubleClick(QCPAxis* m_axis,QCPAxis::SelectablePart m_selectedParts,QMouseEvent* event)
{
    QPoint posInt;
    posInt.setX(round(event->screenPos().x()));
    posInt.setY(round(event->screenPos().y()));
    resizeAxisDialog(m_axis, posInt);
}

void Oscillogram::onCustomContextMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(cPlot);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    contextPos = pos;
    menu->addAction("Resize X-Axis", this, SLOT(onXAxisResize()));
    menu->addAction("Resize Y-Axis", this, SLOT(onYAxisResize()));
    menu->addAction("Export Data", this, SLOT(onExportData()));
    menu->addAction("Reset Axis", this, SLOT(onResetAxis()));
    menu->popup(cPlot->mapToGlobal(pos));
}

void Oscillogram::onXAxisResize()
{
    resizeAxisDialog(cPlot->xAxis, contextPos);
}

void Oscillogram::onYAxisResize()
{
    resizeAxisDialog(cPlot->yAxis, contextPos);
}


void Oscillogram::onResetAxis()
{
    cPlot->xAxis->setRange(0,m_Data->observationTime);
    cPlot->yAxis->setRange(-1,1);
    cPlot->replot();
}

*/
void Oscillogram::onExportData()
{   connect(m_qcfgraph,SIGNAL(exportData()), this, SLOT(onExportData()));

    QMessageBox messageBox;

    if (m_Data->isLiveView)
    {
        messageBox.critical(0,"Error","Could not export in Live Mode!");
        return;
    }


    QString fname;
    fname  = QFileDialog::getSaveFileName(this, "Save file", "", ".csv");

    if ( !fname.isEmpty())
    {
        QFile f( fname );
        f.open( QIODevice::WriteOnly );
        QTextStream stream(&f);
        for (int i=0; i<m_Data->data_length; i++)
            stream << xData.at(i) << "," << yData.at(i) << endl;
        f.close();
    }
}


void Oscillogram::updateXSpectrogramAxes(QCPRange)
{
    m_Data->qPlotSpectrogram->xAxis->setRange(cPlot->xAxis->range());
}


/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/

