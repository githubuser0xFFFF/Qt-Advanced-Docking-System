#include <QMainWindow>
#include <QAction>
#include <QTextEdit>
#include <QApplication>
#include <QMenuBar>
#include "DockManager.h"


class MainWindow : public QMainWindow
{
private:
    ads::CDockManager* m_DockManager = nullptr;

protected:
    virtual void closeEvent(QCloseEvent *event) override
    {
        QMainWindow::closeEvent(event);
        if (m_DockManager)
        {
            m_DockManager->deleteLater();
        }
    }

public:
    void setDockManager(ads::CDockManager* DockManager) {m_DockManager = DockManager;}
};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AllTabsHaveCloseButton, true);
    auto dockManager = new ads::CDockManager(&w);
    w.setDockManager(dockManager);
    QObject::connect(dockManager, &ads::CDockManager::focusedDockWidgetChanged, [] (ads::CDockWidget* old, ads::CDockWidget* now) {
        static int Count = 0;
    	qDebug() << Count++ << " CDockManager::focusedDockWidgetChanged old: " << (old ? old->objectName() : "-") << " now: " << now->objectName() << " visible: " << now->isVisible();
        now->widget()->setFocus();
    });

    QAction *action = new QAction("New [DockWidgetDeleteOnClose]", &w);
    w.menuBar()->addAction(action);

    int i = 0;
    QObject::connect(action, &QAction::triggered, [&]() {
        auto dw = new ads::CDockWidget(QStringLiteral("test %1 [DockWidgetDeleteOnClose]").arg(i++), &w);
        auto editor = new QTextEdit(QStringLiteral("lorem ipsum..."), dw);
        dw->setWidget(editor);
        dw->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
        auto area = dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dw);
        qDebug() << "doc dock widget created!" << dw << area;
    });
	
	auto dw = new ads::CDockWidget(QStringLiteral("test %1 [DeleteContentOnClose]").arg(i++), &w);
	auto editor = new QTextEdit(QStringLiteral("recreated lorem ipsum......"), dw);
	dw->setWidget(editor);
	dw->setFeature(ads::CDockWidget::DeleteContentOnClose, true);
	dw->setWidgetFactory([](QWidget* dw)
	{
		static int timesRecreated = 0;
		return new QTextEdit(QStringLiteral("recreated lorem ipsum... times %1").arg(++timesRecreated), dw);
	});
	auto area = dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dw);
	qDebug() << "DeleteContentOnClose dock widget created!" << dw << area;
	
	action = new QAction("Toggle [DeleteContentOnClose]", &w);
    w.menuBar()->addAction(action);
	
	QObject::connect(action, &QAction::triggered, [dw]() {
		dw->toggleView(dw->isClosed());
		qDebug() << QString("dock widget %1! contents widget %2!").arg(dw->isClosed() ? "closed" : "open", dw->widget() ? "created" : "deleted");
    });
	
    action = new QAction("New", &w);
    w.menuBar()->addAction(action);
    QObject::connect(action, &QAction::triggered, [&]() {
        auto dw = new ads::CDockWidget(QStringLiteral("test %1").arg(i++), &w);
        auto editor = new QTextEdit(QStringLiteral("lorem ipsum..."), dw);
        dw->setWidget(editor);
        auto area = dockManager->addDockWidgetTab(ads::CenterDockWidgetArea, dw);
        qDebug() << "dock widget created!" << dw << area;
    });

    w.show();

    return a.exec();
}
