#include "spectrum.h"

#include <QDesktopWidget>
#include <QShortcut>

# include "qfgraph.h"

using namespace ffft;
Spectrum::Spectrum(DataSharer* data)
{
    m_data = data;

    // windows configuration
    setWindowTitle("FReTiAA: Spectrum");
    setWindowIcon(QIcon(":/icons/iconFFT.png"));
    QDesktopWidget wid;
    int screenWidth = wid.screen()->width();
    int screenHeight = wid.screen()->height();
    this->setGeometry((screenWidth/2)-(width()/2),(screenHeight/2)-(height()/2),width(),height());



    // Magnitude Axis
    cPlot = new QCustomPlot;
    cPlot->addGraph();
    cPlot->xAxis->setLabel("Frequency (Hz)");
    cPlot->yAxis->setLabel("Magnitude (dB SPL)");
    cPlot->xAxis->setRange(0, m_data->fs/2);
    cPlot->yAxis->setRange(-20,80);
    cPlot->graph()->setAdaptiveSampling(true);

    QFGraph *m_qcfgraph = new QFGraph(cPlot, cPlot->graph(0));
    m_qcfgraph->setDefaultXYRange(QCPRange(0,m_data->fs/2), QCPRange(-20,80));
    connect(m_qcfgraph,SIGNAL(exportData()), this, SLOT(onExportData()));

    // Phase Axis
    cPlotAngle = new QCustomPlot;
    cPlotAngle->addGraph();
    cPlotAngle->xAxis->setLabel("Frequency (Hz)");
    cPlotAngle->yAxis->setLabel("Phase (rad)");
    cPlotAngle->xAxis->setRange(0, m_data->fs/2);
    cPlotAngle->yAxis->setRange(-180,180);
    cPlotAngle->graph()->setAdaptiveSampling(true);





    // FFT and ponderation window declaration
    fft_object = new FFTReal<float>(m_data->length_fft);
    for (int i=0; i <m_data->length_fft;i++)
        hann.append(0.5 *(1.0 - cos( (2.0*M_PI * i)/(m_data->length_fft-1.0))));


    // Create the window
    QWidget *c = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_qcfgraph);
    layout->addWidget(cPlotAngle);
    c->setLayout(layout);
    this->setCentralWidget(c);

    connect(this,SIGNAL(updateTracer()),m_qcfgraph,SLOT(updateTracerText())); // NOT VERY GOOD, Should be in QFGraph

}

void Spectrum::setData(QVector<double> data,int idx_begin )
{

    float input_fft[m_data->length_fft];
    float output_fft[m_data->length_fft];

    freqValues.clear();
    fftValues.clear();
    angleValues.clear();

    int i,j;
    for( i=idx_begin, j=0; i<idx_begin+m_data->length_fft;i++, j++)
        input_fft[j] = data.at(i) * hann.at(j);

    fft_object->do_fft(output_fft,input_fft);


    for (int i=0; i<m_data->length_fft/2;i++){
        freqValues.append(float((i*1.0)/m_data->length_fft * 1.0 *m_data->fs));
        fftValues.append(20.0*log10((sqrt(output_fft[i]*output_fft[i] +output_fft[i+m_data->length_fft/2]*output_fft[i+m_data->length_fft/2]))/m_data->length_fft/2.0e-5));
        angleValues.append( 2.0 * atan(output_fft[i+m_data->length_fft/2] / output_fft[i])); // Why multipled by 2 !!
    }


    unwrap(angleValues,angleValues.size());

    cPlot->graph(0)->setData(freqValues,fftValues);
    cPlotAngle->graph(0)->setData(freqValues,angleValues);
    // phaseTracerText->setText(QString("(freq: %1, Mag: %2)").arg(phaseTracer->position->key()).arg(phaseTracer->position->value()));

    cPlot->replot();
    cPlotAngle->replot();

    emit updateTracer();// NOT VERY GOOD, Should be in QFGraph
}


void Spectrum::closeEvent( QCloseEvent* event )
{
    emit spectrumIsClosing();
    event->accept();
}


void Spectrum::onExportData()
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
        QFile f( fname );
        f.open( QIODevice::WriteOnly );
        QTextStream stream(&f);
        for (int i=0; i<m_data->length_fft/2; i++)
            stream << freqValues.at(i) << "," << fftValues.at(i) << endl;
        f.close();
    }
}

void unwrap(QVector<double> &p, int N)
// ported from matlab (Dec 2002)
{
    float dp[p.size()];
    float dps[p.size()];
    float dp_corr[p.size()];
    float cumsum[p.size()];
    float cutoff = M_PI;               /* default value in matlab */
    int j;

    assert(N <= p.size());

    // incremental phase variation
    // MATLAB: dp = diff(p, 1, 1);
    for (j = 0; j < N-1; j++)
        dp[j] = p.at(j+1) - p.at(j);

    // equivalent phase variation in [-pi, pi]
    // MATLAB: dps = mod(dp+dp,2*pi) - pi;
    for (j = 0; j < N-1; j++)
        dps[j] = (dp[j]+M_PI) - floor((dp[j]+M_PI) / (2*M_PI))*(2*M_PI) - M_PI;

    // preserve variation sign for +pi vs. -pi
    // MATLAB: dps(dps==pi & dp>0,:) = pi;
    for (j = 0; j < N-1; j++)
        if ((dps[j] == -M_PI) && (dp[j] > 0))
            dps[j] = M_PI;

    // incremental phase correction
    // MATLAB: dp_corr = dps - dp;
    for (j = 0; j < N-1; j++)
        dp_corr[j] = dps[j] - dp[j];

    // Ignore correction when incremental variation is smaller than cutoff
    // MATLAB: dp_corr(abs(dp)<cutoff,:) = 0;
    for (j = 0; j < N-1; j++)
        if (fabs(dp[j]) < cutoff)
            dp_corr[j] = 0;

    // Find cumulative sum of deltas
    // MATLAB: cumsum = cumsum(dp_corr, 1);
    cumsum[0] = dp_corr[0];
    for (j = 1; j < N-1; j++)
        cumsum[j] = cumsum[j-1] + dp_corr[j];

    // Integrate corrections and add to P to produce smoothed phase values
    // MATLAB: p(2:m,:) = p(2:m,:) + cumsum(dp_corr,1);
    for (j = 1; j < N; j++)
    {
        float t;
        t=p.at(j);
        p.replace(j, t + cumsum[j-1] );
    }
    // p[j] += cumsum[j-1];
}
