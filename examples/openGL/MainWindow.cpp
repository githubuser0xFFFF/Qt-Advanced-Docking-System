#include "MainWindow.h"

#include <QChart>
#include <QChartView>
#include <QLabel>
#include <QLineSeries>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setFixedSize(900, 600);
    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    m_DockManager = new ads::CDockManager(this);

    // Create example chart using OpenGL
    QChartView* chart_view = new QChartView(this);
    QList<QPointF> points = {{0, 0}, {2, 0}, {2, 5}, {4, 5}, {4, -2}, {6, -2}};
    QLineSeries* series = new QLineSeries(this);
    series->setUseOpenGL(true);
    series->replace(points);
    chart_view->chart()->addSeries(series);
    chart_view->chart()->createDefaultAxes();
    chart_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // Create a dock widget with the title "Chart with OpenGL" and set the created
    // chart as the dock widget content
    ads::CDockWidget* DockWidget = new ads::CDockWidget("Chart with OpenGL");
    DockWidget->setWidget(chart_view);
    DockWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    // Add the dock widget to the top dock widget area
    m_DockManager->addDockWidget(ads::CenterDockWidgetArea, DockWidget);
}
