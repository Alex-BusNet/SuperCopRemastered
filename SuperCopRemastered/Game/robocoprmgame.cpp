#include "robocoprmgame.h"

robocoprmgame::robocoprmgame(QWidget *parent, bool industrialGraphics) :
    QWidget(parent)
{
    showDevOpts = true;

//    QWidget::setWindowState(Qt::WindowFullScreen);
    QWidget::setFixedSize(1280, 720);

    if(industrialGraphics)
        currentLevelType = INDUSTRIAL;
    else
        currentLevelType = GRASS;

    if(showDevOpts)
    {
        qDebug() << "Height: " << this->height();
        qDebug() << "Creating LevelBase...";
    }

    lb = new LevelBase(this->width(), this->height());

    if(showDevOpts)
        qDebug() << "Loading player";

    player = new Player(this->width(), lb->getGround());

    if(showDevOpts)
        qDebug() << "Loading Level...";

    view = new GameView(this);
    view->setGeometry(0, 0 , 1280, 720);

    InitLevel();

    msg = new QMessageBox();
    pbox = new QMessageBox();

    scoreFont = new QFont(this->font());
    scoreFont->setPointSize(24);

    pausedFont = new QFont(this->font());
    pausedFont->setPointSize(48);

    connect(lb, &LevelBase::EnemyDefeated, this, &robocoprmgame::scoreUpdate);
    connect(lb, &LevelBase::EndOfGame, this, &robocoprmgame::GameOver);

    if(showDevOpts)
        qDebug() << "Setting Background...";

    QPalette palette;

    if(industrialGraphics)
    {
        QPixmap bkgnd("Assets/UI/fire_background.png");
        bkgnd = bkgnd.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
        palette.setBrush(QPalette::Background, bkgnd);
    }
    else
    {
        QPixmap bkgnd("Assets/UI/grass_background.png");
        bkgnd = bkgnd.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
        palette.setBrush(QPalette::Background, bkgnd);
    }

    this->setPalette(palette);

    if(showDevOpts)
        qDebug() << "Creating Timers...";

    timer = new QTimer();
    timer->setInterval(1000/60);
    connect(timer, &QTimer::timeout, this, &robocoprmgame::updateField);

//    renderTimer = new QTimer();
//    renderTimer->setInterval(17);
//    connect(renderTimer, &QTimer::timeout, this, &robocoprmgame::updateRender);

    keyTimer = new QTimer();
    keyTimer->setInterval(5);
    connect(keyTimer, &QTimer::timeout, this, &robocoprmgame::pollKey);

    //------------------------------------------------------------------------------------------
    // Gamepad configuration
    gpm = QGamepadManager::instance();

    gpkn = new QGamepadKeyNavigation();
    gpkn->setUpKey(Qt::Key_Up);
    gpkn->setDownKey(Qt::Key_Down);
    gpkn->setLeftKey(Qt::Key_Left);
    gpkn->setRightKey(Qt::Key_Right);

    connect(gpkn, &QGamepadKeyNavigation::upKeyChanged, this, &robocoprmgame::actionInput);
    connect(gpkn, &QGamepadKeyNavigation::downKeyChanged, this, &robocoprmgame::actionInput);
    connect(gpkn, &QGamepadKeyNavigation::leftKeyChanged, this, &robocoprmgame::actionInput);
    connect(gpkn, &QGamepadKeyNavigation::rightKeyChanged, this, &robocoprmgame::actionInput);

    //------------------------------------------------------------------------------------------

    resume = new QPushButton("RESUME");
    resume->hide();
    connect(resume, &QPushButton::clicked, this, &robocoprmgame::resumeGame);

    exit = new QPushButton("EXIT");
    exit->hide();
    connect(exit, &QPushButton::clicked, this, &robocoprmgame::exitGame);

    paused = new QLabel("PAUSED");
    paused->setFont(*pausedFont);
    paused->hide();

    hLayout = new QHBoxLayout();
    hLayout2 = new QHBoxLayout();
    vLayout = new QVBoxLayout();


    hLayout->addWidget(resume);
    hLayout->addWidget(exit);
    vLayout->addWidget(view);
    vLayout->addLayout(hLayout);

    setLayout(vLayout);

    timer->start();
    keyTimer->start();

    if(showDevOpts)
        qDebug() << "Done.";
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, 5300);
    connected=false;
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnect()));

    timer = new QTimer();
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendVisibleTerrain()));
    timer->start();
}

robocoprmgame::~robocoprmgame()
{
    delete lb;

    delete player;

    delete view;
}

void robocoprmgame::keyPressEvent(Qt::Key key)
{
    qDebug() << "Press";
    switch(key)
    {
    case Qt::Key_A:
        keyPressState |= 0b0001;
        isLeftPressed = true;
        break;
    case Qt::Key_D:
        keyPressState |= 0b0010;
        isRightPressed = true;
        break;
    case Qt::Key_Shift:
        isSprintPressed = true;
        break;
    case Qt::Key_W:
        keyPressState |= 0b0100;
        isUpPressed = true;
        break;
//    case Qt::Key_Space:
//        keyPressState |= 0b0100;
//        isUpPressed = true;
//        break;
    case Qt::Key_Right:
        keyPressState |= 0b0010;
        isRightPressed = true;
        break;
    case Qt::Key_Down:
        isDownPressed = true;
        break;
    case Qt::Key_Up:
        keyPressState |= 0b0100;
        isUpPressed = true;
        break;
    case Qt::Key_Left:
        keyPressState |= 0b0001;
        isLeftPressed = true;
        break;
    case Qt::Key_Escape:
        if(!gamePaused)
        {
            gamePaused = true;
            resume->show();
            exit->show();
        }
        else
        {
            gamePaused = false;
            resume->hide();
            exit->hide();
        }
        break;
    default:
        break;
    }
}

void robocoprmgame::keyReleaseEvent(Qt::Key key)
{
    qDebug() << "Release";
    switch(key)
    {
    case Qt::Key_A:
        keyPressState &= 0b1110;
        isLeftPressed = false;
        break;
    case Qt::Key_D:
        keyPressState &= 0b1101;
        isRightPressed = false;
        break;
    case Qt::Key_Shift:
        isSprintPressed = false;
        break;
    case Qt::Key_W:
        keyPressState &= 0b1011;
        isUpPressed = false;
        break;
//    case Qt::Key_Space:
//        keyPressState &= 0b1011;
//        isUpPressed = false;
//        break;
    case Qt::Key_Right:
        keyPressState &= 0b1101;
        isRightPressed = false;
        break;
    case Qt::Key_Up:
        keyPressState &= 0b1011;
        isUpPressed = false;
        break;
    case Qt::Key_Left:
        keyPressState &= 0b1110;
        isLeftPressed = false;
        break;
    case Qt::Key_Down:
        keyPressState &= 0b0000;
        isDownPressed = false;
        break;
    default:
        break;
    }
}

void robocoprmgame::setLastKeyPress(int keyPress)
{
    this->lastKeyPress = keyPress;
}

void robocoprmgame::pollKey()
{
    //Checks if any of the keys are pressed.
    if(isDownPressed)
        lastKeyPress = 3;
    else if(isUpPressed)
        lastKeyPress = 2;
    else if(isRightPressed)
        lastKeyPress = 1;
    else if(isLeftPressed )
        lastKeyPress = 4;
    else
        lastKeyPress = 0;
}

void robocoprmgame::updateField()
{
    if(!gamePaused)
    {
//        player->playerAction(lastKeyPress);
        player->playerAction(keyPressState, isSprintPressed);
        player->UpdateFrame();
        player->UpdatePlayer(view);
        lb->UpdateLevel(player, view, showDevOpts);
        this->update();
    }
}

void robocoprmgame::resumeGame()
{
    resume->hide();
    exit->hide();
    paused->hide();
    gamePaused = false;
}

void robocoprmgame::exitGame()
{
    timer->stop();
    keyTimer->stop();
    this->close();
}

void robocoprmgame::actionInput(Qt::Key key)
{
//    qDebug() << "Action Input";
//    switch(key)
//    {
//    case Qt::Key_Left:
//        player->playerAction(LEFT);
//        break;
//    case Qt::Key_Right:
//        player->playerAction(RIGHT);
//        break;
//    case Qt::Key_Up:
//        player->playerAction(UP);
//        break;
//    case Qt::Key_Down:
//        player->playerAction(DOWN);
//        break;
//    default:
//        player->playerAction(NONE);
//        break;
//    }
}

void robocoprmgame::updateRender()
{
    this->update();
}

void robocoprmgame::scoreUpdate(int value)
{
    gamescore += value;
}

void robocoprmgame::GameOver(bool endOfLevel)
{
    if(!endOfLevel)
    {
        QMessageBox *gameOverBox = new QMessageBox();
        gameOverBox->setText("YOU LOSE!");
        gameOverBox->exec();
        delete gameOverBox;

        lb->ClearView(view);
        InitLevel();
        player->Reset();
        view->ensureVisible(player->GetViewPixmap(), 200, 70);
    }
    else
    {
        // Victory Stuff goes here.
        paused->setText(QString("YOU WIN!"));
        gameOver = true;
        player->SetVictory();
    }

    setHighScores();
}

void robocoprmgame::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QPainter painter(this);
    player->drawPlayer(painter, showDevOpts);
    lb->drawLevelBase(painter, showDevOpts);

    //===========================================================
    //    START PHYSICS
    //===========================================================

    /*
     *           /----------------------------/|
     *          /----------------------------/ |
     *          ||          R.I.P           || |
     *          ||                          || |
     *          ||                          || |
     *          ||  Here lies the remnants  || |
     *          || of the original SuperCop || |
     *          ||      physics engine.     || |
     *          ||                          || |      ___
     *          ||                          || |   .-"   "-.
     *          ||    Apr.2015 - Mar.2018   || | .'   . ;   `.
     *          ||                          ||/ /    : . ' :  \
     *          //==========================// |   `  .-. . '  |
     *         //==========================//  |  :  (   ) ; ` |
     *        //==========================//   |   :  `-'   :  |
     *       //==========================//     \   .` ;  :   /
     *      //==========================//       `.   . '   .'
     *     //==========================//          `-.___.-'
     */

    //===========================================================
    //    END PHYSICS
    //===========================================================

    QPen pen = QPen(Qt::white);
    painter.setPen(pen);
    painter.setFont(*scoreFont);
    painter.drawText(15, 50, QString("Score: %1").arg(QString::number(gamescore)));

    if(gamePaused || gameOver)
    {
        painter.setPen(QPen(Qt::black));
        painter.setFont(*pausedFont);
        painter.drawText((this->width() / 2) - 110, 100, paused->text());
    }

    if(showDevOpts)
    {
        QPainter devPaint(this);
        //PlayerRect Outline
        QPen debugPen;
        debugPen.setColor(Qt::red);
        devPaint.setPen(debugPen);

        //Game input info
        devPaint.drawText(20, 60, QString("lastActionPressed: %1").arg(QString::number(lastKeyPress)));
    }
}//Handles Painting all elements on screen

void robocoprmgame::setHighScores()
{
    int scorefile = 0;// moveSpeed / 5;
    QString filename = "Assets/highscores"+QString::number(scorefile)+".txt";
    ifstream scoreset;
    scoreset.open(filename.toStdString().c_str());
    int scores;

    if(scoreset.is_open()){

        scoreset >> scores;
        int firstscore = scores;
        scoreset >> scores;
        int secondscore = scores;
        scoreset >> scores;
        int thirdscore = scores;
        scoreset >> scores;
        int fourthscore = scores;
        scoreset >> scores;
        int fifthscore = scores;
        scoreset.close();

        if(firstscore < gamescore)
        {
            fifthscore = fourthscore;
            fourthscore = thirdscore;
            thirdscore = secondscore;
            secondscore = firstscore;
            firstscore = gamescore;

            QMessageBox sbox;
            sbox.setText("New High Score: "+ QString::number(gamescore));
            sbox.exec();
        }
        else if(secondscore < gamescore)
        {
            fifthscore = fourthscore;
            fourthscore = thirdscore;
            thirdscore = secondscore;
            secondscore = gamescore;
        }
        else if(thirdscore < gamescore)
        {
            fifthscore = fourthscore;
            fourthscore = thirdscore;
            thirdscore = gamescore;
        }
        else if(fourthscore < gamescore)
        {
            fifthscore = fourthscore;
            fourthscore = gamescore;
        }
        else if(fifthscore<gamescore)

        {
            fifthscore = gamescore;
        }

        ofstream setscores;
        setscores.open(filename.toStdString().c_str());

        setscores << firstscore << endl;
        setscores << secondscore << endl;
        setscores << thirdscore << endl;
        setscores << fourthscore << endl;
        setscores << fifthscore << endl;

        setscores.close();
    }
}//resets high scores if new high score acheived

void robocoprmgame::setShowDevOpts(bool devOpts)
{
    this->showDevOpts = devOpts;
}

void robocoprmgame::sendVisibleTerrain()
{
    if(connected){
        QVector<QVector<int>> temp = lb->GetParsedView();
        QByteArray outData;
        outData.append("VisibleTerrain;");
        QString str ="";
        for(int i = 0; i<10;i++){
            for(int j=0;j<18;j++){
                int tmp =temp.at(i).at(j);
                if(tmp !=0){
                    str = QString::number(tmp)+":"+QString::number(i)+":"+QString::number(j)+";";
                    outData.append(str);
                }
            }
        }
        socket->write(outData);
    }
}

void robocoprmgame::InitLevel()
{
    lb->LoadLevel(1, view, showDevOpts);

    if(showDevOpts)
        qDebug() << "Loading player to Scene...";

    player->SetViewPixmap(view->addPixmap(*(player->GetImage())));

    QPen pen;
    if(showDevOpts)
        pen = QPen(Qt::black);
    else
        pen = QPen(Qt::transparent);

    player->SetViewBB(view->addRect(*player->GetBoundingBox(), pen));
    player->SetFallBB(view->addRect(*player->GetFallBB(), pen));
    player->SetJumpBB(view->addRect(*player->GetJumpBB(), pen));

    if(showDevOpts)
        qDebug() << "Setting player start position...";

    player->setPosX(lb->GetPlayerStart().x());
    player->setPosY(lb->GetPlayerStart().y());

    player->SetLevelBounds(0, lb->GetLevelRightBound());

    isUpPressed = false;
    isDownPressed = false;
    isLeftPressed = false;
    isRightPressed = false;
    gamePaused = false;
    gameOver = false;
    lastKeyPress = 0;
    gamescore=0;
    location=0;
    keyPressState = 0b0000;
}

void robocoprmgame::newConnect()
{//Triggered each time a client connects
    while (server->hasPendingConnections()){
        qDebug()<<"Has pending connections";
        //Creates a socket object
        socket = new QTcpSocket(this);
        socket=server->nextPendingConnection();
        //Connects the socket to read and disconnect functions
        connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));
        connect(socket, SIGNAL(disconnected()),this, SLOT(Disconnected()));
        connected=true;
    }
}

void robocoprmgame::Disconnected()
{//When one player disconnects, the game ends
    connected=false;
    socket->disconnectFromHost();
    qDebug() <<" Disconnected";
}

void robocoprmgame::readyRead()
{//Triggers when the client sends data andReads the data
    QString data;
    data = socket->readAll();
    qDebug() << "Player has sent" << data ;
    if(data=="JUMP"){
        //actionInput(Qt::Key_Up);
        keyPressEvent(Qt::Key_Up);
    }
    else if(data=="LEFT"){
        //actionInput(Qt::Key_Left);
        keyPressEvent(Qt::Key_Left);
    }
    else if(data=="RIGHT"){
        //actionInput(Qt::Key_Right);
        keyPressEvent(Qt::Key_Right);
    }
    else if(data=="STOP"){
        //actionInput(Qt::Key_Down);
        //keyPressEvent(Qt::Key_Down);
        keyReleaseEvent(Qt::Key_Down);
        //keyReleaseEvent(Qt::Key_Left);
        //keyReleaseEvent(Qt::Key_Right);
    }
    //ui->Log->setText(ui->Log->toPlainText()+data+'\n');
    //qDebug() << "log it" ;
    //sb->setValue(sb->maximum());
    //qDebug() << "scroll it";
}
