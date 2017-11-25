#include "spectrogram.h"
#include "xyrangedialog.h"
#include "windows.h"
using namespace ffft;

Spectrogram::Spectrogram(DataSharer* data, QWidget*)
{

    m_data = data;

    /*Create graphic*/
    cPlot->xAxis->setLabel("Time (s)");
    cPlot->yAxis->setLabel("Frequency (Hz)");
    cPlot->yAxis->setRange(0,m_data->fs/2000);
    cPlot->xAxis->setRange(0,m_data->observationTime);
    cPlot->yAxis->setNumberFormat("f");
    cPlot->yAxis->setNumberPrecision(1);
    cPlot->axisRect()->setupFullAxesBox(true);
    cPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);


    // add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(cPlot);
    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorScale->axis()->setLabel("Magnitude (dB SPL)");
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    cPlot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect

    /* Create ColorMap*/
    nx = m_data->observationTime*m_data->fs / (m_data->length_fft_spectrogram/2);
    ny = m_data->length_fft_spectrogram/2;
    colorMap->data()->setRange(QCPRange(0, m_data->observationTime), QCPRange(0, float(m_data->fs/2000))); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
    colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
    colorMap->setInterpolate(true);
    colorMap->setAntialiased(true);
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    colorMap->setGradient(QCPColorGradient::gpIon);
    colorMap->setDataRange(QCPRange(-20,50));

    /*Analysis Rectangle */
    rect = new QCPItemRect(cPlot);
    rect->setBrush(QBrush(QColor(0,0,255,70) ));
    rect->setSelectable(false);
    rect->topLeft->setCoords(m_data->observationTime-(m_data->length_fft/m_data->fs) ,m_data->fs/2000);
    rect->bottomRight->setCoords(m_data->observationTime ,0);

    /*Init Fast Fourier Transform */
    fft_object = new FFTReal<float>(m_data->length_fft_spectrogram);

    /* Compute hanning window */
    Windows *m_win = new Windows(Windows::hanning,m_data->length_fft);
    win = m_win->getWin();
   /* for (int i=0; i <m_data->length_fft_spectrogram;i++)
        hann.append(0.5 *(1.0 - cos( (2.0*M_PI * i)/(m_data->length_fft_spectrogram-1.0))));
*/
    /* Add graph to QCFGraph*/
    m_qcfgraph = new QFGraph(cPlot,colorMap);
    m_qcfgraph->setDefaultXYRange(QCPRange(0,m_data->observationTime), QCPRange(0,m_data->fs/2000));
    // m_qcfgraph->setToolBarVisible(false);
    connect(m_qcfgraph,SIGNAL(exportData()), this, SLOT(onExportData()));

    /* Conections */
    connect(this,SIGNAL(updateTracer()),m_qcfgraph,SLOT(updateTracerText())); // NOT VERY GOOD, Should be in QFGraph
    connect(cPlot->xAxis,SIGNAL(rangeChanged(QCPRange)),this,SLOT(updateXSpectrogramAxes(QCPRange)));


}

/** =============================== **/
/** ========== METHODS ============ **/
/** =============================== **/
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
            input_fft[j] = data.at(i+j) * win.at(j);

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




/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/
void Spectrogram::updateXSpectrogramAxes(QCPRange)
{
    m_data->qPlotOscillogram->xAxis->setRange(cPlot->xAxis->range());
}


void Spectrogram::onExportData()
{
    if (m_data->isLiveView)
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Could not export in Live Mode!");
        return;
    }

    QString fname  = QFileDialog::getSaveFileName(this, "Save file", "", ".csv");
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
