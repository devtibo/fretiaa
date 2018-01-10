#include "spectrumonshot.h"

#include <QDesktopWidget>
#include <QShortcut>

# include "qfgraph.h"

#include "windows.h"


SpectrumWinOnShot::SpectrumWinOnShot(QWidget *)
{
    ;
}

// OverLoad CLOSE Request
void SpectrumWinOnShot::closeEvent( QCloseEvent* event )
{
    emit spectrumIsClosing();
    event->accept();
}



using namespace ffft;
SpectrumOnShot::SpectrumOnShot(DataSharer* data, QWidget* parent)
{
    mParent = parent;
    m_data = data;

    // windows configuration
    mWin = new SpectrumWinOnShot();
    mWin->setWindowTitle("FReTiAA: Spectrum");
    mWin->setWindowIcon(QIcon(":/icons/iconFFT.png"));
    QDesktopWidget wid;
    int screenWidth = wid.screen()->width();
    int screenHeight = wid.screen()->height();
    mWin->setGeometry((screenWidth/2)-(mWin->width()/2),(screenHeight/2)-(mWin->height()/2),mWin->width(),mWin->height());



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
    fft_object = new FFTReal<float>(m_data->rectAnalysisLength);

    Windows *m_win = new Windows(Windows::hanning,m_data->rectAnalysisLength);
    win = m_win->getWin();
    /*for (int i=0; i <m_data->length_fft;i++)
        hann.append(0.5 *(1.0 - cos( (2.0*M_PI * i)/(m_data->length_fft-1.0))));
*/

    // Create the window
    QWidget *c = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_qcfgraph);
    layout->addWidget(cPlotAngle);
    c->setLayout(layout);
    mWin->setCentralWidget(c);


}

void SpectrumOnShot::updateData()
{

    QVector<double> data = m_data->ReadRectData();
    float input_fft[m_data->rectAnalysisLength];
    float output_fft[m_data->rectAnalysisLength];

    freqValues.clear();
    fftValues.clear();
    angleValues.clear();

    int i;
    for( i=0; i<m_data->rectAnalysisLength;i++)
        input_fft[i] = data.at(i) * win.at(i);

    fft_object->do_fft(output_fft,input_fft);


    for (int i=0; i<m_data->rectAnalysisLength/2;i++){
        //freqValues.append(float((i*1.0)/m_data->length_fft * 1.0 *m_data->fs));
        freqValues.append(i/(m_data->rectAnalysisLength/2.0-1.0) * m_data->fs/2.0); // TO BE VERIFY
        fftValues.append(20.0*log10((sqrt(output_fft[i]*output_fft[i] +output_fft[i+m_data->rectAnalysisLength/2]*output_fft[i+m_data->rectAnalysisLength/2]))/m_data->rectAnalysisLength/2.0e-5));
        angleValues.append( 2.0 * atan(output_fft[i+m_data->rectAnalysisLength/2] / output_fft[i])); // Why multipled by 2 !!
    }


    this->unwrap(angleValues,angleValues.size());

    cPlot->graph(0)->setData(freqValues,fftValues);
    cPlotAngle->graph(0)->setData(freqValues,angleValues);
    // phaseTracerText->setText(QString("(freq: %1, Mag: %2)").arg(phaseTracer->position->key()).arg(phaseTracer->position->value()));

    cPlot->replot(QCustomPlot::rpQueuedReplot);
    cPlotAngle->replot(QCustomPlot::rpQueuedReplot);
}



void SpectrumOnShot::onExportData()
{

    QMessageBox messageBox;

    if (m_data->isLiveView)
    {
        messageBox.critical(0,"Error","Could not export in Live Mode!");
        return;
    }

    QString fname  = QFileDialog::getSaveFileName(mParent, "Save file", "", "Comma Sperated Values (*.csv)");
    if ( !fname.isEmpty())
    {
        QFile f( fname );
        f.open( QIODevice::WriteOnly );
        QTextStream stream(&f);
        for (int i=0; i<m_data->rectAnalysisLength/2; i++)
            stream << freqValues.at(i) << "," << fftValues.at(i) << endl;
        f.close();
    }
}

// see: https://www.medphysics.wisc.edu/~ethan/phaseunwrap/unwrap.c
void SpectrumOnShot::unwrap(QVector<double> &p, int N)
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
