#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QScreen>
#include <QSurfaceFormat>

#include <glwindow.h>
#include <mainwindow.h>

int main(int argc, char* argv[])
{
    ads::CDockManager::setAutoHideConfigFlags(ads::CDockManager::DefaultAutoHideConfig);
    QApplication a(argc, argv);

    MainWindow* w = new MainWindow();
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();

    return a.exec();
}
