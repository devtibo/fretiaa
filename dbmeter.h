#ifndef DBMETER_H
#define DBMETER_H
#include <QWidget>
#include <QVector>
#include <QLabel>

class dBMeter : public QWidget
{
    Q_OBJECT
public:
    explicit dBMeter(QWidget *parent = 0);
    void setData(QVector<double>, float, int, int);
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

signals:
    void dBWeightingChanged();

};

#endif // DBMETER_H
