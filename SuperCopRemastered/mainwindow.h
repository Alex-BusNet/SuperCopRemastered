#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Game/supercoprmgame.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_exitPB_clicked();

    void on_runNNPB_clicked();

    void on_settingsPB_clicked();

    void on_playSCPB_clicked();

private:
    Ui::MainWindow *ui;
    SuperCopRMGame *scrmg;
};

#endif // MAINWINDOW_H
