#ifndef AUDIOINPUTCONFIGDIAL_H
#define AUDIOINPUTCONFIGDIAL_H
#include "audioengine.h"

#include <QDialog>
#include <QComboBox>

class AudioInputConfigDial : public QDialog
{
    Q_OBJECT

public:
    AudioInputConfigDial(AudioEngine *audioengine,QWidget *parent = 0);

    // To create the dialog Box
    QComboBox* cbxDevice  = new QComboBox;
    QComboBox* cbxSampleRate = new QComboBox;
    QComboBox* cbxSampleSize = new QComboBox;
    QComboBox* cbxChannelCount = new QComboBox;
    QComboBox* cbxCodec = new QComboBox;


    // To save current Device informations
    QString currentDeviceName;
    int currentSampleRate;
    int currentChannelCount;
    int currentSampleSize;
    QString currentCodec;

    // To obtain avaible devices informations
    QAudioDeviceInfo deviceInfo;

    // To set new device settings
    QAudioFormat settings;

public slots:
    // Update available settings when device change
    void updateDeviceInfo(int);

    // Update new settings to change device
    void sampleRateChanged(int);
    void sampleSizeChanged(int);
    void channelCountChanged(int);
    void codecChanged(int);

    // Button Slots
    // void cancelButtonPuched();
    void applyButtonPuched();
};

#endif // AUDIOINPUTCONFIGDIAL_H
