#include "dbmeter.h"

#include <QGridLayout>
#include <QComboBox>

dBMeter::dBMeter(DataSharer *data, QWidget *parent) : QWidget(parent)
{
    m_data = data;
    // Debicel unit label
    m_dBUnit = new QLabel("dB(SPL)<sub>S</sub>");
    QFont f2 = font();
    f2.setBold(true);
    f2.setPointSize(10);
    m_dBUnit->setFont(f2);
    m_dBUnit->setAlignment(Qt::AlignBottom);

    // Debicel value Label
    m_dBValue = new QLabel("00.00");
    QFont f = font();
    f.setBold(true);
    f.setPointSize(15);
    m_dBValue->setFont(f);

    // Frequency weighting combobox
    QComboBox *combo_weigth = new QComboBox;
    combo_weigth->addItem("Z-weighting (SPL)");
    combo_weigth->addItem("A-weighting (A)");
    combo_weigth->addItem("C-weighting (C)");
    ponderationFrequencyList.append("SPL");
    ponderationFrequencyList.append("A");
    ponderationFrequencyList.append("C");
    combo_weigth->setEnabled(false); // TODO !!!!!!!!!

    // Time weighting Combobox
    QComboBox *combo_speed = new QComboBox;
    combo_speed->addItem("Slow (S)");
    combo_speed->addItem("Fast (F)");
    combo_speed->addItem("Impulse (I)");
    combo_speed->addItem("Direct (D)");
    ponderationTimeList.append("S");
    ponderationTimeList.append("F");
    ponderationTimeList.append("I");
    ponderationTimeList.append("D");

    // Create and fill layout
    QGridLayout *m_layout = new QGridLayout;
    m_layout->addWidget(m_dBValue,0,0);
    m_layout->addWidget(m_dBUnit,0,1);
    m_layout->addWidget(combo_weigth,1,0,1,2);
    m_layout->addWidget(combo_speed,2,0,1,2);
    this->setLayout(m_layout);

    // Connections
    connect(combo_speed,SIGNAL(currentIndexChanged(int)),this,SLOT(timeWeigthingChanged(int)));
    connect(combo_weigth,SIGNAL(currentIndexChanged(int)),this,SLOT(frequencyWeigthingChanged(int)));

}

/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/

// See http://www.nti-audio.com/en/support/faq/fast-slow-impulse-time-weighting-what-do-they-mean.aspx for more details anout time weighting

// Slots : Update Data
void dBMeter::updateData()
{

    QVector<double> data =m_data->ReadRectData();
    float Fs = m_data->fs;


    float alpha;
    float alphaLongDecay;
    bool isPeak;
    QVector<double> Leq;
    Leq.append(Leq_PreviousValue);
    int i;
    int j;
    int length = data.size();

    // Set time weighting parameters
    if (!ponderatinTime.compare("D"))
        alpha = 1;
    else if (!ponderatinTime.compare("F"))
        alpha = 1-exp(-(1/Fs)/0.125);//% time constant of 125ms
    else if (!ponderatinTime.compare("S"))
        alpha = 1-exp(-(1/Fs)/1); // time constant of 1s
    else if (!ponderatinTime.compare("I"))
    {
        alpha = 1-exp(-(1/Fs)/0.035); // Rising time constant of 0.035s
        alphaLongDecay = 1-exp(-(1/Fs)/1.5); // Falling time constant of 1.5s
    }

    /*** Compute dB ***/
    //For Time Weighing "I"
    if (!ponderatinTime.compare("I"))
    {
        return ; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!BUG !!!!!
        QVector<double>     Leq_tmp;
        Leq_tmp.append(Leq_PreviousValue);
        QVector<double> LeqLongDecay;
        LeqLongDecay.append(Leq_PreviousValue);


        for (i=0, j=1; i<length;i++,j++)
        {
            Leq_tmp.append(Leq_tmp.at(j-1) + alpha * (data.at(i-1)*data.at(i-1) - Leq_tmp.at(j-1)));
            if (((j+1)-round(0.001*Fs)) >0) // A vERIFIER
            {
                if (Leq_tmp.at(j) < Leq_tmp.at((j+1)-round(0.001*Fs))) // i.e. if the SPL is falling
                {
                    if (isPeak)
                    {
                        isPeak = false;
                        LeqLongDecay.replace(j-1, Leq_tmp.at(j-1));
                        LeqLongDecay.append(LeqLongDecay.at(j-1));
                    }
                    else
                        LeqLongDecay.append(LeqLongDecay.at(j-1) + alphaLongDecay * (data.at(i-1)*data.at(i-1) - LeqLongDecay.at(j-1)));
                }
                else
                {
                    isPeak = true;
                    LeqLongDecay.append(LeqLongDecay.at(j-1) + alphaLongDecay * (data.at(i-1)*data.at(i-1) - LeqLongDecay.at(j-1)));
                }
            } else
                LeqLongDecay.append(0);

            Leq.append(qMax(Leq_tmp.at(i),LeqLongDecay.at(i)));
        }
    }
    // For Time Weighing "D", "F" or "S"
    else
    {
        for(i=0, j=1; i<0+length;i++,j++){ // A REVOIR !!!!!
            Leq.append(Leq.at(j-1) + alpha * (data.at(i) * data.at(i) - Leq.at(j-1)));

        }
    }


    // Convert to dB SPL.
    for (int i=0;i<Leq.size();i++)
        Leq.replace(i, 20*log10(sqrt(Leq.at(i))/2e-5) );

    // Update Lavel
    m_dBValue->setText(QString::number(Leq.at(round(Leq.size()/2)),'f',2));
}

// Slot : Time Weithing Changed
void dBMeter::timeWeigthingChanged(int idx)
{
    ponderatinTime = ponderationTimeList.at(idx);
    m_dBUnit->setText(QString("dB(%1)<sub>%2</sub>").arg(ponderationFrequency).arg(ponderatinTime ));
    emit dBWeightingChanged();
}

// Slot : Freqeuncy Weithing  Changed
void dBMeter::frequencyWeigthingChanged(int idx)
{
    ponderationFrequency = ponderationFrequencyList.at(idx);
    m_dBUnit->setText(QString("dB(%1)<sub>%2</sub>").arg(ponderationFrequency).arg(ponderatinTime ));
    emit dBWeightingChanged();
}





