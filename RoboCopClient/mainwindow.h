#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QtConcurrent/QtConcurrent>
#include "robocophandler.h"
#include <QThread>

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

    void KeyStateUpdate(uint8_t ksu);
    void ResetLevel();
    void genomeStatus(int num);
    void speciesStatus(int num, int size);
    void generationStatus(int num);
    void fitnessUpdate(int num);
    void maxFitnessUpdate(int num);
    void ResetUpdate(bool stat);
    void timeoutValue(float num);
    void on_closePB_clicked();

    void on_resetNNPB_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;
    RoboCopHandler *rch;
    int **parsedView;
//    QFuture<void> rchThread;
};

#endif // MAINWINDOW_H
