#include "mainwindow.h"

#include <QMenuBar>
#include <QStatusBar>
#include <QGridLayout>
#include <QDebug>

#include "audioinputconfigdial.h"
#include "gainconfigdial.h"
#include "aboutdialog.h"
#include "qfgraph.h"
#include "sndfile.h"



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



    /** Store Shared Data and Update plots **/
    data->fs = m_AudioEngine->getFs();
    data->rectAnalysisLength = roundUpToNextPowerOfTwo(0.20*m_AudioEngine->getFs());;
    data->rectAnalysisDuration = data->rectAnalysisLength /  data->fs;
    data->observationTime = 10;
    data->idx_begin = data->observationTime * data->fs - 1.0 * data->rectAnalysisLength ;
    data->t_begin = 1.0 * data->idx_begin /  data->fs;
    data->data_length = data->fs * data->observationTime;
    data->length_fft_spectrogram = roundUpToNextPowerOfTwo(0.02 * m_AudioEngine->getFs());
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
    data->qPlotSpectrogram = cPlotSpectrogram;

    //!--------------------------------------------------------------------


    //![] Spectrum
    //! -----------
    cPlotSpectrum = new Spectrum(data);
    connect(cPlotSpectrum->mWin,SIGNAL(spectrumIsClosing()),this, SLOT(spectrumIsClosingCatch()));
    //!--------------------------------------------------------------------


    //![] LevelMeter
    m_LevelMeter = new LevelMeter(data);
    //!--------------------------------------------------------------------


    //![] Octave Spectrum
    //! -----------------
    cPlotOctaveSpectrum = new OctaveSpectrum(data);
    //connect(cPlotOctaveSpectrum,SIGNAL(octaveIsClosing()),this, SLOT(octaveIsClosingCatch()));
    connect(cPlotOctaveSpectrum->mWin,SIGNAL(octaveIsClosing()),this, SLOT(octaveIsClosingCatch()));
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
    multiMeterOsc = new MultiMeter(data);
    multiMeterSpec = new MultiMeter(data);
    m_dBMeter = new dBMeter(data);
    lMultimeter->addWidget(line3,0,0,1,3);
    lMultimeter->addWidget(multiMeterOsc,1,0,1,3);
    lMultimeter->addWidget(line,2,0,1,3);
    lMultimeter->addWidget(m_dBMeter,3,0,1,3);
    lMultimeter->addWidget(line2,4,0,1,3);

    // connect(m_dBMeter,SIGNAL(dBWeightingChanged()),this, SLOT(updatedBMeter()));
    // connect(m_dBMeter,SIGNAL(dBWeightingChanged()),m_dBMeter, SLOT(updateData()));
    //!--------------------------------------------------------------------


    //![] MainLayout
    //! ------------
    QWidget *c = new QWidget(this);
    c->setLayout(centralLayout);
    this->setCentralWidget(c);

    centralLayout->addWidget(fMultimeter,0,0,1,1);
    centralLayout->addWidget(m_LevelMeter,0,2);
    centralLayout->addWidget(multiMeterSpec,1,0,1,1);

    //cPlotOscillogram->m_qcfgraph->plotLayout()->clear();
    //cPlotSpectrogram->m_qcfgraph->plotLayout()->clear();
    //TODO Replace following by a DCPGridLayout to use MarginGroup an align axesRect
    centralLayout->addWidget(cPlotOscillogram->m_qcfgraph,0,1);
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

    RefreshTimer = new QTimer();
    //RefreshTimer->setInterval(50);
    RefreshTimer->start(50);


    //   connect(mAudioThread, SIGNAL(finished()),cPlotOscillogram, SLOT(start()));

    connect(mAudioThread,SIGNAL(finished()),this,SLOT(connectWidgets()));
    // connect(cPlotOscillogram,SIGNAL(rectDataAvailable()),this,SLOT(connectRectWidgets()));

    //  connect(mAudioThread,SIGNAL(finished()),RefreshTimer,SLOT(start()));
    // connect(mAudioThread,SIGNAL(finished()),this,SLOT(updateOscData()));
    //timer->connect(timer, SIGNAL(timeout()),this, SLOT(updateOscData()));


    /*
    connect(RefreshTimer,SIGNAL(timeout()),this, SLOT(updateMultimeterOsc())); // Update widget only when data are read from the reddctangle


    connect(RefreshTimer,SIGNAL(timeout()),this, SLOT(updateLevelMeter()));


    connect(RefreshTimer,SIGNAL(timeout()),this, SLOT(updatedBMeter()));


    //connect(mAudioThread,SIGNAL(finished()),this, SLOT(updateSpectrogram()));
*/
    //!--------------------------------------------------------------------

    //[DEBUG] sUncomment to see the Widget contour in the main windows
    //setStyleSheet("QWidget {" "border: 1px solid black;" "color: red" "}"); return

    /*Create Menus*/
    createFileMenu();
    createViewMenu();
    createToolsMenu();
    createOptionsMenu();
    createHelpMenu();
    loadWavFileAction->setEnabled(false);

    // plotSpectogramButtonChanged(false);
    // data->isSpectrogramShow=false;
}

void MainWindow::connectRectWidgets()
{
    disconnect(cPlotOscillogram,SIGNAL(rectDataAvailable()),this,SLOT(connectRectWidgets()));

    //connect(RefreshTimer,SIGNAL(timeout()),this, SLOT(updateMultimeterOsc())); // Update widget only when data are read from the reddctangle
    connect(RefreshTimer,SIGNAL(timeout()),multiMeterOsc, SLOT(updateData())); // Update widget only when data are read from the reddctangle
    connect(RefreshTimer,SIGNAL(timeout()),m_LevelMeter, SLOT(updateData()));
    //connect(RefreshTimer,SIGNAL(timeout()),this, SLOT(updatedBMeter()));
    connect(RefreshTimer,SIGNAL(timeout()),m_dBMeter, SLOT(updateData()));



}

void MainWindow::connectWidgets()
{
    disconnect(mAudioThread,SIGNAL(finished()),this,SLOT(connectWidgets()));
    connect(RefreshTimer, SIGNAL(timeout()),cPlotOscillogram, SLOT(start()));

    // connect(mAudioThread, SIGNAL(finished()),cPlotOscillogram, SLOT(start()));

    //connect(RefreshTimer,SIGNAL(timeout()),this, SLOT(updateSpectrogram()));
    connect(mAudioThread,SIGNAL(spectrogramFrameReady()),this,SLOT(updateSpectrogram()));


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

    loadWavFileAction = new QAction("&Load WavFile",this);
    fileMenu->addAction(loadWavFileAction);

    QAction *closeAction = new QAction("&Close",this);
    fileMenu->addAction(closeAction);

    connect(closeAction,SIGNAL(triggered()),this,SLOT(exitApp()));
    connect(exportOscillogramDataAction,SIGNAL(triggered()), cPlotOscillogram,SLOT(onExportData()));
    connect(exportSpectrogramDataAction,SIGNAL(triggered()), cPlotSpectrogram,SLOT(onExportData()));
    connect(loadWavFileAction,SIGNAL(triggered()),this,SLOT(onLoadWavFile()));
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
    cPlotSpectrum->mWin->close();
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

    // QString text=QString("Device info : %1 @ %2 Hz @ %3 bits").arg(m_AudioEngine->inputDeviceInfo.deviceName()).arg(m_AudioEngine->formatAudio.sampleRate()).arg( m_AudioEngine->formatAudio.sampleSize());
    lab_SoundCardInfo->setText(QString("Device info : %1 @ %2 Hz @ %3 bits").arg(m_AudioEngine->inputDeviceInfo.deviceName()).arg(m_AudioEngine->formatAudio.sampleRate()).arg( m_AudioEngine->formatAudio.sampleSize()));

    // QString text2=QString("G1 = %1 %2, G2 = %3 %4, Sensitivity = %5 %6").arg(data->g1_Value).arg(data->g1_Unit).arg(data->g2_Value).arg(data->g2_Unit).arg(data->gSensi_Value).arg(data->gSensi_Unit);
    lab_GainInfo->setText(QString("G1 = %1 %2, G2 = %3 %4, Sensitivity = %5 %6").arg(data->g1_Value).arg(data->g1_Unit).arg(data->g2_Value).arg(data->g2_Unit).arg(data->gSensi_Value).arg(data->gSensi_Unit));
}


//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
//!           SLOTS, Dialog Boxes
//! -*-*-*-*-*-*-*-*-*-*-*-*-*-*-
void MainWindow::openAboutDialog(){
    AboutDialog *mdial = new AboutDialog;
    delete mdial;
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
    GainConfigDial * m_GainConfigDial = new GainConfigDial(data->g1_Value, data->g1_Unit, data->g2_Value,  data->g2_Unit,  data->gSensi_Value, data-> gSensi_Unit);

    if (m_GainConfigDial->result() == QDialog::Accepted)
    {
        data->g1_Value = m_GainConfigDial->ed_G1->text().toFloat();
        data->g2_Value = m_GainConfigDial->ed_G2->text().toFloat();
        data->gSensi_Value = m_GainConfigDial->ed_GSensi->text().toFloat();
        data->g1_Unit = m_GainConfigDial->cbx_G1->currentText();
        data->g2_Unit = m_GainConfigDial->cbx_G2->currentText();
        data->gSensi_Unit = m_GainConfigDial->cbx_GSensi->currentText();
    }

    // A VERIFIER !!!

    if (!data->g1_Unit.compare("dB"))
        data->g1_lin = qPow(10,data->g1_Value/20);
    else if (!data->g1_Unit.compare("dBu"))
        data->g1_lin = qPow(10,(data->g1_Value/0.707)/20);
    else if (!data->g1_Unit.compare("dBV"))
        data->g1_lin = qPow(10,data->g1_Value/20);

    if (!data->g2_Unit.compare("dB"))
        data->g2_lin = qPow(10,data->g2_Value/20);
    else if (!data->g2_Unit.compare("dBu"))
        data->g2_lin = qPow(10,(data->g2_Value/0.707)/20);
    else if (!data->g2_Unit.compare("dBV"))
        data->g2_lin = qPow(10,data->g2_Value/20);

    if (!data->gSensi_Unit.compare("V/Pa"))
        data->gSensi_lin= data->gSensi_Value;
    else if (!data->gSensi_Unit.compare("mV/Pa"))
        data->gSensi_lin = data->gSensi_Value *1000.0;



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

        // Manage with other tools
        loadWavFileAction->setEnabled(false);

        QMessageBox msgBox;
        msgBox.setWindowTitle("Live View Starting ...");
        msgBox.setText("Erase current signal before running live view?");
        msgBox.setStandardButtons(QMessageBox::No);
        msgBox.addButton(QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes){
            this->flushAll();
            // Erase all
        }else {
            // do nothing
        }



        connect(RefreshTimer, SIGNAL(timeout()),cPlotOscillogram, SLOT(start()));
        m_AudioEngine->m_AudioInput->resume();
        liveView->setIcon(QIcon(":/icons/liveViewOnColoredOff.png"));
        interactionHP->setEnabled(true);



    }
    else{

        disconnect(RefreshTimer, SIGNAL(timeout()),cPlotOscillogram, SLOT(start()));
        m_AudioEngine->m_AudioInput->suspend();
        liveView->setIcon(QIcon(":/icons/liveViewOnColoredOn.png"));
        interactionHP->setEnabled(false);

        // Manage with other tools
        loadWavFileAction->setEnabled(true);
    }

    updateStatusBar();
}

void MainWindow::plotSpectogramButtonChanged(bool isChecked)
{
    data->isSpectrogramShow = isChecked;
    if (isChecked)
    {
        cPlotSpectrogram = new Spectrogram(data);
        data->qPlotSpectrogram = cPlotSpectrogram;
        //  cPlotSpectrogram->addGraph();
        //centralLayout->addWidget(cPlotSpectrogram->graphViewer(),1,1,1,2);
        centralLayout->addWidget(cPlotSpectrogram->m_qcfgraph,1,1,1,2);
        multiMeterSpec = new MultiMeter(data);
        centralLayout->addWidget(multiMeterSpec,1,0,1,1);
        //connect(RefreshTimer,SIGNAL(timeout()),this, SLOT(updateSpectrogram()));

    }
    else {
        /* Exemple to delete spectrogram */
        //  disconnect(RefreshTimer,SIGNAL(timeout()),this, SLOT(updateSpectrogram()));

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
    data->interpretAsVolt = bool(i);
    if (data->interpretAsVolt)
        cPlotOscillogram->setYlabel("Magnitude (V)");
    else
        cPlotOscillogram->setYlabel("Pressure (Pa)");

    //fixme : if live view is of and intepret changed redraw all data

    if (!isLiveView)
    {

        QProgressDialog mProgBar;
        mProgBar.setLabelText("Conversion in progress...");

        mProgBar.setRange(0,cPlotOscillogram->cPlot->graph()->dataCount());
        mProgBar.show();
        QVector<double> tmp_y;
        QVector<double> tmp_x;
        for (int i=0; i< cPlotOscillogram->cPlot->graph()->dataCount();i++)
        {
            if(!(i%int(mProgBar.maximum()/20)))
            {
                mProgBar.setValue(i);
                QApplication::processEvents();
            }
            tmp_x.append(cPlotOscillogram->cPlot->graph()->data().data()->at(i)->key);
            if (data->interpretAsVolt) // points.y are currently in Pa
                tmp_y.append(cPlotOscillogram->cPlot->graph()->data().data()->at(i)->value * data->gSensi_lin);
            else
                tmp_y.append(cPlotOscillogram->cPlot->graph()->data().data()->at(i)->value / data->gSensi_lin);
        }

        cPlotOscillogram->cPlot->graph()->setData(tmp_x,tmp_y);
        multiMeterOsc->updateData();
        //updateMultimeterOsc();
        //updateOscData();
    }
}

void MainWindow::plotOctaveSpectrumChanged(bool isChecked)
{
    isOctaveView = isChecked;
    if (isOctaveView)
    {
        connect(cPlotOscillogram,SIGNAL(rectDataAvailable()),cPlotOctaveSpectrum, SLOT(start()));
        cPlotOctaveSpectrum->mWin->show();
    }else
    {
        disconnect(cPlotOscillogram,SIGNAL(rectDataAvailable()),cPlotOctaveSpectrum, SLOT(start()));
        cPlotOctaveSpectrum->mWin->close();
    }
}

void MainWindow::plotSpectrumChanged(bool isChecked)
{
    isSpectrum = isChecked;
    if (isChecked){
        connect(cPlotOscillogram,SIGNAL(rectDataAvailable()),cPlotSpectrum, SLOT(updateData()));
        cPlotSpectrum->mWin->show();
    }
    else{
        disconnect(cPlotOscillogram,SIGNAL(rectDataAvailable()),cPlotSpectrum, SLOT(updateData()));
        cPlotSpectrum->mWin->close();
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
    QVector<double> points_y;
    points_y=data->ReadRectData();
    interactionMoveAnalyseRect->setChecked(false);
    interactionTrigger->setChecked(true);
    if ( data->idx_begin > (points_y.size()- data->rectAnalysisLength) )
        return;

    //for (int i=  data->idx_begin ; i< data->idx_begin + length_fft ; i++)
    for (int i=  data->idx_begin + data->rectAnalysisLength -1  ; i>=data->idx_begin ; i--)
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

void MainWindow::updateSpectrogramOnShot(){

    QVector<double>  p;
    QCPRange mRange = cPlotOscillogram->cPlot->xAxis->range();

    int idx_begin = data->fs * mRange.lower;
    int idx_end = data->fs * mRange.upper;
    for (int i=idx_begin; i<= idx_end;i++)
    {
        p.append(cPlotOscillogram->cPlot->graph()->data().data()->at(i)->value);
    }

    cPlotSpectrogram->runWithParams(p); // Threading
    // cPlotSpectrogram->update(p); // Standard call

}

void MainWindow::updateSpectrogram()
{
    cPlotSpectrogram->start();
}

void MainWindow::flushAll()
{
    m_AudioEngine->m_InputDevice->reset();

    cPlotOscillogram->cPlot->graph(0)->data().clear();
    cPlotOscillogram->cPlot->graph(0)->data().data()->clear();

    cPlotSpectrogram->colorMap->data()->clear();
    cPlotSpectrogram->colorMap->data()->setSize(cPlotSpectrogram->nx, cPlotSpectrogram->ny); // we want the color map to have nx * ny data points
    cPlotSpectrogram->idx=0;
    cPlotSpectrogram->frameCpt = 0;
    cPlotSpectrogram->shiftmode = false;

    mAudioThread->nbreOfSamples = 0;
    mAudioThread->points_y.clear();
    mAudioThread->points_x.clear();
    mAudioThread->frame1.clear();
    mAudioThread->frame2.clear();
    mAudioThread->frame1Leaving.clear();
    mAudioThread->frame2Leaving.clear();
    mAudioThread->framesCpt1=0;
    mAudioThread->framesCpt2=0;
    //cPlotOscillogram->cPlot->graph()->setData(xtmp,ytmp);
    //cPlotSpectrogram->colorMap->data()

    cPlotOctaveSpectrum->cPlot->replot();
    cPlotSpectrogram->cPlot->replot();
}




void MainWindow::onLoadWavFile()
{



    QString fname  = QFileDialog::getOpenFileName(this, "Open file", "./", "Audio File (*.wav)");

    //  return;
    SF_INFO *sfinfo = new SF_INFO;

    SNDFILE* soundfile=sf_open(fname.toStdString().c_str(),   SFM_READ, sfinfo) ;

    double *audioIn = new double[sfinfo->channels * sfinfo->frames];
    sf_read_double(soundfile, audioIn, sfinfo->channels * sfinfo->frames);

    QProgressDialog mProgBar(this);
    mProgBar.setLabelText("Openning in progress...");

    mProgBar.setRange(0,sfinfo->channels * sfinfo->frames-1);
    mProgBar.show();
    mProgBar.setValue(0);
    QApplication::processEvents();
    cPlotOscillogram->cPlot->xAxis->setRange(0,(sfinfo->channels * sfinfo->frames)/sfinfo->samplerate );
    cPlotOscillogram->cPlot->yAxis->setRange(-1,1);

    QVector<double> x;
    QVector<double> y;

    for (int i=0; i < sfinfo->channels * sfinfo->frames; i++)
    {

        if (i==0)
        {
            x.append((i*1.0)/(sfinfo->samplerate*1.0));
            y.append(audioIn[i]);
            cPlotOscillogram->cPlot->graph()->setData(x,y);
        }
        else
            cPlotOscillogram->cPlot->graph()->addData((i*1.0)/(sfinfo->samplerate*1.0),audioIn[i]);



        if(!(i%(10*sfinfo->samplerate)))//Evry 10 seconds chunck
        {
            mProgBar.setValue(i);
            cPlotOscillogram->cPlot->replot();
            QApplication::processEvents();
        }


    }

    cPlotOscillogram->cPlot->replot();



    //soundfile.

    /*
    if ( !fname.isEmpty())
    {
        QFile f( fname );
        f.open( QIODevice::WriteOnly );
        QTextStream stream(&f);

        for (int i=0; i<cPlot->graph()->dataCount(); i++)
            stream << cPlot->graph()->data().data()->at(i)->value << "\n";

        f.close();
    }
*/

}







