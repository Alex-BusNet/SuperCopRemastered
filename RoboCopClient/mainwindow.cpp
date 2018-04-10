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

    rch = new RoboCopHandler();

    connect(rch, &RoboCopHandler::keyStateUpdate, this, &MainWindow::KeyStateUpdate);

    parsedView = new int *[10];
    for(int y = 0; y < 10; y++)
    {
        parsedView[y] = new int[18];
        for(int x = 0; x < 18; x++)
        {
            parsedView[y][x] = 0;
        }
    }

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

        if(socket->state() != QAbstractSocket::UnconnectedState)
        {
            //Connects the socket to slots for reading back data and reseting variables
            connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
            connect(socket, SIGNAL(disconnected()),this, SLOT(Disconnected()));

            ///TODO: We should properly handle this concurrent task better
//            if(rch != NULL)
//                QtConcurrent::run(*rch, RoboCopHandler::GameLoop);
        }
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
    qDebug() << "ReadyRead()";
    QString data;
    data = socket->readAll();
    //qDebug()<<data;

    //Splits the data based on ';'
    qDebug() << "Data: " << data;
    QString command = data.split(";").first();
    //ui->Log->setText(ui->Log->toPlainText()+data);
    qDebug() << "Command: " << command;
    if(command=="END"){
        //If the server is indicating the game has ended
        //Displays who won to the player and disconnects from the server
        //qDebug()<<"End";
        socket->disconnectFromHost();
    }
    else if(command=="VisibleTerrain"){
        QString b = data.split("VisibleTerrain").last();
        qDebug() << "b: " << b;
        qDebug() << "Clearing parsedView";
        for(int y = 0; y < 10; y++)
        {
            for(int x = 0; x < 18; x++)
            {
                parsedView[y][x] = 0;
            }
        }

        qDebug() << "Splitting data";
        QStringList  pieces = b.split(";");
        qDebug() << "Pieces: " << pieces;
        //If the server is indicating the game has ended
        for(int i=1; i<pieces.length()-1; i++){
            QString p = pieces.at(i);
            if(!p.isEmpty() && !p.isNull())
            {
                QStringList arraySet = pieces.at(i).split(":");
                qDebug()<<"test "<<arraySet.length()<<" "<<pieces.at(i);
                qDebug() << arraySet.at(1) << " " << arraySet.at(2) << " " << arraySet.at(0);
                parsedView[arraySet.at(1).toInt()][arraySet.at(2).toInt()] = arraySet.at(0).toInt();
            }
        }

        qDebug() << "Formatting display string";
        QString disp = "";
        for(int y = 0; y < 10; y++)
        {
            for(int x = 0; x < 18; x++)
            {
                if(parsedView[y][x] != 0){
                    disp=disp+" "+QString::number(parsedView[y][x]);
                }
                else{
                    disp=disp+"  ";
                }
            }
            disp=disp+"\n";
        }
        qDebug() << "Setting display string";
        ui->Log->setText(disp);

        qDebug() << "Done";

        //if(rch != NULL) { rch->SetInputs(parsedView); }
    }
    else if(command=="LevelReset")
    {
//        if(rch != NULL) {rch->InitializeRun();}
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

void MainWindow::KeyStateUpdate(uint8_t ksu)
{
    QByteArray bArr;
    bArr.append("Controls;");
    bArr.append(QString::number(ksu, 2));
    socket->write(bArr);
}
