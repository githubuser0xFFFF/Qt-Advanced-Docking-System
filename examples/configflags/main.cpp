#include "createmainviewwindow.h"

#include <QApplication>
#include <QSize>
#include <Qt>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#  if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
   QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#  endif

#  if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
   QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
   QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#  endif
#endif

    QApplication a(argc, argv);
    CreateMainViewWindow w;
    w.show();
    return a.exec();
}
