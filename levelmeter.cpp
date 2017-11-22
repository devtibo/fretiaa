#include "levelmeter.h"

LevelMeter::LevelMeter(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setMinimumWidth(75);
}

void LevelMeter::paintEvent(QPaintEvent *)
{
    // Rectangles declaration
    auto rectGradient= rect().adjusted(0, 0, 0, 0);
    rectGradient.getCoords(&x1,&y1,&x2,&y2);
    rectBackground.setCoords(x1,y1,x2-x1,(y2-y1)*(1-m_level));

    //Gradient definition
    QLinearGradient gradient;
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setStart(0,0);
    gradient.setFinalStop(0, 1);
    gradient.setColorAt(0, QColor::fromRgbF(1, 0, 0, 1));
    gradient.setColorAt(0.1, QColor::fromRgbF(1, 0, 0, 1));
    gradient.setColorAt(0.2, QColor::fromRgbF(1, 1, 0, 1));
    gradient.setColorAt(0.8, QColor::fromRgbF(0, 1, 0, 1));

    // Paint rectangles
    QPainter painter(this);
    painter.drawRect(rectGradient);
    painter.fillRect(rectGradient,QBrush(gradient));
    painter.drawRect(rectBackground);
    painter.fillRect(rectBackground,Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
}

/** =============================== **/
/** =========== SLOTS ============= **/
/** =============================== **/

// Slot: Update
void LevelMeter::levelMeterChanged(float level)
{
    m_level = level;
    emit update();
}
