#ifndef QCPItemTracerColorMapBARS_H
#define QCPItemTracerColorMapBARS_H
#include "qcustomplot/qcustomplot.h"


class QCP_LIB_DECL QCPItemTracerColorMap : public QCPAbstractItem
{
    Q_OBJECT
    /// \cond INCLUDE_QPROPERTIES
    Q_PROPERTY(QPen pen READ pen WRITE setPen)
    Q_PROPERTY(QPen selectedPen READ selectedPen WRITE setSelectedPen)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
    Q_PROPERTY(QBrush selectedBrush READ selectedBrush WRITE setSelectedBrush)
    Q_PROPERTY(double size READ size WRITE setSize)
    Q_PROPERTY(TracerStyle style READ style WRITE setStyle)
    // Q_PROPERTY(QCPGraph* graph READ graph WRITE setGraph)
    //   Q_PROPERTY(QCPBars* bars READ bars WRITE setBars)
    Q_PROPERTY(QCPColorMap* colorMap READ colorMap WRITE setColorMap)
    Q_PROPERTY(double graphKey READ graphKey WRITE setGraphKey)
    Q_PROPERTY(bool interpolating READ interpolating WRITE setInterpolating)
    /// \endcond
public:
    /*!
    The different visual appearances a tracer item can have. Some styles size may be controlled with \ref setSize.

    \see setStyle
  */
    enum TracerStyle { tsNone        ///< The tracer is not visible
                       ,tsPlus       ///< A plus shaped crosshair with limited size
                       ,tsCrosshair  ///< A plus shaped crosshair which spans the complete axis rect
                       ,tsCircle     ///< A circle
                       ,tsSquare     ///< A square
                     };
    Q_ENUMS(TracerStyle)

    explicit QCPItemTracerColorMap(QCustomPlot *parentPlot);
    virtual ~QCPItemTracerColorMap();

    // getters:
    QPen pen() const { return mPen; }
    QPen selectedPen() const { return mSelectedPen; }
    QBrush brush() const { return mBrush; }
    QBrush selectedBrush() const { return mSelectedBrush; }
    double size() const { return mSize; }
    TracerStyle style() const { return mStyle; }
    //QCPGraph *graph() const { return mGraph; }
    QCPColorMap *colorMap() const { return mColorMap; }
    double graphKey() const { return mGraphKey; }
    bool interpolating() const { return mInterpolating; }

    // setters;
    void setPen(const QPen &pen);
    void setSelectedPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setSelectedBrush(const QBrush &brush);
    void setSize(double size);
    void setStyle(TracerStyle style);
    //void setGraph(QCPGraph *graph); //FUX
    //void setBars(QCPBars *bars); // FUX
    void setColorMap(QCPColorMap *colorMap); // FUX
    void setGraphKey(double key);
    void setGraphValue(double value);
    void setInterpolating(bool enabled);

    // reimplemented virtual methods:
    virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const Q_DECL_OVERRIDE;

    // non-virtual methods:
    void updatePosition();

    QCPItemPosition * const position;

protected:
    // property members:
    QPen mPen, mSelectedPen;
    QBrush mBrush, mSelectedBrush;
    double mSize;
    TracerStyle mStyle;
    //QCPGraph *mGraph;//
    //QCPBars *mBars; // FUX
    QCPColorMap *mColorMap;
    double mGraphKey;
    double mGraphValue;
    bool mInterpolating;

    // reimplemented virtual methods:
    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;

    // non-virtual methods:
    QPen mainPen() const;
    QBrush mainBrush() const;
};
Q_DECLARE_METATYPE(QCPItemTracerColorMap::TracerStyle)
#endif // QCPItemTracerColorMapBARS_H
