%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CSpacerWidget : QWidget
{
    %TypeHeaderCode
    #include <DockAreaTitleBar_p.h>
    %End

public:
	CSpacerWidget(QWidget* Parent /TransferThis/ = 0 );
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
};

};

%End
