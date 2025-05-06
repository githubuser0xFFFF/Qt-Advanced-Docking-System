#include "mainwindow.h"

#include <QApplication>
#include <QChart>
#include <QChartView>
#include <QComboBox>
#include <QLabel>
#include <QLineSeries>
#include <QOpenGLWidget>
#include <QPushButton>
#include <QQuickView>
#include <QQuickWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <glwidget.h>
#include <glwindow.h>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

class OpenGLChartWidget : public QWidget
{
public:
    OpenGLChartWidget(QWidget* parent) : QWidget(parent)
    {
        // Create example chart using OpenGL
        QVBoxLayout* layout = new QVBoxLayout(this);
        QChartView* chart_view = new QChartView(this);
        QLineSeries* series = new QLineSeries(this);

        QList<QPointF> points = {{0, 0}, {2, 0},  {2, 5},
                                 {4, 5}, {4, -2}, {6, -2}};
        series->setUseOpenGL(true);
        series->replace(points);
        chart_view->chart()->addSeries(series);
        chart_view->chart()->createDefaultAxes();
        chart_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        layout->addWidget(chart_view);
    }
    ~OpenGLChartWidget() {}
};

class ChartWidget : public QWidget
{
public:
    ChartWidget(QWidget* parent) : QWidget(parent)
    {
        // Create example chart using OpenGL
        QVBoxLayout* layout = new QVBoxLayout(this);
        QChartView* chart_view = new QChartView(this);
        QLineSeries* series = new QLineSeries(this);

        QList<QPointF> points = {{0, 0},  {2, 0}, {2, -5},
                                 {4, -5}, {4, 2}, {6, 2}};
        series->replace(points);
        chart_view->chart()->addSeries(series);
        chart_view->chart()->createDefaultAxes();
        chart_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        layout->addWidget(chart_view);
    }
    ~ChartWidget() {}
};

class OpenGLWindowContainer : public QWidget
{
public:
    OpenGLWindowContainer(QWidget* parent = nullptr) : QWidget(parent)
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        QWidget* widget = QWidget::createWindowContainer(new GLWindow, parent);
        layout->addWidget(widget);
    }

    ~OpenGLWindowContainer() {}
};

class QuickViewContainer : public QWidget
{
public:
    QuickViewContainer(QWidget* parent = nullptr) : QWidget(parent)
    {
        QVBoxLayout* l = new QVBoxLayout(this);
        l->setContentsMargins(0, 0, 0, 0);

        _format.setDepthBufferSize(16);
        _format.setStencilBufferSize(8);
        _format.setSamples(4);

        QUrl source("qrc:/openGL/test.qml");

        QQuickView* quick_view = new QQuickView();
        quick_view->setFormat(_format);
        quick_view->setResizeMode(QQuickView::SizeRootObjectToView);
        quick_view->setSource(source);

        if (quick_view->status() != QQuickView::Ready)
        {
            qWarning() << "QQuickView error:" << quick_view->errors();
        }

        l->addWidget(QWidget::createWindowContainer(quick_view, parent));
    }

    ~QuickViewContainer() {}

private:
    QSurfaceFormat _format;
};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    resize(900, 600);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    _dock_manager = new ads::CDockManager(this);

    OpenGLChartWidget* openGL_chart = new OpenGLChartWidget(this);
    ChartWidget* simple_chart = new ChartWidget(this);
    OpenGLWindowContainer* openGL_container = new OpenGLWindowContainer(this);
    MyGLWidget* gl_widget = new MyGLWidget(this);
    MyGLWidget* gl_widget_2 = new MyGLWidget(this);
    QuickViewContainer* quick_view_container = new QuickViewContainer(this);

    QLabel* l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

    QUrl source("qrc:/openGL/test.qml");

    QQuickWidget* quick_widget = new QQuickWidget(this);
    quick_widget->setSource(source);
    quick_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    auto* center_dock_area_widget = createDockWidget(
        openGL_chart, "Chart with OpenGL", ads::CenterDockWidgetArea);

    createDockWidget(simple_chart, "Simple Chart", ads::CenterDockWidgetArea,
                     center_dock_area_widget);

    createDockWidget(openGL_container, "OpenGL window", ads::CenterDockWidgetArea,
                     center_dock_area_widget);

    createDockWidget(gl_widget, "OpenGL widget", ads::CenterDockWidgetArea,
                     center_dock_area_widget);

    createDockWidget(gl_widget_2, "OpenGL widget 2", ads::CenterDockWidgetArea,
                     center_dock_area_widget);

    createDockWidget(quick_view_container, "Quick View",
                     ads::CenterDockWidgetArea, center_dock_area_widget);

    createDockWidget(quick_widget, "Quick Widget", ads::CenterDockWidgetArea,
                     center_dock_area_widget);

    createDockWidget(l, "Label", ads::CenterDockWidgetArea,
                     center_dock_area_widget);
}

ads::CDockAreaWidget* MainWindow::createDockWidget(
    QWidget* embedded_widget, QString dock_widget_title, ads::DockWidgetArea area,
    ads::CDockAreaWidget* center_dock_area_widget)
{
    ads::CDockWidget* dock_widget = new ads::CDockWidget(dock_widget_title);
    dock_widget->setWidget(embedded_widget);
    return _dock_manager->addDockWidget(area, dock_widget,
                                        center_dock_area_widget);
}
