%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CFloatingDragPreview : QWidget, ads::IFloatingWidget
{

    %TypeHeaderCode
    #include <FloatingDragPreview.h>
    %End


public:
    CFloatingDragPreview(ads::CDockWidget* Content /TransferThis/ );
    CFloatingDragPreview(ads::CDockAreaWidget* Content /TransferThis/ );

    virtual ~CFloatingDragPreview();

    virtual bool eventFilter(QObject* watched, QEvent* event);

    virtual void startFloating(const QPoint& DragStartMousePos, const QSize& Size,
        ads::eDragState DragState, QWidget* MouseEventHandler);

    virtual void moveFloating();

    virtual void finishDragging();

signals:
    void draggingCanceled();

};

};

%End