/***************************************************************************
     Copyright          : (C) 2018 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mainwindow.h"
#include "mainwidget.h"

#include <QActionGroup>
#include <QCloseEvent>
#include <QMenuBar>
#include <QToolButton>

// FIXME: set parent of all QObjects or delete all in destructor

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    m_mainWidget = new MainWidget(this);
    setCentralWidget(m_mainWidget);
    
    QMenuBar* menubar = menuBar();
    
    m_renameMenu = new QMenu("Rename", this);
    m_renameActionGroup = new QActionGroup(this);
    connect(m_renameActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotRenamePatternSelected(QAction*)));
    
    //menubar->addMenu(m_renameMenu);
    createRenameMenu();
    
    /*
    m_tagMenu = new QMenu("Tag", this);
    menubar->addMenu(m_tagMenu);
    m_tagMenu->addAction("Capitalize filename");
    m_tagMenu->addAction("Capitalize Tag");
    m_tagCopyMenu = m_tagMenu->addMenu("Copy");
    m_tagCopyMenu->addAction("Directory -> Artist");
    m_tagCopyMenu->addAction("Directory -> Artist - Album");
    */
    
    m_toolBar = new QToolBar("Test toolbar", this);
    m_toolBar->addAction("Save All", m_mainWidget, SLOT(slotSaveAll()));
    m_toolBar->addSeparator();
    m_toolBar->addAction("Generate tags", m_mainWidget, SLOT(slotGenerateTags()));
    QToolButton* m_renamebtn = new QToolButton(this);
    m_renamebtn->setText("Rename");
    m_renamebtn->setMenu(m_renameMenu);
    m_renamebtn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(m_renamebtn, SIGNAL(clicked(bool)), m_mainWidget, SLOT(slotRenameByPattern()));
    m_toolBar->addWidget(m_renamebtn);
    
    m_toolBar->addAction("Copy", m_mainWidget, SLOT(slotCopyMenuPopup()));
    m_toolBar->addAction("Swap", m_mainWidget, SLOT(slotSwapMenuPopup()));
    m_toolBar->addAction("Clear", m_mainWidget, SLOT(slotClearMenuPopup()));
    m_toolBar->addAction("Capitalize", m_mainWidget, SLOT(slotCapitalizeMenuPopup()));
    m_toolBar->addAction("Reset", m_mainWidget, SLOT(slotResetMenuPopup()));
    
    addToolBar(m_toolBar);
    
    showMaximized();
    //showMinimized();
    //slotShowLibrary();
}

void MainWindow::slotRenamePatternSelected(QAction* a) {
    if(!a) return;
    QString rp = a->text();
    m_mainWidget->setRenamePattern(rp);
    m_mainWidget->slotRenameByPattern();
}

void MainWindow::closeEvent(QCloseEvent* e) {
    // save unsaved files?
    e->accept();
    //else e->ignore();
}

void MainWindow::createRenameMenu() {
    QStringList rplist = m_mainWidget->renamePatternList();
    // clear old actions
    while(m_renameActions.size() > 0) {
        QAction* a = m_renameActions.takeFirst();
        m_renameActionGroup->removeAction(a);
        delete a;
    }

    QString rpcurrent = m_mainWidget->renamePattern();
    QAction* ca = nullptr; // current action
    for(int i = 0; i < rplist.size(); ++i) {
        QString rp = rplist.at(i);
        QAction* a = new QAction(rp, this);
        a->setCheckable(true);
        m_renameMenu->addAction(a);
        m_renameActionGroup->addAction(a);
        if(rp == rpcurrent) ca = a;
    }
    if(ca) ca->setChecked(true);
}
