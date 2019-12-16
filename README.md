# Advanced Docking System for Qt 
[![Build Status](https://travis-ci.org/githubuser0xFFFF/Qt-Advanced-Docking-System.svg?branch=master)](https://travis-ci.org/githubuser0xFFFF/Qt-Advanced-Docking-System)
[![Build status](https://ci.appveyor.com/api/projects/status/qcfb3cy932jw9mpy/branch/master?svg=true)](https://ci.appveyor.com/project/githubuser0xFFFF/qt-advanced-docking-system/branch/master)
[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](gnu-lgpl-v2.1.md)

Qt Advanced Docking System lets you create customizable layouts using a full 
featured window docking system similar to what is found in many popular 
integrated development environments (IDEs) such as Visual Studio. 

[![Video Advanced Docking](doc/advanced-docking_video.png)](https://www.youtube.com/watch?v=7pdNfafg3Qc)

Everything is implemented with standard Qt functionality without any
platform specific code. Basic usage of QWidgets and QLayouts and using basic 
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
### Windows
Windows 10 [![Build status](https://ci.appveyor.com/api/projects/status/qcfb3cy932jw9mpy/branch/master?svg=true)](https://ci.appveyor.com/project/githubuser0xFFFF/qt-advanced-docking-system/branch/master)

The library was developed on and for Windows. It is used in a commercial Windows application and is therefore constantly tested.

### macOS
macOS [![Build Status](https://travis-ci.org/githubuser0xFFFF/Qt-Advanced-Docking-System.svg?branch=master)](https://travis-ci.org/githubuser0xFFFF/Qt-Advanced-Docking-System)

The application can be compiled for macOS. A user reported, that the library works on macOS. If have not tested it.

### Linux
Ubuntu [![Build Status](https://travis-ci.org/githubuser0xFFFF/Qt-Advanced-Docking-System.svg?branch=master)](https://travis-ci.org/githubuser0xFFFF/Qt-Advanced-Docking-System)
[![Build status](https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/workflows/linux-builds/badge.svg)](https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/actions?query=workflow%3Alinux-builds)

The application can be compiled for Linux and has been developed and tested with **Kubuntu 18.04**.

![Advanced Docking on Linux](doc/linux_kubuntu_1804.png)

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
[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](gnu-lgpl-v2.1.md)
This project uses the [LGPLv2.1 license](gnu-lgpl-v2.1.md)

## Alternative Docking System Implementations
If this Qt Advanced Docking System does not fit to your needs you may consider some of the alternative docking system solutions for Qt.

### KDDockWidgets
This is an advanced docking framework for Qt from [KDAB](https://www.kdab.com/). The interesting thing is, that they separated GUI code from logic, so they can easily provide a QtQuick backend in the future.

- [Blog post about KDDockWidgets](https://www.kdab.com/kddockwidgets/)
- [GitHub project](https://github.com/KDAB/KDDockWidgets)


### QtitanDocking
This is a commercial component from [Developer Machines](https://www.devmachines.com/) for Qt Framework that allows to create a Microsoft like dockable user interface. They also offer a lot of other interesting and useful components for Qt.

- [Product page](https://www.devmachines.com/qtitandocking-overview.html)
