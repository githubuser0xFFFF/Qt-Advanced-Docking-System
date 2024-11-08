//
// Created by fuga on 08 nov 2024.
//

#ifndef QTADS_MYDOCKAREATITLEBAR_H
#define QTADS_MYDOCKAREATITLEBAR_H

#include <DockAreaTitleBar.h>

class MyDockAreaTitleBar : public ads::CDockAreaTitleBar {
public:
    explicit MyDockAreaTitleBar(ads::CDockAreaWidget* parent)
        : CDockAreaTitleBar(parent)
    {}

    QMenu* buildContextMenu(QMenu*) override
    {
        auto menu = ads::CDockAreaTitleBar::buildContextMenu(nullptr);
        menu->addSeparator();
        auto action = menu->addAction(tr("Format HardDrive"));
        
        connect(action, &QAction::triggered, this, [this](){
            QMessageBox msgBox;
            msgBox.setText("No, just kidding");
            msgBox.setStandardButtons(QMessageBox::Abort);
            msgBox.setDefaultButton(QMessageBox::Abort);
            msgBox.exec();
        });
        
        return menu;
    }
};

#endif  // QTADS_MYDOCKAREATITLEBAR_H
