#include "qfgraph.h"


QFGraph::QFGraph(QCustomPlot *m_cPlot, QCPColorMap *m_colorMatp, QWidget *parent) : QWidget(parent)
{
    graphTypes=3;
    mColorMap = m_colorMatp;
    graphDataColorMap = m_colorMatp->data();
    init(m_cPlot);
}

QFGraph::QFGraph(QCustomPlot *m_cPlot,QCPBars *m_bars, QWidget *parent) : QWidget(parent)
{
    graphTypes=2;
    mBars  = m_bars;
    graphDataBars = m_bars->data(); //data(key,value)

    init(m_cPlot);

}

QFGraph::QFGraph(QCustomPlot *m_cPlot, QCPGraph *m_graph, QWidget *parent) : QWidget(parent)
{
    mGraph = m_graph;
    graphTypes=1;;
    graphData =  m_graph->data();

    init(m_cPlot);
}

void QFGraph::init(QCustomPlot *m_cPlot)
{
    cPlot = m_cPlot;

    /*
    const QCPDataMap *dataMap = ui->customPlot->selectedGraphs().first()->data();

    QMap<double, QCPData>::const_iterator i = dataMap->constBegin();
    while (i != dataMap->constEnd()) {
        qDebug() << i.key()<< ": " << i.value().key << ": " << i.value().value << endl;
        ++i;
    }
*/
    /*Context Menu*/
    cPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect (cPlot, SIGNAL(customContextMenuRequested(QPoint)), this,SLOT( onCustomContextMenuRequested(QPoint)));

    /* ToolBAR */
    m_toolBar = new QToolBar("Graph ToolBar");


    QAction *interactionRectZoomAction = new QAction;
    interactionRectZoomAction->setText("Selection Zoom");
    interactionRectZoomAction->setIcon(QIcon(":/icons/iconSelectZoom.png"));
    interactionRectZoomAction->setCheckable(true);
    interactionRectZoomAction->setChecked(false);

    QAction *interactionDragAxisAction = new QAction;
    interactionDragAxisAction->setText("Drag Axis");
    interactionDragAxisAction->setIcon(QIcon(":/icons/iconDragAxe.png"));
    interactionDragAxisAction->setCheckable(true);
    interactionDragAxisAction->setChecked(false);

    QAction *interactionMouseWheelZoomAction = new QAction;
    interactionMouseWheelZoomAction->setText("Scroll Zoom");
    interactionMouseWheelZoomAction->setIcon(QIcon(":/icons/iconScrollZoom.png"));
    interactionMouseWheelZoomAction->setCheckable(true);
    interactionMouseWheelZoomAction->setChecked(false);

    QAction *interactionDefaultAxis = new QAction;
    interactionDefaultAxis->setText("Default view");
    interactionDefaultAxis->setIcon(QIcon(":/icons/iconDefaultSize.png"));
    interactionDefaultAxis->setCheckable(false);

    QAction *interactionXYPoints = new QAction;
    interactionXYPoints->setText("Display XY value");
    interactionXYPoints->setIcon(QIcon(":/icons/iconXY.png"));
    interactionXYPoints->setCheckable(true);
    interactionXYPoints->setChecked(false);

    /*   QAction *interactionMoveAnalyseRect = new QAction;
    interactionMoveAnalyseRect->setText("Moce Selection Rectangle");
    interactionMoveAnalyseRect->setIcon(QIcon(":/icons/iconMoceRect.png"));
    interactionMoveAnalyseRect->setCheckable(true);
    interactionMoveAnalyseRect->setChecked(false);*/

    QAction *interactionAutoZoom = new QAction;
    interactionAutoZoom->setText("Auto Y Zoom");
    interactionAutoZoom->setIcon(QIcon(":/icons/iconAutoZoom.png"));
    interactionAutoZoom->setCheckable(false);
    interactionAutoZoom->setChecked(false);

    m_toolBar->addAction(interactionRectZoomAction);
    m_toolBar->addAction(interactionDragAxisAction);
    m_toolBar->addAction(interactionMouseWheelZoomAction);
    m_toolBar->addAction(interactionDefaultAxis);
    m_toolBar->addAction(interactionXYPoints);
    //   m_toolBar->addAction(interactionMoveAnalyseRect);
    m_toolBar->addAction(interactionAutoZoom);

    /* Conneections*/

    connect(interactionRectZoomAction, SIGNAL(toggled(bool)), this,SLOT(onInteractionRectZoomAction(bool)));
    connect(interactionDragAxisAction, SIGNAL(toggled(bool)), this,SLOT(onInteractionDragAxisAction(bool)));
    connect(interactionMouseWheelZoomAction, SIGNAL(toggled(bool)), this,SLOT(onInteractionMouseWheelZoomAction(bool)));
    connect(interactionDefaultAxis, SIGNAL(triggered(bool)), this,SLOT(onInteractionDefaultAxis(bool)));
    connect(interactionXYPoints, SIGNAL(triggered(bool)), this,SLOT(onInteractionXYPoints(bool)));
    connect(interactionAutoZoom, SIGNAL(triggered(bool)), this,SLOT(onInteractionAutoZoom(bool)));

    cPlot->setInteractions(QCP::iSelectAxes);
    connect(cPlot,SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),this,SLOT(onAxisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)));


    connect(cPlot,SIGNAL(beforeReplot()),this,SLOT(updateTracerText()));

    layout = new QGridLayout;
    layout->addWidget(m_toolBar);
    layout->addWidget(cPlot);
    setLayout(layout);
}

void QFGraph::setDefaultXYRange(QCPRange x, QCPRange y)
{
    defaultX = x;
    defaultY = y;
}

void QFGraph::onCustomContextMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(cPlot);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    contextPos = pos;
    menu->addAction("Resize X-Axis", this, SLOT(onXAxisResize()));
    menu->addAction("Resize Y-Axis", this, SLOT(onYAxisResize()));
    menu->addAction("Export Data", this, SLOT(onExportData()));
    menu->addAction("Reset Axis", this, SLOT(onResetAxis()));
    menu->popup(cPlot->mapToGlobal(pos));
}

void QFGraph::onXAxisResize()
{
    resizeAxisDialog(cPlot->xAxis, contextPos);
}

void QFGraph::onYAxisResize()
{
    resizeAxisDialog(cPlot->yAxis, contextPos);
}

void QFGraph::resizeAxisDialog(QCPAxis * m_axis, QPoint pos)
{
    // Create Dialog
    XYRangeDialog *dialog = new XYRangeDialog(this,m_axis->range().lower,m_axis->range().upper);
    // Place the dialog box near the mouse pointer
    dialog->move(pos);
    // Process when OK button is clicked
    if (dialog->exec() == QDialog::Accepted) {
        m_axis->setRange(dialog->val1, dialog->val2);
        cPlot->replot();
    }
}

void QFGraph::onExportData()
{
    emit exportData();
}

void QFGraph::onResetAxis()
{
    cPlot->xAxis->setRange(defaultX);
    cPlot->yAxis->setRange(defaultY);
    cPlot->replot();
}

void QFGraph::onInteractionRectZoomAction(bool isChecked)
{
    if (isChecked)
    {
        cPlot->setSelectionRectMode(QCP::srmZoom);
        cPlot->setSelectionRectMode(QCP::srmZoom);
    }
    else
    {
        cPlot->setSelectionRectMode(QCP::srmNone);
        cPlot->setSelectionRectMode(QCP::srmNone);
    }
}

void QFGraph::onInteractionDragAxisAction(bool isChecked)
{
    if (isChecked)
    {
        cPlot->setInteraction(QCP::iRangeDrag,true);
        cPlot->setInteraction(QCP::iRangeDrag,true);
    }
    else
    {
        cPlot->setInteraction(QCP::iRangeDrag,false);
        cPlot->setInteraction(QCP::iRangeDrag,false);
    }
}

void QFGraph::onInteractionMouseWheelZoomAction(bool isChecked)
{
    if (isChecked)
    {
        cPlot->setInteraction(QCP::iRangeZoom,true);
        cPlot->setInteraction(QCP::iRangeZoom,true);
    }
    else
    {
        cPlot->setInteraction(QCP::iRangeZoom,false);
        cPlot->setInteraction(QCP::iRangeZoom,false);
    }
}

void QFGraph::onInteractionDefaultAxis(bool)
{
    onResetAxis();
}

void QFGraph::onInteractionXYPoints(bool isChecked)
{

    isXYValueMode = isChecked;

    if (isChecked)
    {

        //![] ITEM TRACER
        // add the phase tracer (red circle) which sticks to the graph data (and gets updated in bracketDataSlot by timer event):

        // add label for phase tracer:
        phaseTracerText = new QCPItemText(cPlot);
        phaseTracerText->setPositionAlignment(Qt::AlignLeft|Qt::AlignBottom);

        phaseTracerText->setTextAlignment(Qt::AlignLeft);
        QFont t;
        t.setPixelSize(12);
        t.setBold(true);
        phaseTracerText->setFont(t);
        phaseTracerText->setPadding(QMargins(10, 0, 10, 0));
        phaseTracerText->setBrush(QColor::fromRgbF(0.5,0.5,0.5,0.7));
        connect(cPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(onMousePress(QMouseEvent*)));

        // Move tracer with arrow
        sCArrowsL = new QShortcut(this);
        sCArrowsL->setKey( Qt::Key_Left);
        connect(sCArrowsL, SIGNAL(activated()), this, SLOT(onKeyLPress()));
        sCArrowsR = new QShortcut(this);
        sCArrowsR->setKey(Qt::Key_Right);
        connect(sCArrowsR, SIGNAL(activated()),this, SLOT(onKeyRPress()));


        xValues.clear();
        switch (graphTypes)
        {
        case 1 :
            phaseTracer = new QCPItemTracer(cPlot);
            phaseTracer->setGraph(mGraph);
            phaseTracer->setInterpolating(false); // To exactly match the graph data
            phaseTracer->setStyle(QCPItemTracer::tsCircle);
            phaseTracer->setPen(QPen(Qt::red));
            phaseTracer->setBrush(Qt::red);
            phaseTracer->setSize(7);
            phaseTracerText->position->setParentAnchor(phaseTracer->position); // lower right corner of axis rect
            phaseTracerText->setText(QString("(%1,%2)").arg(phaseTracer->position->key()).arg(phaseTracer->position->value()));

            for (int i=0; i< graphData.data()->size();i++)
                xValues.append(graphData.data()->at(i)->key);

            break;
        case 2:
            phaseTracerBars = new QCPItemTracerBars(cPlot);
            phaseTracerBars->setBars(mBars);
            phaseTracerBars->setInterpolating(false); // To exactly match the graph data
            phaseTracerBars->setStyle(QCPItemTracerBars::tsCircle);
            phaseTracerBars->setPen(QPen(Qt::red));
            phaseTracerBars->setBrush(Qt::red);
            phaseTracerBars->setSize(7);
            phaseTracerText->position->setParentAnchor(phaseTracerBars->position); // lower right corner of axis rect
            phaseTracerText->setText(QString("(%1,%2)").arg(phaseTracerBars->position->key()).arg(phaseTracerBars->position->value()));

            for (int i=0; i< graphDataBars.data()->size();i++)
                xValues.append(graphDataBars.data()->at(i)->key);
            break;


        case 3:
            phaseTracerColorMap = new QCPItemTracerColorMap(cPlot);

            phaseTracerColorMap->setColorMap(mColorMap);
            phaseTracerColorMap->setInterpolating(false); // To exactly match the graph data
            phaseTracerColorMap->setStyle(QCPItemTracerColorMap::tsCircle);
            phaseTracerColorMap->setPen(QPen(Qt::red));
            phaseTracerColorMap->setBrush(Qt::red);
            phaseTracerColorMap->setSize(7);
            phaseTracerText->position->setParentAnchor(phaseTracerColorMap->position); // lower right corner of axis rect

            double firstKey;
            firstKey = mColorMap->data()->keyRange().lower;
            double  lastKey;
            lastKey = mColorMap->data()->keyRange().upper;

            for (int j=0; j<mColorMap->data()->keySize();j++)
            {
                xValues.append((1.0*j)/(mColorMap->data()->keySize()-1) * (lastKey-firstKey) + firstKey);
            }
            break;
        }

        SetTracerAt(xValues.at(round(xValues.size()/2)));
    }
    else
    {

        disconnect(cPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(onMousePress(QMouseEvent*)));
        disconnect(sCArrowsL, SIGNAL(activated()), this, SLOT(onKeyLPress()));
        disconnect(sCArrowsR, SIGNAL(activated()), this, SLOT(onKeyRPress()));

        switch (graphTypes)
        {
        case 1 :
            delete phaseTracer;
            break;
        case 2:
            delete phaseTracerBars;
            break;
        case 3:
            delete phaseTracerColorMap;
            break;
        }



        delete phaseTracerText;
        delete sCArrowsL;
        delete sCArrowsR;

        //phaseTracerText->setVisible(false);
        cPlot->replot();
    }
}


void QFGraph::onMousePress(QMouseEvent* event)
{
    if (event->buttons()==Qt::LeftButton)
    {
        double x,y;
        switch (graphTypes)
        {
        case 1 :
            mGraph->pixelsToCoords(event->pos().x(),event->pos().y(),x,y); // get mouse cursor
            SetTracerAt(x);
            break;
        case 2:
            mBars->pixelsToCoords(event->pos().x(),event->pos().y(),x,y); // get mouse cursor
            SetTracerAt(x);
            break;
        case 3:
            mColorMap->pixelsToCoords(event->pos().x(),event->pos().y(),x,y); // get mouse cursor
            SetTracerAt(x,y);
            break;
        }
    }
}

void QFGraph::updateTracerText()
{


    if(isXYValueMode)
    {
        switch (graphTypes)
        {
        case 1 :
            phaseTracerText->setText(QString("(freq: %1, Mag: %2)").arg(phaseTracer->position->key()).arg(phaseTracer->position->value()));
            break;
        case 2:
            phaseTracerText->setText(QString("(freq: %1, Mag: %2)").arg(phaseTracerBars->position->key()).arg(phaseTracerBars->position->value()));
            break;
        case 3:
            phaseTracerText->setText(QString("(time: %1, freq: %2, Mag: %3)").arg(phaseTracerColorMap->position->key()).arg(phaseTracerColorMap->position->value()).arg(graphDataColorMap->data(phaseTracerColorMap->position->key(),phaseTracerColorMap->position->value())));
            break;
        }


        cPlot->replot(); // TODO : Only if not LiveView !!!!
    }
}

void QFGraph::onKeyLPress()
{
    int idx;
    switch (graphTypes)
    {
    case 1 :
        idx=xValues.indexOf(phaseTracer->position->key());
        break;
    case 2:
        idx=xValues.indexOf(phaseTracerBars->position->key());
    case 3:
        //   idx=xValues.indexOf(phaseTracerColorMap->position->key());
        break;
    }

    //qDebug("%d", idx);
    if ((idx-1) < 1) //FIXME: Why is not ZERO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        idx =1;//FIXME: Why is not ZERO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    SetTracerAt(xValues.at(idx-1));
}

void QFGraph::onKeyRPress()
{
    int idx;
    switch (graphTypes)
    {
    case 1 :
        idx=xValues.indexOf(phaseTracer->position->key());
        break;
    case 2:
        idx=xValues.indexOf(phaseTracerBars->position->key());
        break;
    case 3:
        // idx=xValues.indexOf(phaseTracerColorMap->position->key());
        break;
    }
    //qDebug("%d", idx);
    if ((idx+1) > xValues.size()-2) //FIXME: Why is not freqValues.size()-1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        idx =xValues.size()-2;//FIXME: Why is not freqValues.size()-1 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    SetTracerAt(xValues.at(idx+1));
}

void QFGraph::SetTracerAt(double x)
{
    switch (graphTypes)
    {
    case 1 :
        phaseTracer->setGraphKey(x);
        phaseTracer->updatePosition(); // unstead what the position values are not correct
        phaseTracerText->setText(QString("(freq: %1, Mag: %2)").arg(phaseTracer->position->key()).arg(phaseTracer->position->value()));
        break;
    case 2:
        phaseTracerBars->setGraphKey(x);
        phaseTracerBars->updatePosition(); // unstead what the position values are not correct
        phaseTracerText->setText(QString("(freq: %1, Mag: %2)").arg(phaseTracerBars->position->key()).arg(phaseTracerBars->position->value()));
        break;
    case 3:
        phaseTracerColorMap->setGraphKey(x);
        phaseTracerColorMap->updatePosition(); // unstead what the position values are not correct
        phaseTracerText->setText(QString("(time: %1, freq: %2, Mag: %3)").arg(phaseTracerColorMap->position->key()).arg(phaseTracerColorMap->position->value()).arg(graphDataColorMap->data(phaseTracerColorMap->position->key(),phaseTracerColorMap->position->value())));
        break;
    }


    cPlot->replot(); // TODO : Only if not LiveView !!!!
}


void QFGraph::SetTracerAt(double x, double y)
{
    switch (graphTypes)
    {
    case 3:
        phaseTracerColorMap->setGraphKey(x);
        phaseTracerColorMap->setGraphValue(y);
        phaseTracerColorMap->updatePosition(); // unstead what the position values are not correct
        phaseTracerText->setText(QString("(time: %1, freq: %2, Mag: %3)").arg(phaseTracerColorMap->position->key()).arg(phaseTracerColorMap->position->value()).arg(graphDataColorMap->data(phaseTracerColorMap->position->key(),phaseTracerColorMap->position->value())));
        break;
    }


    cPlot->replot(); // TODO : Only if not LiveView !!!!
}


void  QFGraph::onAxisDoubleClick(QCPAxis* m_axis,QCPAxis::SelectablePart ,QMouseEvent* event)
{
    QPoint posInt;
    posInt.setX(round(event->screenPos().x()));
    posInt.setY(round(event->screenPos().y()));
    resizeAxisDialog(m_axis, posInt);
}


void QFGraph::onInteractionAutoZoom(bool)
{
    cPlot->rescaleAxes();
    cPlot->replot();
}


void QFGraph::setToolBarVisible(bool t)
{
    isToolBarVisible = t;
    if (isToolBarVisible)
    {
        layout->addWidget(m_toolBar);
    }else
    {
        layout->removeWidget(m_toolBar);
    }
}

bool QFGraph::ToolBarVisible()
{
    return isToolBarVisible;
}
