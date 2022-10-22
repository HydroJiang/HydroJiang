#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>
#include <QMessageBox>
#include <QMenu>
#include <QAction>

#include "configPage.h"
#include "ui_configPage.h"

#include "jgqj/global.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void freshTalbleWidget();
    void backUpInQt(string pathAndName);
    string getPasswd();

private slots:

    void on_backUpFileButton_clicked();

    void on_backUpDirButton_clicked();

    void on_configButton_clicked();

    void on_dirMenu_customContextMenuRequested(const QPoint &pos);

    void slotActionRemove();

    void slotActionRestoreTarget();

    void slotActionRestoreNull();

    void slotActionCompare();

    void getFreshMain();

private:
    Ui::MainWindow *ui;
    configPage newPage;

};

#endif // MAINWINDOW_H
