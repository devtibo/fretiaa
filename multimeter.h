#ifndef MULTIMETER_H
#define MULTIMETER_H

#include <QLabel>

class MultiMeter : public QWidget
{
public:
    MultiMeter(QWidget *parent = 0);

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

    void setData(QVector<double>, float, int, int, bool);

    float getMaxValueInV();
    float getMaxValueInPa();

private :
    float maxVal_V;
    float maxVal_Pa;

};

#endif // MULTIMETER_H
