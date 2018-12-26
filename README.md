# Advanced Docking System for Qt

Qt Advanced Docking System lets you create customizable layouts using a full 
featured window docking system similar to what is found in many popular 
integrated development environements (IDEs) such as Visual Studio. 
Everything is implemented with standard Qt functionality without any
platform specific code. Basic usage of QWidgets an QLayouts and using basic 
styles as much as possible.

This work is based on and inspired by the 
[Advanced Docking System for Qt](https://github.com/mfreiholz/Qt-Advanced-Docking-System) 
from Manuel Freiholz. I did an almost complete rewrite of his code to improve
code quality, readibility and to fix all issues from the issue tracker 
of his docking system project.

## Features
### Docking everywhere - no central widget
There is no central widget like in the Qt docking system. You can dock on every
border of the main window or you can dock into each dock area - so you are
free to dock almost everywhere.

![Dropping widgets](doc/preview-dragndrop.png)\
\
![Dropping widgets](doc/preview-dragndrop_dark.png)

### Docking inside floating windows
There is no difference between the main window and a floating window. Docking
into floating windows is supported.

![Docking inside floating windows](doc/floating-widget-dragndrop.png)\
\
![Docking inside floating windows](doc/floating-widget-dragndrop_dark.png)

### Grouped dragging
When dragging the titlebar of a dock, all the tabs that are tabbed with it are 
going to be dragged. So you can move complete groups of tabbed widgets into
a floating widget or from one dock area to another one.

![Grouped dragging](doc/grouped-dragging.png)\
\
![Grouped dragging](doc/grouped-dragging_dark.png)

### Perspectives for fast switching of the complete main window layout
A perspective defines the set and layout of dock windows in the main
window. You can save the current layout of the dockmanager into a named
perspective to make your own custom perspective. Later you can simply
select a perspective from the perspective list to quickly switch the complete 
main window layout.

![Perspective](doc/perspectives.png)\
\
![Perspective](doc/perspectives_dark.png)

## Tested Compatible Environments
- Windows 10

## Build
Open the `ads.pro` with QtCreator and start the build, that's it.
You can run the demo project and test it yourself.

## Getting started / Example
The following example shows the minimum code required to use the advanced Qt docking system.

*MainWindow.h*

```cpp
#include <QMainWindow>
#include "DockManager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    
    // The main container for docking
    ads::CDockManager* m_DockManager;
};
```
*MainWindow.cpp*
```cpp
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    m_DockManager = new ads::CDockManager(this);

    // Create example content label - this can be any application specific
    // widget
    QLabel* l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
    DockWidget->setWidget(l);

    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed
    ui->menuView->addAction(DockWidget->toggleViewAction());

    // Add the dock widget to the top dock widget area
    m_DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
```

## Developers
- Uwe Kindler, Project Maintainer
- Manuel Freiholz 

## License information
This project uses the [LGPLv2.1 license](gnu-lgpl-v2.1.md)

