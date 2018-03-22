#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Creates Socket Object
    //Socket sends and recieves data
    socket = new QTcpSocket(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Connect_clicked()
{
    if(!socket->isOpen()){
        //Retrieves the host ip and player name from the message boxes
        hostIP="127.0.0.1";
        //Initiates a connection to to the host IP and sends the nickname
        QByteArray sendData;
        sendData.append("Connect");
        socket->connectToHost(hostIP, 5300);
        socket->waitForConnected(1000);
        //qDebug() << socket->state();
        if(socket->state() == QAbstractSocket::ConnectedState)
        {
            socket->write(sendData); //write the data itself
            socket->waitForBytesWritten();
        }
        else
        {
            qDebug() << socket->errorString();
            socket->close();
        }

        //Connects the socket to slots for reading back data and reseting variables
        connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()),this, SLOT(Disconnected()));
    }
    else{
        qDebug() <<"Already Connected";
    }

}

void MainWindow::Disconnected()
{//Resets the UI on disconnect
    qDebug() << "Disconnected";
    socket->close();
}

void MainWindow::readyRead()
{//Triggered anytime the server sends data
    //Reads Socket Data sent by the server
    QString data;
    data = socket->readAll();
    //qDebug()<<data;

    //Splits the data based on ';'
    QString command = data.split(";").first();
    ui->Log->setText(ui->Log->toPlainText()+data);
    if(command=="END"){
        //If the server is indicating the game has ended
        //Displays who won to the player and disconnects from the server
        //qDebug()<<"End";
        socket->disconnectFromHost();
    }
}

void MainWindow::on_Jump_clicked()
{
    QByteArray updateData;
    updateData.append("JUMP");
    //qDebug() << socket->state();
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(updateData); //write the data itself
        socket->waitForBytesWritten();
    }
    else
    {
        qDebug() << socket->errorString();
    }
}


void MainWindow::on_Left_clicked()
{
    QByteArray updateData;
    updateData.append("LEFT");
    //qDebug() << socket->state();
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(updateData); //write the data itself
        socket->waitForBytesWritten();
    }
    else
    {
        qDebug() << socket->errorString();
    }
}

void MainWindow::on_Stop_clicked()
{
    QByteArray updateData;
    updateData.append("STOP");
    //qDebug() << socket->state();
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(updateData); //write the data itself
        socket->waitForBytesWritten();
    }
    else
    {
        qDebug() << socket->errorString();
    }
}

void MainWindow::on_Right_clicked()
{
    QByteArray updateData;
    updateData.append("RIGHT");
    //qDebug() << socket->state();
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(updateData); //write the data itself
        socket->waitForBytesWritten();
    }
    else
    {
        qDebug() << socket->errorString();
    }
}
