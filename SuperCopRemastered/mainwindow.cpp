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

    scrmg = new SuperCopRMGame(NULL, ui->industrialCB->isChecked());

    scrmg->show();
}

void MainWindow::on_industrialCB_stateChanged(int arg1)
{
    if(ui->industrialCB->isChecked())
        ui->bgLabel->setPixmap(QPixmap("Assets/UI/fire_background.png"));
    else
        ui->bgLabel->setPixmap(QPixmap("Assets/UI/grass_background.png"));
}
