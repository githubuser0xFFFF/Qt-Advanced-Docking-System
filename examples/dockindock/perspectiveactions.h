#pragma once

#include <QAction>

namespace QtAdsUtl
{

class DockInDockWidget;
class LoadPerspectiveAction : public QAction
{
    Q_OBJECT
public:
    LoadPerspectiveAction( QMenu* parent, const QString& name, QtAdsUtl::DockInDockWidget& dockManager );

public slots:
    void load();

private:
    QString name;
    QtAdsUtl::DockInDockWidget& dockManager;
};

class RemovePerspectiveAction : public QAction
{
    Q_OBJECT
public:
    RemovePerspectiveAction( QMenu* parent, const QString& name, QtAdsUtl::DockInDockWidget& dockManager );

public slots:
    void remove();

private:
    QString name;
    QtAdsUtl::DockInDockWidget& dockManager;
};

}

