#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H


#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
#include <QList>
#include <QStringList>


class AudioEngine : public QObject
{
    Q_OBJECT
public:
    explicit AudioEngine(QObject *parent = 0);
    ~AudioEngine();
    QAudioInput *m_AudioInput;
    QIODevice *m_InputDevice;
    float getFs();
    QAudioFormat formatAudio;
    QAudioDeviceInfo inputDeviceInfo;
    QStringList getsavefilefileupportedSampleRatesAsString();
    QStringList getsavefilefileupportedSampleSizesAsString();
    QStringList getsavefilefileupportedCodecsAsString();
    QStringList getsavefilefileupportedChannelCountsAsString();


    int soundBufferSize;
private:

    float fs;


signals:

public slots:


};

#endif // AUDIOENGINE_H
