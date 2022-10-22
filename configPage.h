#ifndef CONFIGPAGE_H
#define CONFIGPAGE_H

#include <QWidget>
#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QMenu>
#include <QAction>

#include "jgqj/global.h"

namespace Ui {
class configPage;
}

class configPage : public QWidget
{
    Q_OBJECT

signals:
    void freshMain();

public:
    explicit configPage(QWidget *parent = nullptr);
    ~configPage();

private slots:
//    void on_changeCompressButton_clicked();

//    void on_changeEncryptionButton_clicked();

    void on_changeDirButton_clicked();

private:
    Ui::configPage *ui;
};

extern void freshConfig(Ui::configPage* ui);

#endif // CONFIGPAGE_H
