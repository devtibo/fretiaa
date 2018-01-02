#include "mainwindow.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QDebug>

#include "audioinputconfigdial.h"
#include "gainconfigdial.h"
#include "aboutdialog.h"
#include "qfgraph.h"



unsigned int roundUpToNextPowerOfTwo(unsigned int x)
{
    x--;
    x |= x >> 1;  // handle  2 bit numbers
    x |= x >> 2;  // handle  4 bit numbers
    x |= x >> 4;  // handle  8 bit numbers
    x |= x >> 8;  // handle 16 bit numbers
    x |= x >> 16; // handle 32 bit numbers
    x++;

    return x;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //![] Init AudioEngine
    m_AudioEngine = new AudioEngine();// default input
    m_AudioEngine->soundBufferSize = 1024;
    data->m_AudioEngine=m_AudioEngine;



    sensitivity = 1; // V/Pa
    g1_lin=1; g2_lin=1;

    length_fft = roundUpToNextPowerOfTwo(0.10*m_AudioEngine->getFs());
    g1_Value=0; g2_Value=0;;
    g1_Unit="dB"; g2_Unit="dB"; gSensi_Unit="V/Pa";
    /** Store Shared Data and Update plots **/
    data->fs = m_AudioEngine->getFs();
    data->rectAnalysisLength = length_fft;
    data->rectAnalysisDuration = length_fft /  data->fs;
    data->observationTime = 10;
    data->idx_begin = data->observationTime * data->fs - 1.0 * data->rectAnalysisLength ;
    data->t_begin = 1.0 * data->idx_begin /  data->fs;
    data->data_length = data->fs * data->observationTime;
    data->length_fft_spectrogram = 512;

    data->g1_lin = g1_lin;
    data->g2_lin = g2_lin;
    data->sensitivity = sensitivity;

    //!--------------------------------------------------------------------


    //![] MainWindow General Settings
    //! ------------------------------
    /* Set Window Title */
    this->setWindowTitle(QString("FReTiAA (v%1.%2.%3), A Free Real Time Audio Analyzer").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));
    this->setWindowIcon(QIcon(":/icons/iconFretiaa.png"));
    //this->setWindowIcon();
    //!--------------------------------------------------------------------



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
    liveView->setText("LiveView [SPACE BAR]");
    liveView->setCheckable(true);
    liveView->setChecked(true);

    plotSpectro = new QAction;
    plotSpectro->setText("Spectrogram [G]");
    plotSpectro->setIcon(QIcon(":/icons/iconSpectro.png"));
    plotSpectro->setCheckable(true);
    plotSpectro->setChecked(true);


    plotSpectrum = new QAction;
    plotSpectrum->setText("Spectrum [S]");
    plotSpectrum->setIcon(QIcon(":/icons/iconFFT.png"));
    plotSpectrum->setCheckable(true);
    plotSpectrum->setChecked(false);

    plotOctaveSpectrum = new QAction;
    plotOctaveSpectrum->setText("Octave Spectrum [O]");
    plotOctaveSpectrum->setIcon(QIcon(":/icons/iconOctave.png"));
    plotOctaveSpectrum->setCheckable(true);
    plotOctaveSpectrum->setChecked(false);


    interactionTrigger = new QAction;
    interactionTrigger->setText("Trigger [T]");
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
    interactionMoveAnalyseRect->setText("Moce Selection Rectangle [R]");
    interactionMoveAnalyseRect->setIcon(QIcon(":/icons/iconMoceRect.png"));
    interactionMoveAnalyseRect->setCheckable(true);
    interactionMoveAnalyseRect->setChecked(false);

    interactionHP = new QAction;
    interactionHP->setText("20Hz  HP filter [H]");
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


    // cPlotOscillogram->setCursor(Qt::SizeVerCursor);
    //!--------------------------------------------------------------------


    //![] Spectrogram
    //! -------------
    cPlotSpectrogram = new Spectrogram(data);
    data->qPlotSpectrogram = cPlotSpectrogram->cPlot;

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
    centralLayout->addWidget(cPlotOscillogram->cPlot,0,1);
    centralLayout->addWidget(cPlotSpectrogram->m_qcfgraph,1,1,1,2);
    //!--------------------------------------------------------------------


    //![] AudioEngine
    //! --------------------
    m_AudioEngine->m_InputDevice = m_AudioEngine->m_AudioInput->start();
    //    connect(m_AudioEngine->m_InputDevice, SIGNAL(readyRead()), this, SLOT(data2Read()));
    //!--------------------------------------------------------------------




    //![] SHORTCUTS
    //! -----------
    QShortcut *sCSpace = new QShortcut(this);
    sCSpace->setKey( Qt::Key_Space);
    connect(sCSpace, SIGNAL(activated()), this, SLOT(onKeySpacePress()));

    QShortcut *sCKeyT = new QShortcut(this);
    sCKeyT->setKey( Qt::Key_T);
    connect(sCKeyT, SIGNAL(activated()), this, SLOT(onKeyTPress()));

    QShortcut *sCKeyR = new QShortcut(this);
    sCKeyR->setKey( Qt::Key_R);
    connect(sCKeyR, SIGNAL(activated()), this, SLOT(onKeyRPress()));

    QShortcut *sCKeyS = new QShortcut(this);
    sCKeyS->setKey( Qt::Key_S);
    connect(sCKeyS, SIGNAL(activated()), this, SLOT(onKeySPress()));

    QShortcut *sCKeyO = new QShortcut(this);
    sCKeyO->setKey( Qt::Key_O);
    connect(sCKeyO, SIGNAL(activated()), this, SLOT(onKeyOPress()));

    QShortcut *sCKeyH = new QShortcut(this);
    sCKeyH->setKey( Qt::Key_H);
    connect(sCKeyH, SIGNAL(activated()), this, SLOT(onKeyHPress()));

    QShortcut *sCKeyG = new QShortcut(this);
    sCKeyG->setKey( Qt::Key_G);
    connect(sCKeyG, SIGNAL(activated()), this, SLOT(onKeyGPress()));


    //![] THREAD !!!
    //! -------------

    mAudioThread = new InputAudioReadThread(data);
    connect(m_AudioEngine->m_InputDevice, SIGNAL(readyRead()), mAudioThread, SLOT(start()));

    //![] TODO CHANGED THIS !!!!!!!!!!
    //! --------------------


    connect(mAudioThread,SIGNAL(finished()),this,SLOT(updateOscData()));

/*
    connect(cPlotOscillogram,SIGNAL(rectDataAvailable()),this, SLOT(updateMultimeterOsc())); // Update widget only when data are read from the reddctangle


    connect(cPlotOscillogram,SIGNAL(rectDataAvailable()),this, SLOT(updateLevelMeter()));


    connect(cPlotOscillogram,SIGNAL(rectDataAvailable()),this, SLOT(updatedBMeter()));


    //connect(mAudioThread,SIGNAL(finished()),this, SLOT(updateSpectrogram()));

    //!--------------------------------------------------------------------

    //[DEBUG] sUncomment to see the Widget contour in the main windows
    //setStyleSheet("QWidget {" "border: 1px solid black;" "color: red" "}"); return

    /*Create Menus*/
    createFileMenu();
    createViewMenu();
    createToolsMenu();
    createOptionsMenu();
    createHelpMenu();


    plotSpectogramButtonChanged(false);
    data->isSpectrogramShow=false;



}

MainWindow::~MainWindow(){}

void MainWindow::createFileMenu()
{
    /* "File" menu */
    QMenu *fileMenu= menuBar()->addMenu(tr("&File"));

    QMenu *exportSubMenu = new QMenu("Export Data");
    QAction *exportOscillogramDataAction = new QAction("Export Oscillogram");
    QAction *exportSpectrogramDataAction = new QAction("Export Spectrogram");
    exportSubMenu->addAction(exportOscillogramDataAction);
    exportSubMenu->addAction(exportSpectrogramDataAction);
    fileMenu->addMenu(exportSubMenu);

    QAction *closeAction = new QAction("&Close",this);
    fileMenu->addAction(closeAction);

    connect(closeAction,SIGNAL(triggered()),this,SLOT(exitApp()));
    connect(exportOscillogramDataAction,SIGNAL(triggered()), cPlotOscillogram,SLOT(onExportData()));
    connect(exportSpectrogramDataAction,SIGNAL(triggered()), cPlotSpectrogram,SLOT(onExportData()));
}

void MainWindow::createViewMenu()
{
    /* "File" menu */
    QMenu *viewMenu= menuBar()->addMenu(tr("&View"));
    QAction *viewSpectrumAction = new QAction(tr("&Spectrum"));
    viewSpectrumAction->setCheckable(true);
    QAction *viewSpectrogramAction = new QAction(tr("&Spectrogram"));
    viewSpectrogramAction->setCheckable(true);
    QAction *viewOctaveAction = new QAction(tr("&Octave Spectrum"));
    viewOctaveAction->setCheckable(true);

    viewMenu->addAction(viewSpectrumAction);
    viewMenu->addAction(viewSpectrogramAction);
    viewMenu->addAction(viewOctaveAction);
}

void MainWindow::createToolsMenu()
{
    /* "File" menu */
    QMenu *toolsMenu= menuBar()->addMenu(tr("&Tools"));

    QAction *triggerToolsAction = new QAction(tr("&Trigger"));
    triggerToolsAction->setCheckable(false);
    QAction *highPassFilterToolsAction = new QAction(tr("&20Hz High Pass Filter"));
    highPassFilterToolsAction->setCheckable(true);

    toolsMenu->addAction(triggerToolsAction);
    toolsMenu->addAction(highPassFilterToolsAction);
}

void MainWindow::createOptionsMenu()
{
    /* "Options" menu */
    QMenu *optionsMenu= menuBar()->addMenu(tr("&Options"));
    QAction *audioInputConfigAction = new QAction("&Input config",this);
    QAction *gainConfigAction = new QAction("&Gain config",this);

    optionsMenu->addAction(audioInputConfigAction);
    optionsMenu->addAction(gainConfigAction);

    connect(audioInputConfigAction, SIGNAL(triggered()), this, SLOT(openAudioConfigDialog()));
    connect(gainConfigAction, SIGNAL(triggered()), this, SLOT(openGainConfigDialog()));
}

void MainWindow::createHelpMenu()
{
    /* "About" menu */
    QMenu *helpMenu= menuBar()->addMenu(tr("&Help"));
    QAction *aboutAction = new QAction("About FReTiAA");
    helpMenu->addAction(aboutAction);

    connect(aboutAction, SIGNAL(triggered()), this, SLOT(openAboutDialog()));
}

void MainWindow::exitApp()
{
    this->close();
    cPlotSpectrum->close();
    cPlotOctaveSpectrum->mWin->close();
    m_AudioEngine->m_AudioInput->stop();
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
//!           SLOTS, ToolBar
//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
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
    data->isSpectrogramShow = isChecked;
    if (isChecked)
    {
        cPlotSpectrogram = new Spectrogram(data);
        //  cPlotSpectrogram->addGraph();
        //centralLayout->addWidget(cPlotSpectrogram->graphViewer(),1,1,1,2);
        centralLayout->addWidget(cPlotSpectrogram->m_qcfgraph,1,1,1,2);
        multiMeterSpec = new MultiMeter;
        centralLayout->addWidget(multiMeterSpec,1,0,1,1);
        connect(cPlotOscillogram,SIGNAL(rectDataAvailable()),this, SLOT(updateSpectrogram()));
    }
    else {
        /* Exemple to delete spectrogram */
        disconnect(cPlotOscillogram,SIGNAL(rectDataAvailable()),this, SLOT(updateSpectrogram()));

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
    isOctaveView = isChecked;
    if (isOctaveView)
    {
        connect(cPlotOscillogram,SIGNAL(rectDataAvailable()),this, SLOT(updateOctaveSpectrumData()));
        this->show();
        cPlotOctaveSpectrum->mWin->show();
    }else
    {
        disconnect(cPlotOscillogram,SIGNAL(rectDataAvailable()),this, SLOT(updateOctaveSpectrumData()));
        cPlotOctaveSpectrum->mWin->close();
    }
}

void MainWindow::plotSpectrumChanged(bool isChecked)
{
    isSpectrum = isChecked;
    if (isChecked){
        connect(this,SIGNAL(dataAvalaible()),this, SLOT(updateSpectrumData()));
        cPlotSpectrum->show();
    }
    else{
        disconnect(this,SIGNAL(dataAvalaible()),this, SLOT(updateSpectrumData()));
        cPlotSpectrum->close();
    }
}

void MainWindow::onTriggerChanged(bool isChecked)
{
    data->isTrigger = isChecked;

    if (isChecked)
    {
        connect(this,SIGNAL(dataAvalaible()),this, SLOT(updateTriggered()));

        connect(cPlotOscillogram->cPlot,SIGNAL(mouseMove(QMouseEvent*)), cPlotOscillogram, SLOT(onMouseMove(QMouseEvent*)));
        cPlotOscillogram->triggerLine->setVisible(true);
    }else
    {
        disconnect(this,SIGNAL(dataAvalaible()),this, SLOT(updateTriggered()));
        disconnect(cPlotOscillogram->cPlot,SIGNAL(mouseMove(QMouseEvent*)), cPlotOscillogram, SLOT(onMouseMove(QMouseEvent*)));
        cPlotOscillogram->triggerLine->setVisible(false);

        this->setCursor(Qt::ArrowCursor);
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


void MainWindow::onKeySpacePress()
{

    liveView->setChecked(!liveView->isChecked());

}

void MainWindow::onKeyTPress()
{
    interactionTrigger->setChecked(!interactionTrigger->isChecked());
}

void MainWindow::onKeyRPress()
{
    interactionMoveAnalyseRect->setChecked(!interactionMoveAnalyseRect->isChecked());
}


void MainWindow::onKeySPress()
{
    plotSpectrum->setChecked(!plotSpectrum->isChecked());
}

void MainWindow::onKeyOPress()
{
    plotOctaveSpectrum->setChecked(!plotOctaveSpectrum->isChecked());
}

void MainWindow::onKeyHPress()
{
    interactionHP->setChecked(!interactionHP->isChecked());
}


void MainWindow::onKeyGPress()
{
    plotSpectro->setChecked(!plotSpectro->isChecked());
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
    if ( data->idx_begin > (points_y.size()- data->rectAnalysisLength) )
        return;

    //for (int i=  data->idx_begin ; i< data->idx_begin + length_fft ; i++)
    for (int i=  data->idx_begin + length_fft -1  ; i>=data->idx_begin ; i--)
        if (points_y.at(i) >= cPlotOscillogram->triggerLevel)
        {
            m_AudioEngine->m_AudioInput->suspend();
            liveView->setChecked(false);
            interactionTrigger->setChecked(false);
        }
}



//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//!           SLOTS, UPDATES TO REPLACE WITHOUT SIGNAL/SLOT
//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-


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
    //QVector<double> p;
    //p=data->ReadRectData();
    multiMeterOsc->setData(data->ReadRectData(), sensitivity,interpretAsVolt);
}

void MainWindow::updatedBMeter()
{
  //  QVector<double> p;
  //  p=data->ReadRectData();
    m_dBMeter->setData(data->ReadRectData(), data->fs);
}

void MainWindow::updateOscData()
{

    if (~cPlotOscillogram->isRunning())
    {
        cPlotOscillogram->runWithParams(data->ReadPoints_x,data->ReadPoints_y);//Threading
        cPlotOscillogram->wait();
    }
    else
        qDebug("Multiple Call");
}

void MainWindow::updateSpectrumData(){
    cPlotSpectrum->setData(points_y);

}

void MainWindow::updateOctaveSpectrumData(){
    cPlotOctaveSpectrum->runWithParams(data->ReadRectData());//Threading
    //cPlotOctaveSpectrum->setData(points_y); // standard call

}

void MainWindow::updateSpectrogram(){
/*
    QVector<double>  p;
    QCPRange mRange = cPlotOscillogram->cPlot->xAxis->range();

    int idx_begin = data->fs * mRange.lower;
    int idx_end = data->fs * mRange.upper;
    for (int i=idx_begin; i<= idx_end;i++)
    {
        float y = cPlotOscillogram->cPlot->graph()->data().data()->at(i)->value;
        p.append(y);
    }

    cPlotSpectrogram->runWithParams(p); // Threading
    // cPlotSpectrogram->update(p); // Standard call
    */
}
















