#include "qcpitemtracerbars.h"


QCPItemTracerBars::QCPItemTracerBars(QCustomPlot *parentPlot) :
    QCPAbstractItem(parentPlot),
    position(createPosition(QLatin1String("position"))),
    mSize(6),
    mStyle(tsCrosshair),
    mBars(0),
    mGraphKey(0),
    mInterpolating(false)
{
    position->setCoords(0, 0);

    setBrush(Qt::NoBrush);
    setSelectedBrush(Qt::NoBrush);
    setPen(QPen(Qt::black));
    setSelectedPen(QPen(Qt::blue, 2));
}

QCPItemTracerBars::~QCPItemTracerBars()
{
    ;
}

/*!
  Sets the pen that will be used to draw the line of the tracer

  \see setSelectedPen, setBrush
*/

void QCPItemTracerBars::setPen(const QPen &pen)
{
    mPen = pen;
}

/*!
  Sets the pen that will be used to draw the line of the tracer when selected

  \see setPen, setSelected
*/
void QCPItemTracerBars::setSelectedPen(const QPen &pen)
{
    mSelectedPen = pen;
}

/*!
  Sets the brush that will be used to draw any fills of the tracer

  \see setSelectedBrush, setPen
*/
void QCPItemTracerBars::setBrush(const QBrush &brush)
{
    mBrush = brush;
}

/*!
  Sets the brush that will be used to draw any fills of the tracer, when selected.

  \see setBrush, setSelected
*/
void QCPItemTracerBars::setSelectedBrush(const QBrush &brush)
{
    mSelectedBrush = brush;
}

/*!
  Sets the size of the tracer in pixels, if the style supports setting a size (e.g. \ref tsSquare
  does, \ref tsCrosshair does not).
*/
void QCPItemTracerBars::setSize(double size)
{
    mSize = size;
}

/*!
  Sets the style/visual appearance of the tracer.

  If you only want to use the tracer \a position as an anchor for other items, set \a style to
  \ref tsNone.
*/
void QCPItemTracerBars::setStyle(QCPItemTracerBars::TracerStyle style)
{
    mStyle = style;
}

/*!
  Sets the QCPGraph this tracer sticks to. The tracer \a position will be set to type
  QCPItemPosition::ptPlotCoords and the axes will be set to the axes of \a graph.

  To free the tracer from any graph, set \a graph to 0. The tracer \a position can then be placed
  freely like any other item position. This is the state the tracer will assume when its graph gets
  deleted while still attached to it.

  \see setGraphKey
*/

/*void QCPItemTracerBars::setGraph(QCPGraph *graph)
{
    if (graph)
    {
        if (graph->parentPlot() == mParentPlot)
        {
            position->setType(QCPItemPosition::ptPlotCoords);
            position->setAxes(graph->keyAxis(), graph->valueAxis());
            mGraph = graph;
            updatePosition();
        } else
            qDebug() << Q_FUNC_INFO << "graph isn't in same QCustomPlot instance as this item";
    } else
    {
        mGraph = 0;
    }
}*/ // FUX

/*!
  BLABLA FUX
  */
void QCPItemTracerBars::setBars(QCPBars *bars)
{
    if (bars)
    {
        if(bars->parentPlot() == mParentPlot)
        {
            position->setType(QCPItemPosition::ptPlotCoords);
            position->setAxes(bars->keyAxis(), bars->valueAxis());
            mBars = bars;
        }
        else
            qDebug() << Q_FUNC_INFO << "bars isn't in same QCustomPlot instance as this item";
    }else
    {
        mBars= 0;
    }
}

/*!
  Sets the key of the graph's data point the tracer will be positioned at. This is the only free
  coordinate of a tracer when attached to a graph.

  Depending on \ref setInterpolating, the tracer will be either positioned on the data point
  closest to \a key, or will stay exactly at \a key and interpolate the value linearly.

  \see setGraph, setInterpolating
*/
void QCPItemTracerBars::setGraphKey(double key)
{
    mGraphKey = key;
}

/*!
  Sets whether the value of the graph's data points shall be interpolated, when positioning the
  tracer.

  If \a enabled is set to false and a key is given with \ref setGraphKey, the tracer is placed on
  the data point of the graph which is closest to the key, but which is not necessarily exactly
  there. If \a enabled is true, the tracer will be positioned exactly at the specified key, and
  the appropriate value will be interpolated from the graph's data points linearly.

  \see setGraph, setGraphKey
*/
void QCPItemTracerBars::setInterpolating(bool enabled)
{
    mInterpolating = enabled;
}

/* inherits documentation from base class */
double QCPItemTracerBars::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(details)
    if (onlySelectable && !mSelectable)
        return -1;

    QPointF center(position->pixelPosition());
    double w = mSize/2.0;
    QRect clip = clipRect();
    switch (mStyle)
    {
    case tsNone: return -1;
    case tsPlus:
    {
        if (clipRect().intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
            return qSqrt(qMin(QCPVector2D(pos).distanceSquaredToLine(center+QPointF(-w, 0), center+QPointF(w, 0)),
                              QCPVector2D(pos).distanceSquaredToLine(center+QPointF(0, -w), center+QPointF(0, w))));
        break;
    }
    case tsCrosshair:
    {
        return qSqrt(qMin(QCPVector2D(pos).distanceSquaredToLine(QCPVector2D(clip.left(), center.y()), QCPVector2D(clip.right(), center.y())),
                          QCPVector2D(pos).distanceSquaredToLine(QCPVector2D(center.x(), clip.top()), QCPVector2D(center.x(), clip.bottom()))));
    }
    case tsCircle:
    {
        if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
        {
            // distance to border:
            double centerDist = QCPVector2D(center-pos).length();
            double circleLine = w;
            double result = qAbs(centerDist-circleLine);
            // filled ellipse, allow click inside to count as hit:
            if (result > mParentPlot->selectionTolerance()*0.99 && mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0)
            {
                if (centerDist <= circleLine)
                    result = mParentPlot->selectionTolerance()*0.99;
            }
            return result;
        }
        break;
    }
    case tsSquare:
    {
        if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
        {
            QRectF rect = QRectF(center-QPointF(w, w), center+QPointF(w, w));
            bool filledRect = mBrush.style() != Qt::NoBrush && mBrush.color().alpha() != 0;
            return rectDistance(rect, pos, filledRect);
        }
        break;
    }
    }
    return -1;
}

/* inherits documentation from base class */
void QCPItemTracerBars::draw(QCPPainter *painter)
{
    updatePosition();
    if (mStyle == tsNone)
        return;

    painter->setPen(mainPen());
    painter->setBrush(mainBrush());
    QPointF center(position->pixelPosition());
    double w = mSize/2.0;
    QRect clip = clipRect();
    switch (mStyle)
    {
    case tsNone: return;
    case tsPlus:
    {
        if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
        {
            painter->drawLine(QLineF(center+QPointF(-w, 0), center+QPointF(w, 0)));
            painter->drawLine(QLineF(center+QPointF(0, -w), center+QPointF(0, w)));
        }
        break;
    }
    case tsCrosshair:
    {
        if (center.y() > clip.top() && center.y() < clip.bottom())
            painter->drawLine(QLineF(clip.left(), center.y(), clip.right(), center.y()));
        if (center.x() > clip.left() && center.x() < clip.right())
            painter->drawLine(QLineF(center.x(), clip.top(), center.x(), clip.bottom()));
        break;
    }
    case tsCircle:
    {
        if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
            painter->drawEllipse(center, w, w);
        break;
    }
    case tsSquare:
    {
        if (clip.intersects(QRectF(center-QPointF(w, w), center+QPointF(w, w)).toRect()))
            painter->drawRect(QRectF(center-QPointF(w, w), center+QPointF(w, w)));
        break;
    }
    }
}

/*!
  If the tracer is connected with a graph (\ref setGraph), this function updates the tracer's \a
  position to reside on the graph data, depending on the configured key (\ref setGraphKey).

  It is called automatically on every redraw and normally doesn't need to be called manually. One
  exception is when you want to read the tracer coordinates via \a position and are not sure that
  the graph's data (or the tracer key with \ref setGraphKey) hasn't changed since the last redraw.
  In that situation, call this function before accessing \a position, to make sure you don't get
  out-of-date coordinates.

  If there is no graph set on this tracer, this function does nothing.
*/
void QCPItemTracerBars::updatePosition()
{
    if (mBars)
    {
        if (mParentPlot->hasPlottable(mBars))
        {
            if (mBars->data()->size() > 1)
            {
                QCPBarsDataContainer::const_iterator first = mBars->data()->constBegin();
                QCPBarsDataContainer::const_iterator last = mBars->data()->constEnd()-1;
                if (mGraphKey <= first->key)
                    position->setCoords(first->key, first->value);
                else if (mGraphKey >= last->key)
                    position->setCoords(last->key, last->value);
                else
                {
                    QCPBarsDataContainer::const_iterator it = mBars->data()->findBegin(mGraphKey);
                    if (it != mBars->data()->constEnd()) // mGraphKey is not exactly on last iterator, but somewhere between iterators
                    {
                        QCPBarsDataContainer::const_iterator prevIt = it;
                        ++it; // won't advance to constEnd because we handled that case (mGraphKey >= last->key) before
                        if (mInterpolating)
                        {
                            // interpolate between iterators around mGraphKey:
                            double slope = 0;
                            if (!qFuzzyCompare((double)it->key, (double)prevIt->key))
                                slope = (it->value-prevIt->value)/(it->key-prevIt->key);
                            position->setCoords(mGraphKey, (mGraphKey-prevIt->key)*slope+prevIt->value);
                        } else
                        {
                            // find iterator with key closest to mGraphKey:
                            if (mGraphKey < (prevIt->key+it->key)*0.5)
                                position->setCoords(prevIt->key, prevIt->value);
                            else
                                position->setCoords(it->key, it->value);
                        }
                    } else // mGraphKey is exactly on last iterator (should actually be caught when comparing first/last keys, but this is a failsafe for fp uncertainty)
                        position->setCoords(it->key, it->value);
                }
            } else if (mBars->data()->size() == 1)
            {
                QCPBarsDataContainer::const_iterator it = mBars->data()->constBegin();
                position->setCoords(it->key, it->value);
            } else
                qDebug() << Q_FUNC_INFO << "graph has no data";
        } else
            qDebug() << Q_FUNC_INFO << "graph not contained in QCustomPlot instance (anymore)";
    }
}

/*! \internal

  Returns the pen that should be used for drawing lines. Returns mPen when the item is not selected
  and mSelectedPen when it is.
*/
QPen QCPItemTracerBars::mainPen() const
{
    return mSelected ? mSelectedPen : mPen;
}

/*! \internal

  Returns the brush that should be used for drawing fills of the item. Returns mBrush when the item
  is not selected and mSelectedBrush when it is.
*/
QBrush QCPItemTracerBars::mainBrush() const
{
    return mSelected ? mSelectedBrush : mBrush;
}
/* end of 'src/items/item-tracer.cpp' */
