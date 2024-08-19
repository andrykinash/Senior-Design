#include "ModifiedGraphicsView.h"
#include <QDebug>

ModifiedGraphicsView::ModifiedGraphicsView(QWidget* parent, MainWindow* mainWindow)
    : QGraphicsView(parent), m_mainWindow(mainWindow)
{
    setDragMode(QGraphicsView::ScrollHandDrag); // set drag mode to scroll hand drag
    setRenderHint(QPainter::Antialiasing); // set anti-aliasing for smoother rendering
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true); // disable adjustment for antialiasing
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate); // set viewport update mode to smart update
}

void ModifiedGraphicsView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        QPointF pos = mapToScene(event->pos());
        qDebug() << "Right-clicked at position: " << pos;
        ez_point p;

        p.x = pos.x();
        p.y = pos.y();

        m_mainWindow->DrawRightClickArea(p);
        m_mainWindow->mousePressEvent(event, p);

    }
    if (event->button() == Qt::LeftButton) {
        QPointF pos = mapToScene(event->pos());
        qDebug() << "Left-clicked at position: " << pos;
        ez_point p;

        p.x = pos.x();
        p.y = pos.y();

        m_lastPanPos = event->pos();
        m_mainWindow->mousePressEvent(event, p);

    }

    // Call the base class method to handle other mouse events
    QGraphicsView::mousePressEvent(event);
}

void ModifiedGraphicsView::wheelEvent(QWheelEvent* event)
{
    // zoom in/out based on wheel event
    const QPointF pointBefore = mapToScene(event->pos());
    const double scaleFactor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
    scale(scaleFactor, scaleFactor);
    const QPointF pointAfter = mapToScene(event->pos());
    const QPointF offset = pointBefore - pointAfter;
    translate(offset.x(), offset.y());
}

void ModifiedGraphicsView::mouseMoveEvent(QMouseEvent* event)
{

    if (event->buttons() & Qt::LeftButton) {

        const QPointF delta = mapToScene(event->pos()) - mapToScene(m_lastPanPos);
        translate(delta.x(), delta.y());
        m_lastPanPos = event->pos();


    }
    QGraphicsView::mouseMoveEvent(event);
}

void ModifiedGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

