#include "audioengine.h"


//#include <QDebug>
#include <QWidget>
//REFERS TO : http://grimaldi.univ-tln.fr/Utilisation%20d%27une%20carte%20sn%20pour%20acqu%C3%A9rir%20des%20signaux.html


AudioEngine::AudioEngine(QObject *parent) : QObject(parent), m_AudioInput(0)
{


#ifdef Q_OS_MAC
    soundBufferSize = 256;
#else
    soundBufferSize =  1024;
#endif



    //! [0]
    inputDeviceInfo = QAudioDeviceInfo::defaultInputDevice();

    if (inputDeviceInfo.supportedSampleRates().size() > 0
            && inputDeviceInfo.supportedChannelCounts().size() > 0
            && inputDeviceInfo.supportedSampleSizes().size() > 0
            && inputDeviceInfo.supportedCodecs().size() > 0) {

        int idxfs = 3;
        formatAudio.setSampleRate(inputDeviceInfo.supportedSampleRates().at(idxfs)); // set to 0 for 8kHz
        formatAudio.setChannelCount(inputDeviceInfo.supportedChannelCounts().at(0));
        formatAudio.setSampleSize(inputDeviceInfo.supportedSampleSizes().at(1));// set to 1 for 16bits
        formatAudio.setCodec(inputDeviceInfo.supportedCodecs().at(0));
        formatAudio.setByteOrder(QAudioFormat::LittleEndian);
        formatAudio.setSampleType(QAudioFormat::SignedInt);
        /*
        qDebug("Sample Rate : %d",inputDeviceInfo.supportedSampleRates().at(0));
        qDebug("Channle Count : %d",inputDeviceInfo.supportedChannelCounts().at(0));
        qDebug("Sample Size : %d",inputDeviceInfo.supportedSampleSizes().at(0));
        qDebug("Codec : %s",inputDeviceInfo.supportedCodecs().at(0));
*/
        m_AudioInput = new QAudioInput(inputDeviceInfo, formatAudio, this);
        m_AudioInput->setBufferSize(soundBufferSize);

        fs = (float)inputDeviceInfo.supportedSampleRates().at(idxfs);
    }
}

AudioEngine::~AudioEngine()
{
    if (m_AudioInput)
        m_AudioInput->stop();
}

QStringList AudioEngine::getSupportedSampleRatesAsString(){
    QStringList list;
    for (int i=0; i< inputDeviceInfo.supportedSampleRates().size();i++)
        list.append(QString::number(inputDeviceInfo.supportedSampleRates().at(i)));

    return list;
}

QStringList AudioEngine::getSupportedChannelCountsAsString()
{
    QStringList list;
    for (int i=0; i< inputDeviceInfo.supportedChannelCounts().size();i++)
        list.append(QString::number(inputDeviceInfo.supportedChannelCounts().at(i)));

    return list;
}


QStringList AudioEngine::getSupportedSampleSizesAsString()
{
    QStringList list;
    for (int i=0; i< inputDeviceInfo.supportedSampleSizes().size();i++)
        list.append(QString::number(inputDeviceInfo.supportedSampleSizes().at(i)));

    return list;
}


QStringList AudioEngine::getSupportedCodecsAsString()
{
    return inputDeviceInfo.supportedCodecs();
}


float AudioEngine::getFs()
{
    return this->fs;
}
