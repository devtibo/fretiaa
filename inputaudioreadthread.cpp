#include "inputaudioreadthread.h"
#include "windows.h"
#include <QtConcurrent>
#include <QFuture>
#include <QtConcurrentRun>


using namespace QtConcurrent;


InputAudioReadThread::InputAudioReadThread(DataSharer* data, QObject *parent )
{
    m_data = data;
    dataSound = new unsigned char [m_data->m_AudioEngine->soundBufferSize];


    /* Compute hanning window */
    Windows *m_win = new Windows(Windows::hanning,m_data->length_fft_spectrogram);
    win = m_win->getWin();

    nbreOfSamples = 0; // sometime bugs when initialized in h file



}

void InputAudioReadThread::run()
{
    if (frame1.size() == m_data->length_fft_spectrogram) // if frame1 is full. Clear frame1. Copy leaving points in frame1 and set the sample counter according to the number of leaving points.
    {

        frame1 = frame1Leaving; // if overlap 50%
        //  frame1.clear(); // else if overlap not 50%
        framesCpt1 = frame1Leaving.size();
        frame1Leaving.clear();
    }

    if (frame2.size() == m_data->length_fft_spectrogram) // if frame1 is full. Clear frame1. Copy leaving points in frame1 and set the sample counter according to the number of leaving points.
    {

        frame2 = frame2Leaving; // if overlap 50%
        //  frame1.clear(); // else if overlap not 50%
        framesCpt2 = frame2Leaving.size();
        frame2Leaving.clear();
    }







    qint64 range = round(m_data->observationTime * m_data->m_AudioEngine->getFs()) + 1 / m_data->m_AudioEngine->getFs(); // NEED TO BE GREATER THAN BUFFER SIZE
    //+1/Fs to go from 0 to observation time!!

    if (range < m_data->m_AudioEngine->soundBufferSize)
    {
        qDebug("ERROR range");
        return;
    }
    points_x.clear();
    points_y.clear();

    int resolution = 2; // 1: for 8bits and 2 for int

    // Fill the vector to Draw which is greater or equal to the input buffer size (i.e. range >= SOUND_BUFFER_SIZE)
    int maxSize = m_data->m_AudioEngine->m_InputDevice->read((char*)dataSound, m_data->m_AudioEngine->soundBufferSize);

    if(maxSize==-1)
        qDebug("ERROR");

    qint16 *ptr = reinterpret_cast<qint16*>(dataSound);


    // Remplir par laz lecture
    for (int k = 0; k < maxSize/resolution; k++){
        points_x.append(nbreOfSamples/m_data->m_AudioEngine->getFs());
        if (m_data->interpretAsVolt)
            points_y.append(m_data->g1_lin * m_data->g2_lin * ptr[k]/(pow(2,8*resolution)/2));
        else
            points_y.append(m_data->g1_lin * m_data->g2_lin * ptr[k]/(pow(2,8*resolution)/2)/m_data->gSensi_lin);


        nbreOfSamples ++;

        if(m_data->isSpectrogramShow)
        {
            //Process Frame1
            processFrame1_inFLy();
            //Process Frame2
            if(nbreOfSamples >= m_data->length_fft_spectrogram/2 )
                processFrame2_inFLy();

            //                qDebug("FRAMEQUEUE# %d",m_data->framesQueue.size());
        }
    }
    //nbreOfSamples+= (maxSize/resolution);

    m_data->writeData(points_x,points_y);
    //m_data->ReadPoints_x = points_x; // Take a lot of time to copy
    //m_data->ReadPoints_y = points_y;



}

void InputAudioReadThread::processFrame1_inFLy()
{
    //FRAMMING FOR SPECTROGRAM

    if (framesCpt1 <  m_data->length_fft_spectrogram)
    {
        frame1.append( points_y.last()*win.at(frame1.size()));
    }

    if  (framesCpt1 >=  m_data->length_fft_spectrogram)
    {
        frame1Leaving.append( points_y.last()*win.at(frame1Leaving.size())); // save remaining points for the next pass

    }
    framesCpt1++;

    if  (framesCpt1 ==  m_data->length_fft_spectrogram)
    {
        // m_data->wrireFrame1Data(frame1);
        m_data->framesQueue.enqueue(frame1);
        //qDebug("emit0: %d", nbreOfSamples);
        //m_data->qPlotSpectrogram->start(QThread::HighestPriority);
        //emit spectrogramFrameReady(); // Too Slow
      //  qDebug("FRAMEQUEUE# %d",m_data->framesQueue.size());
        QFuture<void> f1 = QtConcurrent::run(m_data->qPlotSpectrogram,&Spectrogram::run);

    }
}

void InputAudioReadThread::processFrame2_inFLy()
{
    //FRAMMING FOR SPECTROGRAM

    if (framesCpt2 <  m_data->length_fft_spectrogram)
    {
        frame2.append( points_y.last() *win.at(frame2.size()));
    }

    if  (framesCpt2 >=  m_data->length_fft_spectrogram)
        frame2Leaving.append( points_y.last() *win.at(frame2Leaving.size())); // save remaining points for the next pass

    framesCpt2++;

    if  (framesCpt2 ==  m_data->length_fft_spectrogram)
    {
        //m_data->wrireFrame2Data(frame2);
        m_data->framesQueue.enqueue(frame2);
        //   qDebug("emit1 %d", nbreOfSamples);
        // m_data->qPlotSpectrogram->start(QThread::HighestPriority);
     //   qDebug("FRAMEQUEUE# %d",m_data->framesQueue.size());
        QFuture<void> f2 = QtConcurrent::run(m_data->qPlotSpectrogram,&Spectrogram::run);
        //emit spectrogramFrameReady(); // Too slow
    }
}

