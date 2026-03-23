#ifndef ADS_FRAME_LESS_H
#define ADS_FRAME_LESS_H

#include <QMainWindow>

#include "ads_globals.h"

class QMenu;
class QLabel;
class QPushButton;
class QWidget;
class QHBoxLayout;
class WidgetEventHelper;

namespace ads
{
class ADS_EXPORT IStyledWindow
{
public:
    virtual void setupMenuBar(QMenuBar* menu) = 0;
    virtual void setIcon(QIcon menu) = 0;
    virtual QMenuBar* menuBar() = 0;
    virtual void setSubToolbar(QToolBar* toolbar) = 0;

protected:
    ~IStyledWindow() = default;
};

class ADS_EXPORT StyledWindow : public QMainWindow, public IStyledWindow
{
    Q_OBJECT

public:
    explicit StyledWindow(QWidget* parent = 0,
                          Qt::WindowFlags f = Qt::WindowFlags(),
                          QString windowTitle = "");
    ~StyledWindow();

    void setupMenuBar(QMenuBar* menuBar) override;
    QMenuBar* menuBar() override;
    void setIcon(QIcon icon) override;
    void setSubToolbar(QToolBar* toolbar) override;

protected:
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void init();
    void initWindowTitle();
#ifdef Q_OS_WIN
    void showSystemMenu(QWidget* widget, const QPoint& pos);
public slots:
    void showFullScreen();

protected:
    void initWindowBackground(bool transparent = true);
    void setResizeable(bool resizeable = true);
    bool isResizeable();
    void setResizeableAreaWidth(int width = 5);
    void setTitleBar(QWidget* titlebar);
    void addIgnoreWidget(QWidget* widget);
    void setContentsMargins(const QMargins& margins);
    void setContentsMargins(int left, int top, int right, int bottom);
    void constructHintButtons();
    bool nativeEvent(const QByteArray& eventType, void* message,
                     long* result) override;
    bool isOutOfWidget(QWidget* widget);
    QMenu* createPopupMenu() override;

    void forceRedraw();
    void updateWindowDpr(float dpr, QRect rect, WId wid);

private slots:
    void onTitleBarDestroyed();
#endif
private:
    struct StyledWindowPrivate;
    StyledWindowPrivate* d;
};

}  // namespace ads
#endif  // ADS_FRAME_LESS_H
