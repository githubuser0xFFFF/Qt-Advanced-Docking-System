#ifndef ADS_QWIDGET_EVENT_HELPER_H
#define ADS_QWIDGET_EVENT_HELPER_H

#include <QObject>

class QWidget;

class WidgetEventHelper : QObject {
    Q_OBJECT
public:
    WidgetEventHelper(QObject* parent = nullptr);
    ~WidgetEventHelper();

    void SetWidget(QWidget* widget);
    void ReleaseFlag();

    void SetWidgetRectFlag(bool inWidgetRect);
    QWidget* Widget();
    bool IsFirstMove();

    void SetFirstMove(bool firstEnter);

    bool HandleMousePress(long* result);
    bool HandleMouseRelease(long* result, bool isNClient = true);
    void HandleMouseMove();

    void SendMouseRelease(bool inWidgetRect);

private:
    void SendMouseEnter();
    void SendMouseLeave();
    void SendMousePress();

private:
    QWidget* widget_{ nullptr };

    bool inWidgetRect_{ false };
    bool inLastWidgetRect_{ false };

    bool left_{ false };
    bool pressed_{ false };
    bool released_{ false };
    bool firstMove_{ false };
};

#endif // ADS_QWIDGET_EVENT_HELPER_H