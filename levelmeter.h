#ifndef LEVELMETER_H
#define LEVELMETER_H

#include <QWidget>
#include <QRect>
#include <QPainter>
#include "datasharer.h"

class LevelMeter : public QWidget
{
    Q_OBJECT

public:
    explicit LevelMeter(DataSharer *data, QWidget *parent = 0);
    void paintEvent(QPaintEvent *) override;
    DataSharer *m_data;

signals:

public slots:
    //void levelMeterChanged(float level);
    void updateData();
private :
    QRect rectBackground;
    int x1, y1, x2, y2;
    float m_level=0;

};

#endif // LEVELMETER_H
