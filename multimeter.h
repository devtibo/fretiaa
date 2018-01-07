#ifndef MULTIMETER_H
#define MULTIMETER_H

#include <QLabel>
#include "datasharer.h"

class MultiMeter : public QWidget
{
        Q_OBJECT

public:
    explicit MultiMeter(DataSharer *data, QWidget *parent=0);

    DataSharer* m_data;
    QLabel *lMaxValue = new QLabel("0.0");
    QLabel *lMinValue = new QLabel("0.0");
    QLabel *lAvgValue = new QLabel("0.0");
    QLabel *lAbsValue = new QLabel("0.0");
    QLabel *lMaxValueV = new QLabel("0.0");
    QLabel *lMinValueV = new QLabel("0.0");
    QLabel *lAvgValueV = new QLabel("0.0");
    QLabel *lAbsValueV = new QLabel("0.0");

    void setMaxValueV(float);
    void setMinValueV(float);
    void setAvgValueV(float);
    void setAbsValueV(float);

    void setMaxValuePa(float);
    void setMinValuePa(float);
    void setAvgValuePa(float);
    void setAbsValuePa(float);



    float getMaxValueInV();
    float getMaxValueInPa();

public slots:
    void updateData();
private :
    float maxVal_V;
    float maxVal_Pa;

};

#endif // MULTIMETER_H
