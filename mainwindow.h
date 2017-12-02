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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private :
    //![] Global Variables to scale data
    bool interpretAsVolt = false;
    float sensitivity = 1; // V/Pa
    float g1_lin=1, g2_lin=1;
    int length_fft = 1024;
    float g1_Value=0, g2_Value=0, gSensi_Value=sensitivity;
    QString g1_Unit="dB", g2_Unit="dB", gSensi_Unit="V/Pa";
    DataSharer *data = new DataSharer;

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
    uchar *dataSound;
    AudioEngine *m_AudioEngine;
    QVector<double> points_x;
    QVector<double> points_y;

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
    bool isOctave;
    OctaveSpectrum *cPlotOctaveSpectrum;

    void updateStatusBar();
    void updateGain();
    void closeEvent(QCloseEvent *);

    void createViewMenu();
    void createToolsMenu();
    void createFileMenu();
    void createOptionsMenu();
    void createHelpMenu();

public slots:
    // Sound acquisition Slot
    void data2Read(void);

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
    void updateOscData();
    void updateSpectrumData();
    void updateOctaveSpectrumData();
    void updateMultimeterOsc();
    void updateLevelMeter();
    void updatedBMeter();
    void updateSpectrogram();

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
    void dataAvalaible(void);

};

#endif // MAINWINDOW_H
