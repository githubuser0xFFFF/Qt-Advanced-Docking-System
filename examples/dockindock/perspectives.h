#pragma once

#include <QObject>
#include <QMap>
#include <QString>

#include <memory>

class QMenu;
class QSettings;

namespace QtAdsUtl
{

class DockInDockWidget;
class PerspectivesManager : public QObject
{
    Q_OBJECT

public:
    PerspectivesManager( const QString& perspectivesFolder );
    virtual ~PerspectivesManager();

    QStringList perspectiveNames() const;

    void addPerspective( const QString& name, DockInDockWidget& widget );
    void openPerspective( const QString& name, DockInDockWidget& widget );
    void removePerspectives();
    void removePerspective( const QString& name );

    void loadPerspectives();
    void savePerspectives() const;

signals:
    void perspectivesListChanged();
    void openingPerspective();
    void openedPerspective();

private:

    // Partially bypass ADS perspective management, store list here
    // and then ADS will only have one perspective loaded
    // this is because all docking widgets must exist when a perspective is loaded
    // we will guarantee that!
    class PerspectiveInfo
    {
    public:
        std::shared_ptr<QSettings> settings;
        QMap<QString,QStringList> groups;
    };
    QMap<QString,PerspectiveInfo> m_perspectives;

    QString m_perspectivesFolder;
    QString getSettingsFileName( const QString& perspective, bool temp ) const;
    std::shared_ptr<QSettings> getSettingsObject( const QString& filePath ) const;
};

}

