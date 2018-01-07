#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractButton>
#include <QToolButton>
#include <QLabel>

#include "qcustomplot/qcustomplot.h" // Add "printsupport" to .pro file
#include "multimeter.h"
#include "audioengine.h"
#include "oscillogram.h"
#include "levelmeter.h"
#include "dbmeter.h"
#include "datasharer.h"
#include "clickablelabel.h"
#include "spectrum.h"
#include "octavespectrum.h"
#include "spectrogram.h"
#include "biquad.h"


#include "inputaudioreadthread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private :
    //![] Global Variables to scale data
    // bool interpretAsVolt = false;
    DataSharer *data = new DataSharer;
    QTimer *RefreshTimer;
    //![] HP filter
    Biquad *HP_filter;

    //![] Multimeter
    MultiMeter *multiMeterOsc;

    //![] TOOL BAR
    QAction *liveView ;
    bool isLiveView=true;
    QAction *plotSpectro ;
    QAction *plotSpectrum ;
    QAction *plotOctaveSpectrum;
    QAction *interactionMoveAnalyseRect ;
    QAction *interactionHP;
    QAction *interactionTrigger;

    //![] Status Bar
    QLabel *lab_status;
    ClickableLabel *lab_SoundCardInfo;
    ClickableLabel *lab_GainInfo ;

    //![] Mulitmeter
    MultiMeter *multiMeterSpec;

    //![] Main Layout
    QGridLayout * centralLayout = new QGridLayout;

    //![] AudioEngine
    AudioEngine *m_AudioEngine;



    //![] Oscillogram
    Oscillogram *cPlotOscillogram;

    //![] LevelMeter
    LevelMeter *m_LevelMeter;
    dBMeter *m_dBMeter;

    //![] Spectrum
    Spectrum *cPlotSpectrum;
    bool isSpectrum;
    // qint64 dataCounter;

    //![] Spectrogram
    Spectrogram *cPlotSpectrogram;

    //![] Octave Spectrum
    bool isOctaveView;
    OctaveSpectrum *cPlotOctaveSpectrum;

    void updateStatusBar();

    void closeEvent(QCloseEvent *);

    void createViewMenu();
    void createToolsMenu();
    void createFileMenu();
    void createOptionsMenu();
    void createHelpMenu();

    InputAudioReadThread *mAudioThread;

public slots:

    void connectWidgets();
    void connectRectWidgets();

    //Slot of ToolBar
    void liveViewChanged(bool);
    void plotSpectogramButtonChanged(bool);
    void plotAsChanged(int);
    void plotSpectrumChanged(bool);
    void plotOctaveSpectrumChanged(bool);
    void spectrumIsClosingCatch(); // To unchecked button in toolbar
    void octaveIsClosingCatch();// To unchecked button in toolbar
    void onTriggerChanged(bool);

    //Slot oscilloscope interaction
    void onMoveAnalyseRect(bool);
    void updateTriggered();

    // Slot to update widgets
    void updateSpectrogram();
    void updateSpectrogramOnShot();



    // ShortCuts
    void onKeySpacePress();
    void onKeyTPress();
    void onKeyRPress();
    void onKeySPress();
    void onKeyOPress();
    void onKeyHPress();
    void onKeyGPress();

    // DialogBox
    void openAudioConfigDialog();
    void openGainConfigDialog();
    void openAboutDialog();

    void exitApp();

signals:
    //void dataAvalaible(void);


};

#endif // MAINWINDOW_H
