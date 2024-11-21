/***************************************************************************
     Copyright          : (C) 2014 by Martin Sakmar
     e-mail             : martin.sakmar@gmail.com
 ***************************************************************************/

/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QList>
#include <QToolBar>

class MainWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    //~MainWindow() override;

    
protected slots:
    void slotRenamePatternSelected(QAction* a);

protected:
    void closeEvent(QCloseEvent* e) override;
    
private:
    MainWidget* m_mainWidget;
    
    QMenu* m_renameMenu;
    QList<QAction*> m_renameActions;
    QActionGroup* m_renameActionGroup;
    //QMenu* m_tagMenu;
    //QMenu* m_tagCopyMenu;
    
    QToolBar* m_toolBar;
    
    void createRenameMenu();
};

#endif // MAINWINDOW_H
