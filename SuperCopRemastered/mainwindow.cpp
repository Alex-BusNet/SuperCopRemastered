#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scrmg = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_exitPB_clicked()
{
    this->close();
}

void MainWindow::on_runNNPB_clicked()
{

}

void MainWindow::on_settingsPB_clicked()
{

}

void MainWindow::on_playSCPB_clicked()
{
    if(scrmg != NULL)
        delete scrmg;

    scrmg = new SuperCopRMGame();

    scrmg->show();
}
