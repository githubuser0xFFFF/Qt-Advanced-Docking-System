%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CResizeHandle : QFrame
{

    %TypeHeaderCode
    #include <ResizeHandle.h>
    %End

protected:
	void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

public:
	CResizeHandle(Qt::Edge HandlePosition, QWidget* parent /TransferThis/);
	virtual ~CResizeHandle();
	void setHandlePosition(Qt::Edge HandlePosition);
	Qt::Edge handlePostion() const;
	Qt::Orientation orientation() const;
	QSize sizeHint() const;
	bool isResizing() const;
	void setMinResizeSize(int MinSize);
	void setMaxResizeSize(int MaxSize);
	void setOpaqueResize(bool opaque = true);
	bool opaqueResize() const;
};

};

%End
