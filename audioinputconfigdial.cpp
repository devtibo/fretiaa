#include "audioinputconfigdial.h"
#include <QBoxLayout>
#include <QWidget>
#include <QLabel>

#include <QString>
#include <QDialogButtonBox>
#include <QPushButton>


#include <QMessageBox>

#include <QtMultimedia/QAudioDeviceInfo>
AudioInputConfigDial::AudioInputConfigDial(AudioEngine *audioengine)
{
    QVBoxLayout *layout = new QVBoxLayout;

    // Get Current Device name
    currentDeviceName = audioengine->inputDeviceInfo.deviceName();// A verifier

    /* Create combobox layout */

    // Device Name ComboBox Title and filling
    QLabel *l = new QLabel("Sound Cards :");
    layout->addWidget(l);
    cbxDevice = new QComboBox();
    foreach (const QAudioDeviceInfo &deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
        cbxDevice->addItem(deviceInfo.deviceName(), qVariantFromValue(deviceInfo));
    cbxDevice->setCurrentText(currentDeviceName);
    layout->addWidget(cbxDevice);

    // Horizontal seprarator line
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);

    // Sampling frequency Title and ComboBox filling
    QLabel *l1 = new QLabel("Sampling Frequency (Hz) :");
    layout->addWidget(l1);
    cbxSampleRate->addItems(audioengine->getSupportedSampleRatesAsString());
    currentSampleRate = audioengine->formatAudio.sampleRate();
    cbxSampleRate->setCurrentText(QString::number(currentSampleRate));
    layout->addWidget(cbxSampleRate);

    // Sample size Title and ComboBox filling
    QLabel *l2 = new QLabel("Sample Size (Bits) :");
    layout->addWidget(l2);
    cbxSampleSize->addItems(audioengine->getSupportedSampleSizesAsString());
    currentSampleSize = audioengine->formatAudio.sampleSize();
    cbxSampleSize->setCurrentText(QString::number((currentSampleSize)));
    layout->addWidget(cbxSampleSize);

    // Channel count Title and ComboBox filling
    QLabel *l3 = new QLabel("Channel Count :");
    layout->addWidget(l3);
    cbxChannelCount->addItems(audioengine->getSupportedChannelCountsAsString());
    currentChannelCount = audioengine->formatAudio.channelCount();
    cbxChannelCount->setCurrentText(QString::number(currentChannelCount));
    layout->addWidget(cbxChannelCount);


    // Codec Title and ComboBox filling
    QLabel *l4 = new QLabel("Codecs :");
    layout->addWidget(l4);
    cbxCodec->addItems(audioengine->getSupportedCodecsAsString());
    currentCodec  =audioengine->formatAudio.codec();
    cbxCodec->setCurrentText(currentCodec);
    layout->addWidget(cbxCodec);

    // Add buttons
    // Create Apply/Cancel Buttons in GroupButton
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                       | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    /*
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
    QPushButton * applyButton = new QPushButton(tr("&Apply"));
    buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(applyButton, QDialogButtonBox::ActionRole);
    applyButton->setDefault(true);
    applyButton->setAutoDefault(true);
    cancelButton->setDefault(false);
    cancelButton->setAutoDefault(false);
    layout->addWidget(buttonBox);*/

    // connect ComboBox
    connect(cbxDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDeviceInfo(int)));
    connect(cbxSampleRate, SIGNAL(currentIndexChanged(int)), this, SLOT(sampleRateChanged(int)));
    connect(cbxSampleSize, SIGNAL(currentIndexChanged(int)), this, SLOT(sampleSizeChanged(int)));
    connect(cbxChannelCount, SIGNAL(currentIndexChanged(int)), this, SLOT(channelCountChanged(int)));
    connect(cbxCodec, SIGNAL(currentIndexChanged(int)), this, SLOT(codecChanged(int)));

    // Connect buttons
    /* connect(cancelButton, SIGNAL (released()),this, SLOT (cancelButtonPuched()));
    connect(applyButton, SIGNAL (released()),this, SLOT (applyButtonPuched()));*/

    connect(buttonBox,SIGNAL(accepted()),dial,SLOT(accept()));
    connect(buttonBox,SIGNAL(rejected()),dial, SLOT(reject()));

    // Dialog Box Configuration
    dial->setModal(true);
    dial->setWindowFlags(Qt::Tool);
    dial->setWindowTitle("Audio Input Configuration");
    dial->setLayout(layout);
    dial->exec();

}

/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/

// Slot : Update Device Info
void AudioInputConfigDial::updateDeviceInfo(int i)
{
    // Update Device Name combobox
    deviceInfo = cbxDevice->itemData(i).value<QAudioDeviceInfo>();

    // Update Device SampleRate combobox
    cbxSampleRate->clear();
    for(int j=0; j<deviceInfo.supportedSampleRates().size();j++)
        cbxSampleRate->addItem(QString::number(deviceInfo.supportedSampleRates().at(j)));
    settings.setSampleRate(deviceInfo.supportedSampleRates().at(0));

    // Update Device SampleSier Combobox
    cbxSampleSize->clear();
    for(int j=0; j<deviceInfo.supportedSampleSizes().size();j++)
        cbxSampleSize->addItem(QString::number(deviceInfo.supportedSampleSizes().at(j)));
    settings.setSampleSize(deviceInfo.supportedSampleSizes().at(0));

    // Update Device ChannelCount Combobox
    cbxChannelCount->clear();
    for(int j=0; j<deviceInfo.supportedChannelCounts().size();j++)
        cbxChannelCount->addItem(QString::number(deviceInfo.supportedChannelCounts().at(j)));
    settings.setChannelCount(deviceInfo.supportedChannelCounts().at(0));

    // Update Device Supported Codec ComboBox
    cbxCodec->clear();
    for(int j=0; j<deviceInfo.supportedCodecs().size();j++)
        cbxCodec->addItem(deviceInfo.supportedCodecs().at(j));
    settings.setCodec(deviceInfo.supportedCodecs().at(0));

    // Update ComboBox values for current device
    if (!(this->cbxDevice->currentText().compare(currentDeviceName)))
    {
        cbxSampleRate->setCurrentText(QString::number(currentSampleRate));
        cbxSampleSize->setCurrentText(QString::number(currentSampleSize));
        cbxChannelCount->setCurrentText(QString::number(currentChannelCount));
        cbxCodec->setCurrentText(currentCodec);
    }

}

//Slot :
void AudioInputConfigDial::sampleRateChanged(int idx)
{
    settings.setSampleRate(cbxSampleRate->itemText(idx).toInt());
}

//Slot :
void AudioInputConfigDial::sampleSizeChanged(int idx)
{
    settings.setSampleSize(cbxSampleSize->itemText(idx).toInt());
}


//Slot:
void AudioInputConfigDial::channelCountChanged(int idx)
{
    settings.setChannelCount(cbxChannelCount->itemText(idx).toInt());
}

//Slot:
void AudioInputConfigDial::codecChanged(int idx)
{
    settings.setCodec(cbxCodec->itemText(idx));
}

// Slot : Apply
void AudioInputConfigDial::applyButtonPuched()
{
    ;

    // 2 - Stop and start new audio engine;
}

/*
// Slot :Exit
void AudioInputConfigDial::cancelButtonPuched()
{
    dial->close();
    delete this;
}
*/
