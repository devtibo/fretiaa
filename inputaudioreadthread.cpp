#include "inputaudioreadthread.h"

InputAudioReadThread::InputAudioReadThread(DataSharer* data, QObject *parent )
{
    m_data = data;

    /*   for (int i=0; i<=data->observationTime*data->fs; i++)
        timeVec.append(i/data->fs);*/
    dataSound = new unsigned char [m_data->m_AudioEngine->soundBufferSize];
}

void InputAudioReadThread::run()
{

    qint64 range = round(m_data->observationTime * m_data->m_AudioEngine->getFs()) + 1 / m_data->m_AudioEngine->getFs(); // NEED TO BE GREATER THAN BUFFER SIZE
    //+1/Fs to go from 0 to observation time!!

    if (range < m_data->m_AudioEngine->soundBufferSize)
    {        qDebug("ERROR range");
        return;
    }


    points_x.clear();
    points_y.clear();

    int resolution = 2; // 1: for 8bits and 2 for int

    // Fill the vector to Draw which is greater or equal to the input buffer size (i.e. range >= SOUND_BUFFER_SIZE)
    qint64 maxSize = m_data->m_AudioEngine->m_InputDevice->read((char*)dataSound, m_data->m_AudioEngine->soundBufferSize);

    if(maxSize==-1)
        qDebug("ERROR");

    qint16 *ptr = reinterpret_cast<qint16*>(dataSound);


    // Remplir par laz lecture
    for (int k = 0; k < maxSize/resolution; k++){
        points_x.append(k/m_data->m_AudioEngine->getFs() + nbreOfSamples/m_data->m_AudioEngine->getFs());
        points_y.append(m_data->g1_lin * m_data->g2_lin * ptr[k]/(pow(2,8*resolution)/2)/m_data->sensitivity);
    }
    nbreOfSamples+= (maxSize/resolution);

    m_data->ReadPoints_x = points_x; // Take a lot of time to copy
    m_data->ReadPoints_y = points_y;

    // qDebug("mTHREAD execution Time : %d ms",timeProfiler.elapsed());

}
