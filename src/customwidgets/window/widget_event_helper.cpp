#include "widget_event_helper.h"

#include <QEvent>
#include <QMouseEvent>
#include <QWidget>
#include <QApplication>

WidgetEventHelper::WidgetEventHelper(QObject* parent)
    : QObject(parent),
      widget_(nullptr),
      inWidgetRect_(false),
      inLastWidgetRect_(false),
      left_(false),
      pressed_(false),
      released_(false),
      firstMove_(false)
{}

WidgetEventHelper::~WidgetEventHelper() {}

void WidgetEventHelper::SetWidget(QWidget* widget)
{
    widget_ = widget;
}

void WidgetEventHelper::ReleaseFlag()
{
    pressed_ = false;
    firstMove_ = false;
}

void WidgetEventHelper::SetWidgetRectFlag(bool inWidgetRect)
{
    inWidgetRect_ = inWidgetRect;
}

QWidget* WidgetEventHelper::Widget()
{
    return widget_;
}

bool WidgetEventHelper::IsFirstMove()
{
    return firstMove_;
}

void WidgetEventHelper::SetFirstMove(bool firstEnter)
{
    firstMove_ = firstEnter;
}

bool WidgetEventHelper::HandleMousePress(long* result)
{
    *result = 0;
    if (widget_) {
        this->firstMove_ = true;
        this->pressed_ = true;
        this->SendMousePress();
        return true;
    }
    return false;
}

bool WidgetEventHelper::HandleMouseRelease(long* result, bool isNClient)
{
    *result = 0;
    if (widget_) {
        if (pressed_) {
            pressed_ = false;
            if (isNClient) {
                SendMouseRelease(inWidgetRect_);
                left_ = true;
                inWidgetRect_ = false;
                inLastWidgetRect_ = false;
                SendMouseLeave();
                return true;
            } else {
                if (inWidgetRect_) {
                    SendMousePress();
                    SendMouseRelease(true);
                }
            }
        }
    }
    return false;
}

void WidgetEventHelper::HandleMouseMove()
{
    if (widget_) {
        if (left_) {
            left_ = false;
            return;
        }
        if (inWidgetRect_ != inLastWidgetRect_) {
            inLastWidgetRect_ = inWidgetRect_;
            if (inWidgetRect_) {
                SendMouseEnter();
            } else {
                SendMouseLeave();
            }
        }
    }

    inWidgetRect_ = false;
}

void WidgetEventHelper::SendMouseEnter()
{
    QEvent event(QEvent::Enter);
    QApplication::sendEvent(widget_, &event);
    widget_->update();
}

void WidgetEventHelper::SendMouseLeave()
{
    QEvent event(QEvent::Leave);
    QApplication::sendEvent(widget_, &event);
    widget_->update();
}

void WidgetEventHelper::SendMousePress()
{
    QMouseEvent event(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(widget_, &event);
}

void WidgetEventHelper::SendMouseRelease(bool inWidgetRect)
{
    if (inWidgetRect) {
        QMouseEvent event(QEvent::MouseButtonRelease, QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(widget_, &event);
    } else {
        QMouseEvent event(QEvent::MouseButtonRelease, QPoint(-1, -1), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(widget_, &event);
    }
}
