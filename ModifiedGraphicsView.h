#pragma once
#include <QtWidgets/QGraphicsView>
#include <QMouseEvent>
#include "MainWindow.h"

class ModifiedGraphicsView : public QGraphicsView
{
public:
    ModifiedGraphicsView(QWidget* parent = nullptr, MainWindow* mainWindow = nullptr);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QPoint m_lastPanPos;
    MainWindow* m_mainWindow;
};