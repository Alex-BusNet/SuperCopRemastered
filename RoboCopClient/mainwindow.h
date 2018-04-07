#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QtConcurrent/QtConcurrent>
#include "robocophandler.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString hostIP;

private slots:
    void on_Connect_clicked();
    void Disconnected();
    void readyRead();

    void on_Jump_clicked();

    void on_Left_clicked();

    void on_Stop_clicked();

    void on_Right_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    RoboCopHandler *rch;
    int **parsedView;
};

#endif // MAINWINDOW_H
