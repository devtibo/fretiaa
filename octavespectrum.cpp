#include "octavespectrum.h"
#include <QDesktopWidget>
#include "qfgraph.h"

OctaveSpectrum::OctaveSpectrum(DataSharer* data)
{
    m_data = data;

    // Objects declarations
    m_filter = new OneNOctaveFilters(m_data);
    cPlot = new QCustomPlot;
    newBars = new QCPBars(cPlot->xAxis, cPlot->yAxis);

    // Window configuration
    setWindowTitle("FReTiAA: Octave Spectrum");
    setWindowIcon(QIcon(":/icons/iconOctave.png"));

    // Set Windows Size
    QDesktopWidget wid;
    int screenWidth = wid.screen()->width();
    int screenHeight = wid.screen()->height();
    this->setGeometry((screenWidth/2)-(width()/2),(screenHeight/2)-(height()/2),width(),height());

    // Initial Axes Configuration
    cPlot->yAxis->setRange(-20,100);
    cPlot->xAxis->setRange(-1,m_data->numOctaveFilters);
    cPlot->xAxis->setLabel("Third-Octave Frequencies (Hz)");
    cPlot->yAxis->setLabel("Magnitude (dB SPL)");

    // Set X-Axis Labels
    labels  <<  "25" << "31.5"<< "40" << "50" << "63" <<"80"<<"100" <<"125"<< "160" <<"200"<< "250"<< "315" <<"400" <<"500"<< "630"<< "800" <<"1000"<< "1250"<< "1600" <<"2000"<<"2500" <<"3150"<< "4000"<<"5000"<< "6300"<< "8000" <<"10000"<< "12500"<< "16000"<< "20000";
    QVector<double> ticks;
    for(int i=0; i<m_data->numOctaveFilters; i++)
        ticks.append(i);
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    cPlot->xAxis->setTicker(textTicker);

    // Include BarPlot in QCFGraph
    QFGraph *m_qcfgraph = new QFGraph(cPlot, newBars);
    m_qcfgraph->setDefaultXYRange(QCPRange(-1,m_data->numOctaveFilters), QCPRange(-20,100));
    connect(m_qcfgraph,SIGNAL(exportData()), this, SLOT(onExportData()));

    // Set CentralWWidget
    this->setCentralWidget(m_qcfgraph);

    // Connection
    connect(this,SIGNAL(updateTracer()),m_qcfgraph,SLOT(updateTracerText()));// NOT VERY GOOD, Should be in QFGraph

}


// OverLoad CLOSE Request
void OctaveSpectrum::closeEvent( QCloseEvent* event )
{
    emit octaveIsClosing();
    event->accept();
}

/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/

// Slot: Update
void OctaveSpectrum::setData(QVector<double> data,int idx_begin )
{
    //ORDRE 6 /Devrai suffir 2 ou filtre en HF hors classe
    // Declaration
    float rmsValues;
    int idx,j,i;

    //Reset variables
    rmsValuesVec.clear();
    xData.clear();

    for( i=0; i<m_data->numOctaveFilters; i++)
    {
        xData.append(i);

        // reset loop-variables
        rmsValues = 0.0;
        valuefilt_vec.clear();
        valuefilt_vec2.clear();
        valuefilt_vec3.clear();

        // Apply filters
        for (idx = idx_begin,j=0; idx < (idx_begin+m_data->length_fft); idx++,j++) {
            valuefilt_vec.append(m_filter->filters_FcHigh_stage1.at(i)->process(m_filter->filters_FcLow_stage1.at(i)->process(data.at(idx))));
            valuefilt_vec2.append(m_filter->filters_FcHigh_stage2.at(i)->process(m_filter->filters_FcLow_stage2.at(i)->process(valuefilt_vec.at(j))));
            valuefilt_vec3.append(m_filter->filters_FcHigh_stage3.at(i)->process(m_filter->filters_FcLow_stage3.at(i)->process(valuefilt_vec2.at(j))));
            //rmsValues  = data.at(idx);//(float)rmsValues + (valuefilt_vec3.at(j).y()*valuefilt_vec3.at(j).y()) / (float)m_data->length_fft;
            // Compute RMS
            rmsValues  = (float)rmsValues + (valuefilt_vec.at(j)*valuefilt_vec.at(j)) ;
        }

        // Convert to  dB SPL
        rmsValues/= (float)m_data->length_fft;
        rmsValues = 20.0*log10(sqrt(rmsValues)/2e-5);
        rmsValuesVec.append(rmsValues);
    }

    // Update DATA
    newBars->setData(xData,rmsValuesVec); // NOT VERY GOOD SHOULD BE DONE USING SIGNAL/SLOT

    // Update view
    emit updateTracer();// NOT VERY GOOD, Should be in QFGraph
    cPlot->replot();
}




// Slot: Export Data
void OctaveSpectrum::onExportData()
{
    //Exit if application is in LiveMode
    if (m_data->isLiveView)
    {
        QMessageBox::critical(0,"Error","Could not export in Live Mode!");
        return;
    }

    // Open dialogBox
    QFileDialog m_fileDialog;
    QString fname  = m_fileDialog.getSaveFileName(this, "Save file", "", ".csv");

    // Save Data if file name is not empty and if SAVE button was Clicked
    if ( !fname.isEmpty() && m_fileDialog.result()==QFileDialog::Accept)
    {
        QFile f( fname );
        f.open( QIODevice::WriteOnly );
        QTextStream stream(&f);
        for (int i=0; i<m_data->numOctaveFilters; i++)
            stream << labels.at(i) << "," << rmsValuesVec.at(i) << endl;
        f.close();
    }
}
