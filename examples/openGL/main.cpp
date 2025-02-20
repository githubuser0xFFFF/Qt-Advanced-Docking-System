#include <QApplication>

#include <MainWindow.h>

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);  // required for
                                                             // dockings system
                                                             // when using OpenGL
                                                             // widgets like
                                                             // Coin3D viewer
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
