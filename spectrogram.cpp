#include "spectrogram.h"
#include "xyrangedialog.h"
using namespace ffft;

Spectrogram::Spectrogram(DataSharer* data)
{

    m_data = data;
    cPlot->xAxis->setLabel("Time (s)");
    cPlot->yAxis->setLabel("Frequency (Hz)");

    cPlot->yAxis->setRange(0,m_data->fs/2000);
    cPlot->xAxis->setRange(0,m_data->observationTime);
    cPlot->yAxis->setNumberFormat("f");
    cPlot->yAxis->setNumberPrecision(1);


    cPlot->axisRect()->setupFullAxesBox(true);


    nx = m_data->observationTime*m_data->fs / (m_data->length_fft_spectrogram/2);
    ny = m_data->length_fft_spectrogram/2;
    //cPlot->xAxis->setRange(0,nx);
    colorMap->data()->setRange(QCPRange(0, m_data->observationTime), QCPRange(0, float(m_data->fs/2000))); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions

    colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points



    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(cPlot);
    cPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorScale->axis()->setLabel("Magnitude (dB SPL)");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpIon);
    // we could have also created a QCPColorGradient instance and added own colors to
    // the gradient, see the documentation of QCPColorGradient for what's possible.

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):

    cPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    colorMap->setDataRange(QCPRange(-20,50));
    colorMap->setInterpolate(true);
    colorMap->setAntialiased(true);
    cPlot->rescaleAxes();

    fft_object = new FFTReal<float>(m_data->length_fft_spectrogram);

    for (int i=0; i <m_data->length_fft_spectrogram;i++)
        hann.append(0.5 *(1.0 - cos( (2.0*M_PI * i)/(m_data->length_fft_spectrogram-1.0))));


    connect(cPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),this,SLOT(updateXSpectrogramAxes(QCPRange)));




    rect = new QCPItemRect(cPlot);
    rect->setBrush(QBrush(QColor(0,0,255,70) ));
    rect->setSelectable(false);

    rect->topLeft->setCoords(m_data->observationTime-(m_data->length_fft/m_data->fs) ,m_data->fs/2000);
    rect->bottomRight->setCoords(m_data->observationTime ,0);

    /* Contextual Menu
    cPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect (cPlot, SIGNAL(customContextMenuRequested(QPoint)), this,SLOT( onCustomContextMenuRequested(QPoint)));


    cPlot->setInteractions(QCP::iSelectAxes);
    connect(cPlot,SIGNAL(           axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),this,SLOT(onAxisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)));
*/



    m_qcfgraph = new QFGraph(cPlot,colorMap);
    m_qcfgraph->setDefaultXYRange(QCPRange(0,m_data->observationTime), QCPRange(0,m_data->fs/2000));
    // m_qcfgraph->setToolBarVisible(false);
    connect(m_qcfgraph,SIGNAL(exportData()), this, SLOT(onExportData()));
    connect(this,SIGNAL(updateTracer()),m_qcfgraph,SLOT(updateTracerText())); // NOT VERY GOOD, Should be in QFGraph

}


QCustomPlot* Spectrogram::graphViewer()
{
    return cPlot;
}
void Spectrogram::update(QVector<double> data)
{
    float input_fft[m_data->length_fft_spectrogram];
    float output_fft[m_data->length_fft_spectrogram];

    int idx = 0;
    int i,j,k;
    double z;
    for (i=0; i< data.size()-m_data->length_fft_spectrogram ; i+= m_data->length_fft_spectrogram/2)
    {
        for (j=0;j<m_data->length_fft_spectrogram; j++)
            input_fft[j] = data.at(i+j) * hann.at(j);

        fft_object->do_fft(output_fft,input_fft);

        for (k=0; k<m_data->length_fft_spectrogram/2;k++){
            z=20.0*log10((sqrt(output_fft[k]*output_fft[k]+output_fft[k+m_data->length_fft_spectrogram/2] \
                          *output_fft[k+m_data->length_fft_spectrogram/2]))/m_data->length_fft_spectrogram/2.0e-5);
            colorMap->data()->setCell(idx, k, z);
        }
        idx++;
    }

    emit updateTracer();
    cPlot->replot();
}


void Spectrogram::updateXSpectrogramAxes(QCPRange)
{
    qDebug("BeforeReplot");
    m_data->qPlotOscillogram->xAxis->setRange(cPlot->xAxis->range());
}



/*

void  Spectrogram::onAxisDoubleClick(QCPAxis* m_axis,QCPAxis::SelectablePart m_selectedParts,QMouseEvent* event)
{

    QPoint posInt;
    posInt.setX(round(event->screenPos().x()));
    posInt.setY(round(event->screenPos().y()));
    resizeAxisDialog(m_axis, posInt);
}

void Spectrogram::onCustomContextMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(cPlot);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    contextPos = pos;
    menu->addAction("Resize X-Axis", this, SLOT(onXAxisResize()));
    menu->addAction("Resize Y-Axis", this, SLOT(onYAxisResize()));
    menu->addAction("Export Data", this, SLOT(onExportData()));
    menu->addAction("Reset Axis",this,SLOT(onResetAxis()));
    menu->popup(pos);
}


void Spectrogram::resizeAxisDialog(QCPAxis * m_axis, QPoint pos)
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

void Spectrogram::onXAxisResize()
{
    resizeAxisDialog(cPlot->xAxis, contextPos);
}

void Spectrogram::onYAxisResize()
{
    resizeAxisDialog(cPlot->yAxis, contextPos);
}


void Spectrogram::onResetAxis()
{
    cPlot->xAxis->setRange(0,m_data->observationTime);
    cPlot->yAxis->setRange(0,m_data->fs/2000);
    cPlot->replot();
}
*/
void Spectrogram::onExportData()
{
    QMessageBox messageBox;

    if (m_data->isLiveView)
    {
        messageBox.critical(0,"Error","Could not export in Live Mode!");
        return;
    }
    QString fname;
    fname  = QFileDialog::getSaveFileName(this, "Save file", "", ".csv");

    if ( !fname.isEmpty())
    {
        QFile f( fname + ".csv" );
        QFile f2(fname + ".x");
        QFile f3(fname + ".y");
        f.open( QIODevice::WriteOnly );
        f2.open( QIODevice::WriteOnly );
        f3.open( QIODevice::WriteOnly );
        QTextStream stream(&f);
        QTextStream stream2(&f2);
        QTextStream stream3(&f3);

        for (int i=0; i< colorMap->data()->keySize(); i++)
            stream2 <<  1.0*i /colorMap->data()->keySize() * m_data->observationTime << endl;

        for (int i=0; i< colorMap->data()->valueSize(); i++)
            stream3 <<  1.0*i /colorMap->data()->valueSize() * m_data->fs/2.0 << endl;

        for( int i=0; i < ny ; i++)
        {    for (int j=0; j <nx; j++)
            {
                stream << colorMap->data()->cell(i,j);
                if (j<(nx-1))
                    stream << ",";
            }
            stream << endl;
        }
        f.close();
        f2.close();
        f3.close();
    }
}


/*
void Spectrogram::setData(QVector<double> data)
{
    float input_fft[m_data->length_fft_spectrogram];
    float output_fft[m_data->length_fft_spectrogram];

    QVector<double> freqValues;
    QVector<double> fftValues;


    int i,j;
    for( i=data.size() -m_data->length_fft_spectrogram, j=0; i<data.size();i++, j++)
        input_fft[j] = data.at(i) * hann.at(j);

    fft_object->do_fft(output_fft,input_fft);


    for (int i=0; i<m_data->length_fft_spectrogram/2;i++){
        freqValues.append(float((i*1.0)/m_data->length_fft_spectrogram * 1.0 *m_data->fs));
        fftValues.append(20.0*log10((sqrt(output_fft[i]*output_fft[i] +output_fft[i+m_data->length_fft_spectrogram/2]*output_fft[i+m_data->length_fft_spectrogram/2]))/m_data->length_fft_spectrogram/2.0e-5));
    }
void Oscillogram::updateXSpectrogramAxes(QCPRange)
{
    qDebug("BeforeReplot");
 m_Data->qPlotSpectrogram->xAxis->setRange(cPlotOscillogram->xAxis->range());
}


    double x, y, z;
    if (begin_ctr<nx)
    {
        for (int yIndex=0; yIndex<ny; ++yIndex)
        {
            //colorMap->data()->cellToCoord(nx-2, yIndex, &x, &y);
            z = fftValues.at(yIndex);
            colorMap->data()->setCell(begin_ctr, yIndex, z);

        }
          begin_ctr++;
    }else
    {
       for (int xIndex=1; xIndex<nx; ++xIndex)
       {
           for (int yIndex=0; yIndex<ny; ++yIndex)
           {
               //colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
               z = colorMap->data()->cell(xIndex,yIndex);
               colorMap->data()->setCell(xIndex-1, yIndex, z);
           }
       }

       for (int yIndex=0; yIndex<ny; ++yIndex)
       {
           //colorMap->data()->cellToCoord(nx-2, yIndex, &x, &y);
           z = fftValues.at(yIndex);
           colorMap->data()->setCell(nx-1, yIndex, z);
       }
}
       cPlot->replot();;
void Oscillogram::updateXSpectrogramAxes(QCPRange)
{
    qDebug("BeforeReplot");
 m_Data->qPlotSpectrogram->xAxis->setRange(cPlotOscillogram->xAxis->range());
}


void Oscillogram::updateXSpectrogramAxes(QCPRange)
{
    qDebug("BeforeReplot");
 m_Data->qPlotSpectrogram->xAxis->setRange(cPlotOscillogram->xAxis->range());
}

    //cPlot->graph(0)->addData();
}*/ // SPECTROGRAM V1
