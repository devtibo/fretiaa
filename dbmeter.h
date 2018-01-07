#ifndef DBMETER_H
#define DBMETER_H
#include <QWidget>
#include <QVector>
#include <QLabel>
#include "datasharer.h"

class dBMeter : public QWidget
{
    Q_OBJECT
public:
    explicit dBMeter(DataSharer *data, QWidget *parent = 0);
    DataSharer *m_data;

signals:

private :
    float Leq_PreviousValue= 0;
    QString ponderatinTime = "S";
    QString ponderationFrequency = "Z";
    QList<QString> ponderationTimeList;
    QList<QString> ponderationFrequencyList;
    QLabel *m_dBUnit;
    QLabel *m_dBValue;

public slots:
    void timeWeigthingChanged(int);
    void frequencyWeigthingChanged(int);
    void updateData();

signals:
    void dBWeightingChanged();

};

#endif // DBMETER_H
