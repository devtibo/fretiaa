#include "mainwindow.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QDebug>

#include "audioinputconfigdial.h"
#include "gainconfigdial.h"
#include "aboutdialog.h"
#include "qfgraph.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //![] Init AudioEngine
    m_AudioEngine = new AudioEngine();// default input
    m_AudioEngine->soundBufferSize = 1024;
    dataSound = new unsigned char [m_AudioEngine->soundBufferSize];

    /** Store Shared Data and Update plots **/
    data->fs = m_AudioEngine->getFs();
    data->length_fft = length_fft;
    data->observationTime = 2.0;
    data->idx_begin = data->observationTime * data->fs - 1.0 * data->length_fft ;
    data->t_begin = 1.0 * data->idx_begin /  data->fs;
    data->data_length = data->fs * data->observationTime;
    data->length_fft_spectrogram = 512;
    //!--------------------------------------------------------------------


    //![] MainWindow General Settings
    //! ------------------------------
    /* Set Window Title */
    this->setWindowTitle(QString("FReTiAA (v%1.%2.%3), A Free Real Time Audio Analyzer").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));
    this->setWindowIcon(QIcon(":/icons/iconFretiaa.png"));
    //this->setWindowIcon();
    //!--------------------------------------------------------------------


    //![] MenuBar
    //! ----------
    /* "File" menu */
    QMenu *menuFile= menuBar()->addMenu(tr("&File"));

    QMenu *menuExport = new QMenu("Export Data");
    QAction *actionExportOsc = new QAction("Export Oscillogram");
    QAction *actionExportSpectro = new QAction("Export Spectrogram");
    menuExport->addAction(actionExportOsc);
    menuExport->addAction(actionExportSpectro);
    menuFile->addMenu(menuExport);

    QAction *actionClose = new QAction("&Close",this);
    menuFile->addAction(actionClose);

    /* "Options" menu */
    QMenu *menuOptions= menuBar()->addMenu(tr("&Options"));
    QAction *actionAudioInputConfig = new QAction("&Input config",this);
    QAction *actionGainConfig = new QAction("&Gain config",this);
    menuOptions->addAction(actionAudioInputConfig);
    menuOptions->addAction(actionGainConfig);
    /* "About" menu */
    QMenu *menuHelp= menuBar()->addMenu(tr("&Help"));
    QAction *action_about = new QAction("About FReTiAA");
    menuHelp->addAction(action_about);


    connect(actionClose,SIGNAL(triggered()),this,SLOT(exitApp()));
    connect(actionAudioInputConfig, SIGNAL(triggered()), this, SLOT(openAudioConfigDialog()));
    connect(actionGainConfig, SIGNAL(triggered()), this, SLOT(openGainConfigDialog()));
    connect(action_about, SIGNAL(triggered()), this, SLOT(openAboutDialog()));

    //!--------------------------------------------------------------------


    //![] StatusBar
    //! ------------
    QStatusBar *bar_StatusBar = statusBar();
    lab_SoundCardInfo = new ClickableLabel;
    lab_status = new QLabel;
    lab_GainInfo = new ClickableLabel;
    lab_status->setFrameStyle(QFrame::Panel + QFrame::Sunken);
    lab_SoundCardInfo->setFrameStyle(QFrame::Panel + QFrame::Sunken);
    lab_GainInfo->setFrameStyle(QFrame::Panel + QFrame::Sunken);
    bar_StatusBar->addWidget(lab_status);
    bar_StatusBar->addWidget(lab_SoundCardInfo);
    bar_StatusBar->addWidget(lab_GainInfo);

    connect(lab_SoundCardInfo, SIGNAL(clicked()),this,SLOT(openAudioConfigDialog()));
    connect(lab_GainInfo, SIGNAL(clicked()),this,SLOT(openGainConfigDialog()));

    updateStatusBar();
    //!--------------------------------------------------------------------


    //![] ToolBar
    //! ----------
    QToolBar *bar_toolbar = new QToolBar("Tools Bar");
    this->addToolBar(Qt::LeftToolBarArea,bar_toolbar);
    bar_toolbar->setIconSize(QSize(40,40));
    liveView = new QAction;
    liveView->setIcon(QIcon(":/icons/liveViewOnColoredOff.png"));
    liveView->setText("LiveView");
    liveView->setCheckable(true);
    liveView->setChecked(true);

    plotSpectro = new QAction;
    plotSpectro->setText("Spectrogram");
    plotSpectro->setIcon(QIcon(":/icons/iconSpectro.png"));
    plotSpectro->setCheckable(true);
    plotSpectro->setChecked(true);

    plotSpectrum = new QAction;
    plotSpectrum->setText("Spectrum");
    plotSpectrum->setIcon(QIcon(":/icons/iconFFT.png"));
    plotSpectrum->setCheckable(true);
    plotSpectrum->setChecked(false);

    plotOctaveSpectrum = new QAction;
    plotOctaveSpectrum->setText("Octave Spectrum");
    plotOctaveSpectrum->setIcon(QIcon(":/icons/iconOctave.png"));
    plotOctaveSpectrum->setCheckable(true);
    plotOctaveSpectrum->setChecked(false);


    interactionTrigger = new QAction;
    interactionTrigger->setText("Trigger");
    interactionTrigger->setIcon(QIcon(":/icons/iconTrigger.png"));
    interactionTrigger->setCheckable(true);
    interactionTrigger->setChecked(false);

    QComboBox *cbxPlotAsVolt = new QComboBox;
    cbxPlotAsVolt->addItem("Pascal");
    cbxPlotAsVolt->addItem("Volt");

    bar_toolbar->addAction(liveView);
    bar_toolbar->addAction(interactionTrigger);
    bar_toolbar->addWidget(cbxPlotAsVolt);
    bar_toolbar->addSeparator();
    bar_toolbar->addAction(plotSpectro);
    bar_toolbar->addAction(plotSpectrum);
    bar_toolbar->addAction(plotOctaveSpectrum);

    interactionMoveAnalyseRect = new QAction;
    interactionMoveAnalyseRect->setText("Moce Selection Rectangle");
    interactionMoveAnalyseRect->setIcon(QIcon(":/icons/iconMoceRect.png"));
    interactionMoveAnalyseRect->setCheckable(true);
    interactionMoveAnalyseRect->setChecked(false);

    interactionHP = new QAction;
    interactionHP->setText("20Hz  HP filter");
    interactionHP->setIcon(QIcon(":/icons/iconHP.png"));
    interactionHP->setCheckable(true);
    interactionHP->setChecked(false);

    bar_toolbar->addSeparator();
    bar_toolbar->addAction(interactionMoveAnalyseRect);
    bar_toolbar->addSeparator();
    bar_toolbar->addAction(interactionHP);

    HP_filter = new Biquad(bq_type_highpass, 20 / data->fs, 0.707, 0);

    connect(liveView,SIGNAL(toggled(bool)), this, SLOT(liveViewChanged(bool)));
    connect(interactionTrigger,SIGNAL(toggled(bool)), this, SLOT(onTriggerChanged(bool)));
    connect(plotSpectro, SIGNAL(toggled(bool)), this, SLOT(plotSpectogramButtonChanged(bool)));
    connect(cbxPlotAsVolt,SIGNAL(currentIndexChanged(int)),this,SLOT(plotAsChanged(int)));
    connect(plotSpectrum, SIGNAL(toggled(bool)), this, SLOT(plotSpectrumChanged(bool)));
    connect(plotOctaveSpectrum, SIGNAL(toggled(bool)), this, SLOT(plotOctaveSpectrumChanged(bool)));
    connect(interactionMoveAnalyseRect,SIGNAL(toggled(bool)),this,SLOT(onMoveAnalyseRect(bool)));
    //!--------------------------------------------------------------------


    //![] Oscilloscope
    //! --------------
    cPlotOscillogram = new Oscillogram(data);
    data->qPlotOscillogram = cPlotOscillogram->cPlot;
    cPlotOscillogram->updateRect();
    connect(actionExportOsc,SIGNAL(triggered()), cPlotOscillogram,SLOT(onExportData()));

    //!--------------------------------------------------------------------


    //![] Spectrogram
    //! -------------
    cPlotSpectrogram = new Spectrogram(data);
    data->qPlotSpectrogram = cPlotSpectrogram->cPlot;
    connect(actionExportSpectro,SIGNAL(triggered()), cPlotSpectrogram,SLOT(onExportData()));
    //!--------------------------------------------------------------------


    //![] Spectrum
    //! -----------
    cPlotSpectrum = new Spectrum(data);
    connect(cPlotSpectrum,SIGNAL(spectrumIsClosing()),this, SLOT(spectrumIsClosingCatch()));
    //!--------------------------------------------------------------------


    //![] LevelMeter
    m_LevelMeter = new LevelMeter;
    //!--------------------------------------------------------------------


    //![] Octave Spectrum
    //! -----------------
    cPlotOctaveSpectrum = new OctaveSpectrum(data);
    connect(cPlotOctaveSpectrum,SIGNAL(octaveIsClosing()),this, SLOT(octaveIsClosingCatch()));
    //!--------------------------------------------------------------------


    //![] Multimeter
    //! -------------
    QFrame *fMultimeter = new QFrame;
    QGridLayout *lMultimeter = new QGridLayout;
    lMultimeter->setAlignment(Qt::AlignTop);

    fMultimeter->setLayout(lMultimeter);
    fMultimeter->setMaximumWidth(150);

    /* Separation Line */
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    QFrame *line2  = new QFrame();
    line2->setFrameShape(QFrame::HLine);
    line2->setFrameShadow(QFrame::Sunken);
    QFrame *line3  = new QFrame();
    line3->setFrameShape(QFrame::HLine);
    line3->setFrameShadow(QFrame::Sunken);

    /* MultiMeter */
    multiMeterOsc = new MultiMeter;
    multiMeterSpec = new MultiMeter;
    m_dBMeter = new dBMeter;
    lMultimeter->addWidget(line3,0,0,1,3);
    lMultimeter->addWidget(multiMeterOsc,1,0,1,3);
    lMultimeter->addWidget(line,2,0,1,3);
    lMultimeter->addWidget(m_dBMeter,3,0,1,3);
    lMultimeter->addWidget(line2,4,0,1,3);

    connect(m_dBMeter,SIGNAL(dBWeightingChanged()),this, SLOT(updatedBMeter()));
    //!--------------------------------------------------------------------


    //![] MainLayout
    //! ------------
    QWidget *c = new QWidget(this);

    c->setLayout(centralLayout);
    this->setCentralWidget(c);

    centralLayout->addWidget(fMultimeter,0,0,1,1);
    centralLayout->addWidget(m_LevelMeter,0,2);
    centralLayout->addWidget(multiMeterSpec,1,0,1,1);

    //TODO Replace following by a DCPGridLayout to use MarginGroup an align axesRect
    centralLayout->addWidget(cPlotOscillogram->m_qcfgraph,0,1);
    centralLayout->addWidget(cPlotSpectrogram->m_qcfgraph,1,1,1,2);
    //!--------------------------------------------------------------------


    //![] AudioEngine
    //! --------------------
    m_AudioEngine->m_InputDevice = m_AudioEngine->m_AudioInput->start();
    connect(m_AudioEngine->m_InputDevice, SIGNAL(readyRead()), this, SLOT(data2Read()));
    //!--------------------------------------------------------------------




    //![] TODO CHANGED THIS !!!!!!!!!!
    //! --------------------
    connect(this,SIGNAL(dataAvalaible()),this,SLOT(updateOscData()));
    connect(this,SIGNAL(dataAvalaible()),this, SLOT(updateMultimeterOsc()));
    connect(this,SIGNAL(dataAvalaible()),this, SLOT(updateLevelMeter()));
    connect(this,SIGNAL(dataAvalaible()),this, SLOT(updatedBMeter()));
    connect(this,SIGNAL(dataAvalaible()),this, SLOT(updateSpectrumData()));
    connect(this,SIGNAL(dataAvalaible()),this, SLOT(updateOctaveSpectrumData()));
    connect(this,SIGNAL(dataAvalaible()),this, SLOT(updateSpectrogram()));

    connect(cPlotOscillogram,SIGNAL(update()),this, SLOT(updateMultimeterOsc()));
    connect(cPlotOscillogram,SIGNAL(update()),this, SLOT(updateLevelMeter()));
    connect(cPlotOscillogram,SIGNAL(update()),this, SLOT(updatedBMeter()));
    //!--------------------------------------------------------------------

    //[DEBUG] sUncomment to see the Widget contour in the main windows
    //setStyleSheet("QWidget {" "border: 1px solid black;" "color: red" "}"); return
}

MainWindow::~MainWindow(){}

void MainWindow::exitApp()
{
    this->close();
    cPlotSpectrum->close();
    cPlotOctaveSpectrum->close();
}


//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//!           METHODS
//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-

void MainWindow::closeEvent(QCloseEvent *){
    this->exitApp();
}

void MainWindow::updateStatusBar()
{
    if (isLiveView)
        lab_status->setText("Running ...");
    else
        lab_status->setText("Stopped");

    QString text=QString("Device info : %1 @ %2 Hz @ %3 bits").arg(m_AudioEngine->inputDeviceInfo.deviceName()).arg(m_AudioEngine->formatAudio.sampleRate()).arg( m_AudioEngine->formatAudio.sampleSize());
    lab_SoundCardInfo->setText(text);

    QString text2=QString("G1 = %1 %2, G2 = %3 %4, Sensitivity = %5 %6").arg(g1_Value).arg(g1_Unit).arg(g2_Value).arg(g2_Unit).arg(gSensi_Value).arg(gSensi_Unit);
    lab_GainInfo->setText(text2);
}


void MainWindow::updateGain()
{
    // A VERIFIER !!!

    if (!g1_Unit.compare("dB"))
        g1_lin = qPow(10,g1_Value/20);
    else if (!g1_Unit.compare("dBu"))
        g1_lin = qPow(10,(g1_Value/0.707)/20);
    else if (!g1_Unit.compare("dBV"))
        g1_lin = qPow(10,g1_Value/20);

    if (!g2_Unit.compare("dB"))
        g2_lin = qPow(10,g2_Value/20);
    else if (!g2_Unit.compare("dBu"))
        g2_lin = qPow(10,(g2_Value/0.707)/20);
    else if (!g2_Unit.compare("dBV"))
        g2_lin = qPow(10,g2_Value/20);

    if (!gSensi_Unit.compare("V/Pa"))
        sensitivity = gSensi_Value;
    else if (!gSensi_Unit.compare("mV/Pa"))
        sensitivity = gSensi_Value *1000.0;
}

//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//!           SLOTS, Dialog Boxes
//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
void MainWindow::openAboutDialog(){
    new AboutDialog;
}


void MainWindow::openAudioConfigDialog(){

    AudioInputConfigDial *m_AudioInputConfigDial = new AudioInputConfigDial(m_AudioEngine);

    if (m_AudioInputConfigDial->result() == QDialogButtonBox::Apply)
    {
        QMessageBox messageBox;
        // 1 - Test if format is supported
        if (!m_AudioInputConfigDial->deviceInfo.isNull())
        {
            if (! m_AudioInputConfigDial->deviceInfo.isFormatSupported(m_AudioInputConfigDial->settings))
            {
                QAudioFormat nearest = m_AudioInputConfigDial->deviceInfo.nearestFormat(m_AudioInputConfigDial->settings);
                messageBox.critical(0,"Error",QString("Unsupported configuration! Try the Following : sampleRate = %1 Hz ; sampleSize = %2 bits; channelCount = %3; codec = %4").arg(nearest.sampleRate()).arg(nearest.sampleSize()).arg(nearest.channelCount()).arg(nearest.codec()));
                // TODO Auto Set nearest Config and diplsay it in red
            }
        }
        else
            messageBox.critical(0,"Error", "No Device");
    }
    delete m_AudioInputConfigDial;
}

void MainWindow::openGainConfigDialog(){
    GainConfigDial * m_GainConfigDial = new GainConfigDial(g1_Value, g1_Unit, g2_Value,  g2_Unit,  gSensi_Value,  gSensi_Unit);

    if (m_GainConfigDial->result() == QDialog::Accepted)
    {
        g1_Value = m_GainConfigDial->ed_G1->text().toFloat();
        g2_Value = m_GainConfigDial->ed_G2->text().toFloat();
        gSensi_Value = m_GainConfigDial->ed_GSensi->text().toFloat();
        g1_Unit = m_GainConfigDial->cbx_G1->currentText();
        g2_Unit = m_GainConfigDial->cbx_G2->currentText();
        gSensi_Unit = m_GainConfigDial->cbx_GSensi->currentText();
    }

    updateStatusBar();
    updateGain();
    delete m_GainConfigDial;
}

//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//!           SLOTS, Audio Engine
//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
void MainWindow::data2Read(void)
{
    //![] Read Buffer
    //!---------------
    qint64 range = round(data->observationTime * m_AudioEngine->getFs()); // NEED TO BE GREATER THAN BUFFER SIZE

    if (range < m_AudioEngine->soundBufferSize)
    {        qDebug("ERROR range");
        return;
    }
    QVector<double> oldPoints_x = points_x; //osc->m_series->points();
    QVector<double> oldPoints_y = points_y; //osc->m_series->points();
    points_x.clear();
    points_y.clear();

    int resolution = 2; // 1: for 8bits and 2 for int

    // Fill the vector to Draw which is greater or equal to the input buffer size (i.e. range >= SOUND_BUFFER_SIZE)
    qint64 maxSize = m_AudioEngine->m_InputDevice->read((char*)dataSound, m_AudioEngine->soundBufferSize);
    dataCounter += maxSize/resolution;
    qint16 *ptr = reinterpret_cast<qint16*>(dataSound);
    // qDebug("Available Data : %d", maxSize/resolution);

    /** Soit je copie tous soit je décale le buffer du graph pour le remplir par la lecture **/
    if (oldPoints_y.count() < (range)) { // Ici pour remplir au fur et a mesure
        points_x = oldPoints_x;//osc->m_series->points();
        points_y = oldPoints_y;//osc->m_series->points();

    } else { // ici en fonctionnement nominal et décaalge
        for (int i = maxSize/resolution; i < oldPoints_y.count(); i++){
            points_x.append((i - maxSize/resolution)/m_AudioEngine->getFs());
            points_y.append( oldPoints_y.at(i));
        }
    }

    // Remplir par laz lecture
    qint64 size = points_y.count();
    for (int k = 0; k < maxSize/resolution; k++){
        if ((this->interpretAsVolt))
        {
            points_x.append((k+size)/m_AudioEngine->getFs());
            if (interactionHP->isChecked())
                points_y.append(HP_filter->process(g1_lin * g2_lin * ptr[k]/(pow(2,8*resolution)/2)));
            else
                points_y.append(g1_lin * g2_lin * ptr[k]/(pow(2,8*resolution)/2));

        }
        else
        {
            points_x.append((k+size)/m_AudioEngine->getFs());
            if (interactionHP->isChecked())
                points_y.append( HP_filter->process(g1_lin * g2_lin * ptr[k]/(pow(2,8*resolution)/2)/this->sensitivity));
            else
                points_y.append(g1_lin * g2_lin * ptr[k]/(pow(2,8*resolution)/2)/this->sensitivity);
        }

    }

    if ( qMax(length_fft,data->length_fft_spectrogram)> points_y.size()) // TOTO: The right rules is this on AND  to skeep only if less than "length_fft" points are read
        return;
    else
        emit dataAvalaible();

    /* Wait until data are in analysis rectangle before send to plotters(spectrogrum, dbmeter, etc...) */
    if ( data->idx_begin > (points_y.size()- data->length_fft) )
    {;}
    else
    {;//Make connections to all plotters}
    }
}



//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//!           SLOTS, ToolBar
//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
/*
void liveViewChanged(bool);
void plotSpectogramButtonChanged(bool);
void plotAsChanged(int);
void plotSpectrumChanged(bool);
void plotOctaveSpectrumChanged(bool);
void spectrumIsClosingCatch(); // To unchecked button in toolbar
void octaveIsClosingCatch();// To unchecked button in toolbar
void onTriggerChanged(bool);

*/

void MainWindow::liveViewChanged(bool isChecked)
{
    isLiveView = isChecked;

    data->isLiveView = isLiveView;
    if (isLiveView){
        //connect(m_AudioEngine->m_InputDevice, SIGNAL(readyRead()), this, SLOT(data2Read())); // Not really necessary
        m_AudioEngine->m_AudioInput->resume();
        liveView->setIcon(QIcon(":/icons/liveViewOnColoredOff.png"));
        interactionHP->setEnabled(true);
    }
    else{
        // disconnect(m_AudioEngine->m_InputDevice, SIGNAL(readyRead()), this, SLOT(data2Read())); // Not really necessary
        m_AudioEngine->m_AudioInput->suspend();
        liveView->setIcon(QIcon(":/icons/liveViewOnColoredOn.png"));
        interactionHP->setEnabled(false);
    }

    updateStatusBar();
}

void MainWindow::plotSpectogramButtonChanged(bool isChecked)
{
    if (isChecked)
    {
        cPlotSpectrogram = new Spectrogram(data);
        //  cPlotSpectrogram->addGraph();
        //centralLayout->addWidget(cPlotSpectrogram->graphViewer(),1,1,1,2);
        centralLayout->addWidget(cPlotSpectrogram->m_qcfgraph,1,1,1,2);
        multiMeterSpec = new MultiMeter;
        centralLayout->addWidget(multiMeterSpec,1,0,1,1);
        connect(this,SIGNAL(dataAvalaible()),this, SLOT(updateSpectrogram()));
    }
    else {
        /* Exemple to delete spectrogram */
        disconnect(this,SIGNAL(dataAvalaible()),this, SLOT(updateSpectrogram()));


        QLayoutItem *item = centralLayout->itemAtPosition(1,1);
        //centralLayout->removeWidget(cPlotSpectrogram->graphViewer());
        centralLayout->removeWidget(cPlotSpectrogram->m_qcfgraph);
        centralLayout->removeItem(item);
        //        delete cPlotSpectrogram->graphViewer();
        delete cPlotSpectrogram->m_qcfgraph;
        delete cPlotSpectrogram;

        centralLayout->removeWidget(multiMeterSpec);
        QLayoutItem *item2 = centralLayout->itemAtPosition(1,0);
        centralLayout->removeItem(item2);
        delete multiMeterSpec;
    }
}

void MainWindow::plotAsChanged(int i)
{
    interpretAsVolt = bool(i);
    if (interpretAsVolt)
        cPlotOscillogram->setYlabel("Magnitude (V)");
    else
        cPlotOscillogram->setYlabel("Pressure (Pa)");

    //fixme : if live view is of and intepret changed redraw all data
    if (!isLiveView)
    {
        for(int i=0; i <points_y.size();i++)
        {
            if (interpretAsVolt) // points.y are currently in Pa
                points_y.replace(i,points_y.at(i) * sensitivity);
            else //points.y are currently in V
                points_y.replace(i,points_y.at(i) / sensitivity);
        }
        updateMultimeterOsc();
        updateOscData();
    }
}

void MainWindow::plotOctaveSpectrumChanged(bool isChecked)
{
    isOctave = isChecked;
    if (isOctave)
    {
        connect(cPlotOscillogram,SIGNAL(update()),this, SLOT(updateOctaveSpectrumData()));
        cPlotOctaveSpectrum->show();
    }else
    {
        disconnect(cPlotOscillogram,SIGNAL(update()),this, SLOT(updateOctaveSpectrumData()));
        cPlotOctaveSpectrum->close();
    }
}

void MainWindow::plotSpectrumChanged(bool isChecked)
{
    isSpectrum = isChecked;
    if (isChecked){
        connect(cPlotOscillogram,SIGNAL(update()),this, SLOT(updateSpectrumData()));
        cPlotSpectrum->show();
    }
    else{
        disconnect(cPlotOscillogram,SIGNAL(update()),this, SLOT(updateSpectrumData()));
        cPlotSpectrum->close();
    }
}

void MainWindow::onTriggerChanged(bool isChecked)
{
    data->isTrigger = isChecked;

    if (isChecked)
    {
        connect(this,SIGNAL(dataAvalaible()),this, SLOT(updateTriggered()));
        //connect(cPlotOscillogram->graphViewer(),SIGNAL(mouseMove(QMouseEvent*)), cPlotOscillogram, SLOT(onMouseMove(QMouseEvent*)));
        connect(cPlotOscillogram->cPlot,SIGNAL(mouseMove(QMouseEvent*)), cPlotOscillogram, SLOT(onMouseMove(QMouseEvent*)));
        cPlotOscillogram->triggerLine->setVisible(true);
    }else
    {
        disconnect(this,SIGNAL(dataAvalaible()),this, SLOT(updateTriggered()));
        disconnect(cPlotOscillogram->cPlot,SIGNAL(mouseMove(QMouseEvent*)), cPlotOscillogram, SLOT(onMouseMove(QMouseEvent*)));
        cPlotOscillogram->triggerLine->setVisible(false);
    }
}

void MainWindow::octaveIsClosingCatch()
{
    plotOctaveSpectrum->setChecked(false);
}

void MainWindow::spectrumIsClosingCatch()
{
    plotSpectrum->setChecked(false);
}


//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//!           SLOTS, Oscilloscope interaction
//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-


void MainWindow::onMoveAnalyseRect(bool checked)
{
    interactionTrigger->setChecked(false);

    data->isRectMove = checked;
    if (checked)
    {
        //interactionRectZoomAction->setChecked(false);
        //interactionDragAxisAction->setChecked(false);
        //interactionMouseWheelZoomAction->setChecked(false);
        connect(cPlotOscillogram->cPlot,SIGNAL(mouseMove(QMouseEvent*)), cPlotOscillogram, SLOT(onMouseMove(QMouseEvent*)));
    }
    else
        disconnect(cPlotOscillogram->cPlot,SIGNAL(mouseMove(QMouseEvent*)), cPlotOscillogram, SLOT(onMouseMove(QMouseEvent*)));
}



void MainWindow::updateTriggered()
{
    interactionMoveAnalyseRect->setChecked(false);
    interactionTrigger->setChecked(true);
    if ( data->idx_begin > (points_y.size()- data->length_fft) )
        return;

    for (int i=  data->idx_begin ; i< data->idx_begin + length_fft/2 ; i++)
        if (points_y.at(i ) >= cPlotOscillogram->triggerLevel)
        {
            liveView->setChecked(false);
            interactionTrigger->setChecked(false);
        }
}

//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//!           SLOTS, UPDATES TO REPLACE WITHOUT SIGNAL/SLOT
//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
/// EOF

void MainWindow::updateLevelMeter()
{
    if(interpretAsVolt)
        m_LevelMeter->levelMeterChanged(multiMeterOsc->getMaxValueInV());
    else
        m_LevelMeter->levelMeterChanged(multiMeterOsc->getMaxValueInPa());
}


// TODO : The followings can directly be called as a SLOT in the correspondong Class
void MainWindow::updateMultimeterOsc()
{
    if ( data->idx_begin > (points_y.size()- data->length_fft) )
        return;

    multiMeterOsc->setData(points_y, sensitivity,data->idx_begin ,data->length_fft,interpretAsVolt);
}

void MainWindow::updatedBMeter()
{
    if ( data->idx_begin > (points_y.size()- data->length_fft) )
        return;

    m_dBMeter->setData(points_y, data->fs,data->idx_begin ,data->length_fft);
}

void MainWindow::updateOscData()
{
    cPlotOscillogram->updateData(points_x,points_y);
}

void MainWindow::updateSpectrumData()
{
    if ( data->idx_begin > (points_y.size()- data->length_fft) )
        return;

    if (isSpectrum)
        cPlotSpectrum->setData(points_y, data->idx_begin);
}

void MainWindow::updateOctaveSpectrumData()
{
    if ( data->idx_begin > (points_y.size()- data->length_fft) )
        return;

    if (isOctave)
        cPlotOctaveSpectrum->setData(points_y, data->idx_begin);

}

void MainWindow::updateSpectrogram(){
    cPlotSpectrogram->update(points_y);
}














