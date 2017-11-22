#ifndef LEVELMETER_H
#define LEVELMETER_H

#include <QWidget>
#include <QRect>
#include <QPainter>


class LevelMeter : public QWidget
{
    Q_OBJECT

public:
    explicit LevelMeter(QWidget *parent = 0);
    void paintEvent(QPaintEvent *) override;

signals:

public slots:
    void levelMeterChanged(float level);

private :
    QRect rectBackground;
    int x1, y1, x2, y2;
    float m_level=0;

};

#endif // LEVELMETER_H
