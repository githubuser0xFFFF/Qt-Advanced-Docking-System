#include <windows.h>

#include <MainWindow.h>
#include <QString>
#include <QFile>
#include <QApplication>
#include <QDebug>

#include <memory>



void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stdout, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }

    fflush(stderr);
    fflush(stdout);
}

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	std::shared_ptr<int> b;
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(true);
	qInstallMessageHandler(myMessageOutput);
	qDebug() << "Message handler test";

	CMainWindow mw;
	mw.show();
	return a.exec();
}
