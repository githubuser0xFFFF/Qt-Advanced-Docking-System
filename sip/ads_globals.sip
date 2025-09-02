%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

%ModuleHeaderCode
PyObject *qtads_FindParent(PyObject* type, const QWidget *child);
%End

%ModuleCode
PyObject *qtads_FindParent(PyObject* type, const QWidget *w)
{
    // Check that the types checking was successful.
    if (!type)
        return 0;
   
    QWidget* parentWidget = w->parentWidget();

    while (parentWidget)
    {
        PyObject *ParentImpl = sipConvertFromType(parentWidget, sipType_QObject, 0);
        if (!ParentImpl)
        {
            return 0;
        }
            
        if (PyObject_IsInstance(ParentImpl, type))
            return ParentImpl;
        
        Py_DECREF(ParentImpl);
        
        parentWidget = parentWidget->parentWidget();
    }
    
    Py_INCREF(Py_None);
    return Py_None;
}
%End

namespace ads
{
    %TypeHeaderCode
    #include <ads_globals.h>
    %End

    enum DockWidgetArea
    {
        NoDockWidgetArea,
        LeftDockWidgetArea,
        RightDockWidgetArea,
        TopDockWidgetArea,
        BottomDockWidgetArea,
        CenterDockWidgetArea,
        LeftAutoHideArea,
        RightAutoHideArea,
        TopAutoHideArea,
        BottomAutoHideArea,
        InvalidDockWidgetArea,
        OuterDockAreas,
        AutoHideDockAreas,
        AllDockAreas
    };
    typedef QFlags<ads::DockWidgetArea> DockWidgetAreas;


    enum eTabIndex
    {
        TabDefaultInsertIndex,
        TabInvalidIndex
    };

    enum TitleBarButton
    {
        TitleBarButtonTabsMenu,
        TitleBarButtonUndock,
        TitleBarButtonClose,
        TitleBarButtonAutoHide,
        TitleBarButtonMinimize
    };

    enum eDragState
    {
        DraggingInactive,
        DraggingMousePressed,
        DraggingTab,
        DraggingFloatingWidget
    };

    enum eIcon
    {
        TabCloseIcon,
        AutoHideIcon,
        DockAreaMenuIcon,
        DockAreaUndockIcon,
        DockAreaCloseIcon,
        IconCount,
    };

    enum eBitwiseOperator
    {
        BitwiseAnd,
        BitwiseOr
    };
    
    enum SideBarLocation
    {
        SideBarTop,
        SideBarLeft,
        SideBarRight,
        SideBarBottom,
        SideBarNone
    };
    
    namespace internal
    {    
        void replaceSplitterWidget(QSplitter* Splitter, QWidget* From, QWidget* To);
        void hideEmptyParentSplitters(ads::CDockSplitter* FirstParentSplitter);
        
        class CDockInsertParam
        {
            %TypeHeaderCode
            #include <ads_globals.h>
            %End
            
        public:
            Qt::Orientation orientation() const;
            bool append() const;
            int insertOffset() const;
        };
        ads::internal::CDockInsertParam dockAreaInsertParameters(ads::DockWidgetArea Area);
        ads::SideBarLocation toSideBarLocation(ads::DockWidgetArea Area);
        bool isHorizontalSideBarLocation(ads::SideBarLocation Location);
        bool isSideBarArea(ads::DockWidgetArea Area);
        
        SIP_PYOBJECT findParent(SIP_PYTYPE type, const QWidget *w) const /TypeHint="QObject"/;
        %MethodCode
        sipRes = qtads_FindParent(a0, a1);

        if (!sipRes)
        {
            sipIsErr = 1;
        }
        %End
        
        QPixmap createTransparentPixmap(const QPixmap& Source, qreal Opacity);  

        QPoint globalPositionOf(QMouseEvent* ev);

        void setButtonIcon(QAbstractButton* Button, QStyle::StandardPixmap StandarPixmap, ads::eIcon CustomIconId);
        
        enum eRepolishChildOptions
        {
            RepolishIgnoreChildren,
            RepolishDirectChildren,
            RepolishChildrenRecursively
        };
        
       void repolishStyle(QWidget* w, ads::internal::eRepolishChildOptions Options = ads::internal::RepolishIgnoreChildren); 
       QRect globalGeometry(QWidget* w);
    };

};

%End
