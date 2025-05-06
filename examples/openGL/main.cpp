#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QScreen>
#include <QQuickWindow>

#include <glwindow.h>
#include <mainwindow.h>


int main(int argc, char* argv[])
{
    // https://doc.qt.io/qt-6/qtdatavisualization-known-issues.html
    // Use either `qputenv("QSG_RHI_BACKEND", "opengl");` or the following line
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    // Disable warnings when attempts are made to convert non-convertible non-native widgets
    // to native widgets (such as QQuickWidget)
    QApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    // Enable Native widget AutoHide
    ads::CDockManager::setConfigFlags(
        ads::CDockManager::UseNativeWidgets);

    // Enable ADS AutoHide
    ads::CDockManager::setAutoHideConfigFlags(ads::CDockManager::DefaultAutoHideConfig);

    QApplication a(argc, argv);

    MainWindow* w = new MainWindow();
    
    // Release memory when closing main window and quit application
    w->setAttribute(Qt::WA_DeleteOnClose);

    w->show();

    return a.exec();
}
