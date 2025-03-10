#include "mainwindow.h"

#include <QApplication>
#include <QChart>
#include <QChartView>
#include <QComboBox>
#include <QLabel>
#include <QLineSeries>
#include <QOpenGLWidget>
#include <QPushButton>
#include <QQuickWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <glwidget.h>
#include <glwindow.h>

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

class OpenGLWidgetContainer : public QWidget
{
public:
    OpenGLWidgetContainer(QWidget* parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_NativeWindow, true);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);

        QWidget* widget = QWidget::createWindowContainer(new GLWindow, parent);
        layout->addWidget(widget);
    }

    ~OpenGLWidgetContainer() {}
};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    resize(900, 600);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    _dock_manager = new ads::CDockManager(this);

    OpenGLChartWidget* openGL_chart = new OpenGLChartWidget(this);
    OpenGLWidgetContainer* openGL_container = new OpenGLWidgetContainer(this);
    GLWidget* gl_widget = new GLWidget(this);
    GLWidget* gl_widget_2 = new GLWidget(this);

    // Create a dock widget with the title "Chart with OpenGL" and set the created
    // chart as the dock widget content
    ads::CDockWidget* opengl_chart_dock_widget =
        new ads::CDockWidget("Chart with OpenGL");
    opengl_chart_dock_widget->setWidget(openGL_chart);
    auto* area = _dock_manager->addDockWidget(ads::CenterDockWidgetArea,
                                              opengl_chart_dock_widget);

    ads::CDockWidget* chart_dock_widget = new ads::CDockWidget("Simple Chart");
    chart_dock_widget->setWidget(new ChartWidget(this));
    _dock_manager->addDockWidgetTabToArea(chart_dock_widget, area);

    ads::CDockWidget* openGL_window_dock_widget =
        new ads::CDockWidget("OpenGL window");
    openGL_window_dock_widget->setWidget(openGL_container);
    _dock_manager->addDockWidgetTabToArea(openGL_window_dock_widget, area);

    ads::CDockWidget* openGL_widget_dock_widget =
        new ads::CDockWidget("OpenGL widget");
    openGL_widget_dock_widget->setWidget(gl_widget);
    _dock_manager->addDockWidgetTabToArea(openGL_widget_dock_widget, area);

    ads::CDockWidget* openGL_widget_dock_widget_2 =
        new ads::CDockWidget("OpenGL widget 2");
    openGL_widget_dock_widget_2->setWidget(gl_widget_2);
    _dock_manager->addDockWidgetTabToArea(openGL_widget_dock_widget_2, area);

    ads::CDockWidget* label_dock_widget = new ads::CDockWidget("Label");

    QLabel* l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

    label_dock_widget->setWidget(l);
    _dock_manager->addDockWidgetTabToArea(label_dock_widget, area);
}