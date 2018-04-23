#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>

typedef struct { int x; int y; float value; } Cell;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("RoboCop NEAT Controller");
    //Creates Socket Object
    //Socket sends and recieves data
    socket = new QTcpSocket(this);

    qRegisterMetaType<uint8_t>("uint8_t");

    rch = RoboCopHandler::instance();
    ui->resetNNPB->setEnabled(false);
    ui->saveNNPb->setEnabled(false);
    ui->loadNNPb->setEnabled(false);

    ui->Right->hide();
    ui->Left->hide();
    ui->Jump->hide();
    ui->Stop->hide();

    connect(rch, &RoboCopHandler::keyStateUpdate, this, &MainWindow::KeyStateUpdate);
    connect(rch, &RoboCopHandler::FitnessUpdate, this, &MainWindow::fitnessUpdate);
    connect(rch, &RoboCopHandler::GenerationUpdate, this, &MainWindow::generationStatus);
    connect(rch, &RoboCopHandler::GenomeUpdate, this, &MainWindow::genomeStatus);
    connect(rch, &RoboCopHandler::MaxFitnessUpdate, this, &MainWindow::maxFitnessUpdate);
    connect(rch, &RoboCopHandler::SpeciesUpdate, this, &MainWindow::speciesStatus);
    connect(rch, &RoboCopHandler::NewSpecies, this, &MainWindow::ResetLevel);
    connect(rch, &RoboCopHandler::resetStat, this, &MainWindow::ResetUpdate);
    connect(rch, &RoboCopHandler::timeoutval, this, &MainWindow::timeoutValue);

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
    if(rch->isRunning())
        rch->quit();

    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    /*
     * Draw the NN here.
     */
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        if(!this->paintingActive())
        {
            QPainter paint;
            paint.begin(this);

            paint.setPen(Qt::black);
            // window height: 287px
            paint.drawRect(10, 290, 18*10, 10*10);

            if(rch->isRunning() && rch->isGameRunning())
            {
                Pool *p = rch->GetPool();
                if(p != NULL && (p->species.size() > p->GetCurrentSpecies()))
                {
                    Species *s = p->species[p->GetCurrentSpecies()];
                    if(s != NULL && (s->genomes.size() > p->GetCurrentGenome()))
                    {
                        Genome *gm = s->genomes[p->GetCurrentGenome()];
                        if(gm != NULL && gm->network != NULL)
                        {
                            Network *net = gm->network;

                            QMap<int, Cell> cells;

                            int i = 0;
                            if((net != NULL) && (net->neurons.size() > 0))
                            {
                                QList<int> neuronKeys = net->neurons.keys();
                                // Generate a cell for every input.
                                for(int dy = 0; dy < 10; dy++)
                                {
                                    for(int dx = 0; dx < 18; dx++)
                                    {
                                        if(i >= neuronKeys.size()) { break; }

                                        Cell c;

                                        c.x = 10 + 10 * dx;
                                        c.y = 290 + 10 * dy;

                                        c.value = net->neurons[neuronKeys.at(i)]->value;

                                        if(!cells.contains(neuronKeys.at(i)))
                                            cells.insert(neuronKeys.at(i), c);
                                        else
                                            cells[neuronKeys.at(i)] = c;

                                        i++;
                                    }

                                    if(i >= neuronKeys.size()) { break; }
                                }

                                // Generate  cell for the Bias neuron.
                                Cell biasCell;
                                biasCell.x = 180;
                                biasCell.y = 400;

                                // We are using InputSize instead of Inputs here
                                // because the Bias node is at index 180, which
                                // is the 181st item in the neuron vector. Inputs
                                // has a value of 181, which would be the 182nd item
                                // in the vector.
                                if(net->neurons.contains(RoboCop::InputSize))
                                    biasCell.value = net->neurons[RoboCop::InputSize]->value;
                                else
                                    biasCell.value = 1;

                                cells.insert(RoboCop::InputSize, biasCell);

                                // Generate the output neurons.
                                for(int j = 0; j < RoboCop::Outputs; j++)
                                {
                                    Cell c;
                                    c.x = 560;
                                    c.y = 310 + (RoboCop::BoxRadius * j) + (j * 15);
                                    if(net->neurons.contains(RoboCop::MaxNodes + j))
                                        c.value = net->neurons[RoboCop::MaxNodes + j]->value;
                                    else
                                        c.value = 0;

                                    if(!cells.contains(RoboCop::MaxNodes+j))
                                        cells.insert(RoboCop::MaxNodes+j, c);
                                    else
                                        cells[RoboCop::MaxNodes+j] = c;

                                    if(c.value > 0)
                                        paint.setPen(Qt::blue);
                                    else
                                        paint.setPen(Qt::black);

                                    // Draw the text for each button on screen.
                                    paint.drawText(575, c.y + 10, RoboCop::ButtonNames[j]);
                                }
                                paint.setPen(Qt::black);

                                // Add the network neurons.
                                foreach(int k, net->neurons.keys())
                                {
                                    if(k > RoboCop::InputSize && k < RoboCop::MaxNodes)
                                    {
                                        Cell c;
                                        c.x = 200;
                                        c.y = 290;
                                        c.value = net->neurons[k]->value;
                                        cells.insert(k, c);
                                    }
                                }

                                for(int j = 0; j < 4; j++)
                                {
                                    // Calculate the position of each cell on the screen (four times?)
                                    foreach(Gene *g, gm->genes)
                                    {
                                        if(g->enabled)
                                        {
                                            Cell c1;
                                            Cell c2;

                                            if(cells.contains(g->into))
                                                c1 = cells[g->into];
                                            else
                                                c1 = cells[RoboCop::Inputs];

                                            if(cells.contains(g->out))
                                                c2 = cells[g->out];
                                            else
                                                c2 = cells[RoboCop::MaxNodes];

                                            // Calculate the position of cell 1
                                            if(g->into > RoboCop::Inputs && g->into < RoboCop::MaxNodes)
                                            {
                                                c1.x = (0.75*c1.x + 0.25*c2.x);
                                                if(c1.x >= c2.x)
                                                {
                                                    c1.x -= 15;
                                                }

                                                if(c1.x < 200)
                                                    c1.x = 200;

                                                if(c1.x > 560)
                                                    c1.x = 560;

                                                c1.y = (0.75*c1.y + 0.25*c2.y);

                                                if(c1.y > 400)
                                                    c1.y = 400;

                                                if(cells.contains(g->into))
                                                    cells[g->into] = c1;
                                            }
                                            // Calculate the position of cell 2
                                            if(g->out > RoboCop::Inputs && g->out < RoboCop::MaxNodes)
                                            {
                                                c2.x = (0.25*c1.x + 0.75*c2.x);
                                                if(c1.x >= c2.x)
                                                {
                                                    c2.x += 15;
                                                }

                                                if(c2.x < 200)
                                                    c2.x = 200;

                                                if(c2.x > 560)
                                                    c2.x = 560;

                                                c2.y = (0.25*c1.y + 0.75*c2.y);

                                                if(c2.y > 400)
                                                    c2.y = 400;

                                                if(cells.contains(g->out))
                                                    cells[g->out] = c2;
                                            }
                                        }
                                    }
                                }

                                // Color and render the neurons.
                                foreach(int k, cells.keys())
                                {
                                    Cell c = cells[k];
                                    if(k > RoboCop::InputSize || c.value != 0)
                                    {
                                        // BizHawk color format is 0xAARRGGBB
                                        quint64 opacity = 0xFF000000;
                                        if(c.value == 0)
                                        {
                                            opacity = 0x50000000;
                                        }

                                        int colorVal = std::floor((c.value+1)/ 2.0f * 256);
                                        if(colorVal > 255) { colorVal = 255; }
                                        else if(colorVal < 0) { colorVal = 0; }
                                        uint32_t colorOut = opacity + colorVal*0x10000 + colorVal*0x100 + colorVal;

                                        QColor color(colorOut);

                                        // Convert the 0xAARRGGBB format to 0xRRGGBBAA format
                                        color.setAlpha(opacity >> 24);
                                        color.setRed((colorOut << 8) >> 24);
                                        color.setGreen((colorOut << 16) >> 24);
                                        color.setBlue((colorOut << 24) >> 24);

                                        // Draw each cell on screen
                                        QRect cRect(c.x, c.y, RoboCop::BoxRadius, RoboCop::BoxRadius);
                                        paint.fillRect(cRect, color);
                                        paint.drawRect(cRect);
                                    }
                                }

                                // Draw the lines connecting neurons
                                foreach(Gene *g, gm->genes)
                                {
                                    if(g->enabled)
                                    {
                                        Cell c1;
                                        Cell c2;
                                        if(cells.contains(g->into))
                                            c1 = cells[g->into];
                                        else
                                            c1 = cells[RoboCop::Inputs];

                                        if(cells.contains(g->out))
                                            c2 = cells[g->out];
                                        else
                                            c2 = cells[RoboCop::MaxNodes];

                                        quint64 opacity = 0xA0000000;
                                        if(c1.value == 0)
                                            opacity = 0x20000000;

                                        uint32_t colorVal = 0x80 - std::floor(std::abs(Network::sigmoid(g->weight)) * 0x80);
                                        if(g->weight > 0)
                                            colorVal = opacity + 0x8000 + 0x10000*colorVal;
                                        else
                                            colorVal = opacity + 0x800000 + 0x100*colorVal;

                                        QColor color(colorVal);

                                        // Convert the 0xAARRGGBB format to 0xRRGGBBAA format
                                        color.setAlpha(opacity >> 24);
                                        color.setRed((colorVal << 8) >> 24);
                                        color.setGreen((colorVal << 16) >> 24);
                                        color.setBlue((colorVal << 24) >> 24);

                                        paint.setPen(QPen(QColor(colorVal)));
                                        paint.drawLine(c1.x+5, c1.y+5, c2.x+5, c2.y+5);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            paint.end();
        }
    }
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

            rch->start();

            ui->Connect->setText("Disconnect");

//            ui->Jump->setEnabled(false);
//            ui->Left->setEnabled(false);
//            ui->Right->setEnabled(false);
//            ui->Stop->setEnabled(false);

            ui->resetNNPB->setEnabled(true);
            ui->saveNNPb->setEnabled(true);
            ui->loadNNPb->setEnabled(true);
        }
    }
    else
    {
        Disconnected();
        ui->Connect->setText("Connect");

//        ui->Jump->setEnabled(true);
//        ui->Left->setEnabled(true);
//        ui->Right->setEnabled(true);
//        ui->Stop->setEnabled(true);

        ui->resetNNPB->setEnabled(false);
        ui->saveNNPb->setEnabled(false);
        ui->loadNNPb->setEnabled(false);

        if(rch->isRunning())
            rch->exit();

//        qDebug() <<"Already Connected";
    }

}

void MainWindow::Disconnected()
{//Resets the UI on disconnect
    qDebug() << "\tDisconnected";
    socket->close();

    ui->Connect->setText("Connect");

//    ui->Jump->setEnabled(true);
//    ui->Left->setEnabled(true);
//    ui->Right->setEnabled(true);
//    ui->Stop->setEnabled(true);

    ui->resetNNPB->setEnabled(false);
    ui->saveNNPb->setEnabled(false);
    ui->loadNNPb->setEnabled(false);

    if(rch->isRunning())
        rch->exit();
}

void MainWindow::readyRead()
{//Triggered anytime the server sends data
    //Reads Socket Data sent by the server
    QString data;
    data = socket->readAll();
    //qDebug()<<data;

    //Splits the data based on '__'
//    qDebug() << "Data: " << data;

    // String format: <Command>_<Data>;<Data>;...;__<Command>_<Data>;<Data>;...;
    // Splits the string into each command passed from RoboCop
    QStringList cmdSet = data.split("__");
    foreach(QString s, cmdSet)
    {
        // Splits each command set into the command and associated data
        QStringList subSet = s.split("_");
        QString command = subSet.first();

        //ui->Log->setText(ui->Log->toPlainText()+data);
    //    qDebug() << "Command: " << command;
        if(command=="END"){
            //If the server is indicating the game has ended
            //Displays who won to the player and disconnects from the server
            //qDebug()<<"End";
            socket->disconnectFromHost();
        }
        else if(command=="VisibleTerrain"){
//            QString b = data.split("VisibleTerrain").last();
    //        qDebug() << "b: " << b;
//            qDebug() << "\tClearing parsedView";
            for(int y = 0; y < 10; y++)
            {
                for(int x = 0; x < 18; x++)
                {
                    parsedView[y][x] = 0;
                }
            }

//            qDebug() << "\tSplitting data";
            QStringList  pieces = subSet.last().split(";");
//            qDebug() << "Pieces: " << pieces;
            //If the server is indicating the game has ended
            for(int i=0; i<pieces.length()-1; i++){
                QString p = pieces.at(i);
                if(!p.isEmpty() && !p.isNull())
                {
                    QStringList arraySet = pieces.at(i).split(":");
//                    qDebug()<<"test "<<arraySet.length()<<" "<<pieces.at(i);
//                    qDebug() << arraySet.at(1) << " " << arraySet.at(2) << " " << arraySet.at(0);
                    int x = arraySet.at(2).toInt();
                    int y = arraySet.at(1).toInt();
                    parsedView[y][x] = arraySet.at(0).toInt();
                }
            }

//            qDebug() << "\tFormatting display string";
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
            disp=disp+"\n\n\n";
//            qDebug() << "\tSetting display string";
            ui->Log->setText(disp);

//            qDebug() << "\tDone";

            rch->SetInputs(parsedView);
            this->update();
        }
        else if(command=="LevelReset")
        {
            rch->LevelReset();
            rch->InitializeRun(true);
        }
        else if(command == "NextFrame")
        {
            rch->FrameUpdated();
            rch->SetPosition(subSet.last().toInt());
        }
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
    ui->keyStateNumLabel->setText(QString("0b%1").arg(ksu, 4, 2, QChar('0')));
    QByteArray bArr;
    bArr.append("Controls;");
    bArr.append(QString::number(ksu, 2));
    bArr.append(";");
    socket->write(bArr);

    if((ksu & 0b0001) == 0b0001)
        ui->leftLabel->setStyleSheet("QLabel { background-color: lightblue; }");
    else
        ui->leftLabel->setStyleSheet("QLabel { background-color: transparent; }");

    if((ksu & 0b0010) == 0b0010)
        ui->rightLabel->setStyleSheet("QLabel { background-color: lightblue; }");
    else
        ui->rightLabel->setStyleSheet("QLabel { background-color: transparent; }");

    if((ksu & 0b0100) == 0b0100)
        ui->jumpLabel->setStyleSheet("QLabel { background-color: lightblue; }");
    else
        ui->jumpLabel->setStyleSheet("QLabel { background-color: transparent; }");

    if((ksu & 0b1000) == 0b1000)
        ui->sprintLabel->setStyleSheet("QLabel { background-color: lightblue; }");
    else
        ui->sprintLabel->setStyleSheet("QLabel { background-color: transparent; }");
}

void MainWindow::ResetLevel()
{
//    qDebug() << "\t\t\tResetLevel()";
    QByteArray bArr;
    bArr.append("Reset;");
    socket->write(bArr);
//    qDebug() << "\t\t\t--Finished ResetLevel()";
}

void MainWindow::genomeStatus(int num)
{
    ui->genomeNumLabel->setText(QString::number(num));
}

void MainWindow::speciesStatus(int num, int size)
{
    ui->speciesNumLabel->setText(QString::number(num) + " / " + QString::number(size));
}

void MainWindow::generationStatus(int num)
{
    ui->genNumberLabel->setText(QString::number(num));
}

void MainWindow::fitnessUpdate(int num)
{
    ui->fitnessNumLabel->setText(QString::number(num));
}

void MainWindow::maxFitnessUpdate(int num)
{
    ui->maxFitNumLabel->setText(QString::number(num));
}

void MainWindow::ResetUpdate(bool stat)
{
    if(stat)
        ui->resetLabel->setStyleSheet("QLabel { background-color: red; }");
    else
        ui->resetLabel->setStyleSheet("QLabel { background-color: transparent; }");
}

void MainWindow::timeoutValue(float num)
{
    ui->timeoutValueLabel->setText(QString::number(num));
}

void MainWindow::on_closePB_clicked()
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        ResetLevel();
        Disconnected();
    }

    if(rch->isRunning())
        rch->end();

    this->close();
}

void MainWindow::on_resetNNPB_clicked()
{
    if(rch->isRunning())
    {
        rch->ResetNN();
    }

    if(socket->state() == QAbstractSocket::ConnectedState)
        ResetLevel();
}

void MainWindow::on_saveNNPb_clicked()
{
    QString filepath = "States/usersave_" + ui->filenamePb->text();
    qDebug() << filepath;
    if(filepath.contains("#"))
        filepath.replace("#", QString::number(rch->GetPool()->GetGeneration()));
    qDebug() << filepath;
    rch->SaveFile(filepath);
}

void MainWindow::on_loadNNPb_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Save"), "States", tr("Save State files (*.json)"));
    qDebug() << filename;
    rch->LoadFile(filename);
    rch->LevelReset();
    ResetLevel();
    rch->InitializeRun(false);
}
