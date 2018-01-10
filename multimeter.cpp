#include "multimeter.h"
#include <QGridLayout>


MultiMeter::MultiMeter(DataSharer *data, QWidget *parent) : QWidget(parent)
{

    m_data=data;
    // Create widgetsavefilefile and the layout
    QFont f = font();
    f.setUnderline(true);

    QLabel *sep0 = new QLabel("Pressure");
    sep0->setFont(f);

    QLabel *lMaxText = new QLabel("Max");
    QLabel *lMaxUnit = new QLabel("Pa");

    QLabel *lMinText = new QLabel("Min");
    QLabel *lMinUnit = new QLabel("Pa");

    QLabel *lAvgText = new QLabel("Avg");
    QLabel *lAvgUnit = new QLabel("Pa");

    QLabel *lAbsText = new QLabel("RMS");
    QLabel *lAbsUnit = new QLabel("Pa");

    QLabel *sep1 = new QLabel("Voltage");
    sep1->setFont(f);

    QLabel *lMaxTextV = new QLabel("Max");
    QLabel *lMaxUnitV = new QLabel("V");

    QLabel *lMinTextV = new QLabel("Min");
    QLabel *lMinUnitV = new QLabel("V");

    QLabel *lAvgTextV = new QLabel("Avg");
    QLabel *lAvgUnitV = new QLabel("V");

    QLabel *lAbsTextV = new QLabel("RMS");
    QLabel *lAbsUnitV = new QLabel("V");

    QGridLayout *m_layout = new QGridLayout;

    // Add widgetsavefilefile to layout
    m_layout->addWidget(sep0,0,1,1,2);

    m_layout->addWidget(lMaxText,1,0);
    m_layout->addWidget(lMaxValue,1,1);
    m_layout->addWidget(lMaxUnit,1,2);

    m_layout->addWidget(lMinText,2,0);
    m_layout->addWidget(lMinValue,2,1);
    m_layout->addWidget(lMinUnit,2,2);

    m_layout->addWidget(lAvgText,3,0);
    m_layout->addWidget(lAvgValue,3,1);
    m_layout->addWidget(lAvgUnit,3,2);

    m_layout->addWidget(lAbsText,4,0);
    m_layout->addWidget(lAbsValue,4,1);
    m_layout->addWidget(lAbsUnit,4,2);

    m_layout->addWidget(sep1,5,1,1,2);

    m_layout->addWidget(lMaxTextV,6,0);
    m_layout->addWidget(lMaxValueV,6,1);
    m_layout->addWidget(lMaxUnitV,6,2);

    m_layout->addWidget(lMinTextV,7,0);
    m_layout->addWidget(lMinValueV,7,1);
    m_layout->addWidget(lMinUnitV,7,2);

    m_layout->addWidget(lAvgTextV,8,0);
    m_layout->addWidget(lAvgValueV,8,1);
    m_layout->addWidget(lAvgUnitV,8,2);

    m_layout->addWidget(lAbsTextV,9,0);
    m_layout->addWidget(lAbsValueV,9,1);
    m_layout->addWidget(lAbsUnitV,9,2);
    m_layout->setAlignment(Qt::AlignTop);

    this->setLayout(m_layout);

}

float MultiMeter::getMaxValueInV(){return maxVal_V;}

float MultiMeter::getMaxValueInPa(){return maxVal_Pa;}

/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/

void MultiMeter::updateData()
{

    QVector<double> points_y = m_data->ReadRectData();

    float minVal_V, avgVal_V,rmsVal_V;
    float minVal_Pa, avgVal_Pa,rmsVal_Pa;
    double val = 1;

    // init Values
    if (m_data->interpretAsVolt){
        minVal_V = points_y.at(0);
        maxVal_V = points_y.at(0);
    }
    else
    {
        minVal_V = points_y.at(0)*m_data->gSensi_lin;
        maxVal_V = points_y.at(0)*m_data->gSensi_lin;
    }
    avgVal_V = 0 ;
    rmsVal_V = 0 ;

    // Loop to update values
    for (int i=0; i<points_y.size();i++){
        if (!m_data->interpretAsVolt)
            val  = points_y.at(i)* m_data->gSensi_lin;
        else
            val = points_y.at(i);

        maxVal_V = qMax(maxVal_V,((float)(val)));
        minVal_V = qMin(minVal_V,((float)(val)));
        avgVal_V += points_y.at(i);
        rmsVal_V += points_y.at(i) * val;
    }

    // convertion from V to Pa
    maxVal_Pa = maxVal_V / m_data->gSensi_lin;
    minVal_Pa = minVal_V / m_data->gSensi_lin;
    avgVal_V  = avgVal_V/points_y.size();
    avgVal_Pa = (avgVal_V/m_data->gSensi_lin)/points_y.size();
    rmsVal_V  = sqrt(rmsVal_V/points_y.size());
    rmsVal_Pa = sqrt((rmsVal_V/m_data->gSensi_lin)/points_y.size());

    // Update labels
    QString text;
    text.sprintf("%02.3f", maxVal_Pa);
    lMaxValue->setText(text);
    text.sprintf("%02.3f", maxVal_V);
    lMaxValueV->setText(text);

    text.sprintf("%02.3f", minVal_Pa);
    lMinValue->setText(text);
    text.sprintf("%02.3f", minVal_V);
    lMinValueV->setText(text);

    text.sprintf("%02.3f", rmsVal_Pa);
    lAbsValue->setText(text);
    text.sprintf("%02.3f", rmsVal_V);
    lAbsValueV->setText(text);

    text.sprintf("%02.3f", avgVal_Pa);
    lAvgValue->setText(text);
    text.sprintf("%02.3f", avgVal_V);
    lAvgValueV->setText(text);

    m_data->maxValueInPa = maxVal_Pa;
    m_data->maxValueInV = maxVal_V;

}

