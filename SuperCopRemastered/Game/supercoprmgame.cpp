#include "supercoprmgame.h"

SuperCopRMGame::SuperCopRMGame(QWidget *parent)
    : QWidget(parent)
{
//    QWidget::setWindowState(Qt::WindowFullScreen);
    QWidget::setFixedSize(1280, 720);
    qDebug() << "Loading player";
    player = new Player(this->width(), this->height());
    qDebug() << "Creating LevelBase...";
    lb = new LevelBase(this->width(), this->height());
    qDebug() << "Loading Level...";
    lb->LoadLevel(1);

    msg = new QMessageBox();
    pbox = new QMessageBox();

    scoreFont = new QFont(this->font());
    scoreFont->setPointSize(24);

    pausedFont = new QFont(this->font());
    pausedFont->setPointSize(48);
    qDebug() << "Setting Background...";
    QPixmap bkgnd("Assets/UI/background.png");
    bkgnd = bkgnd.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    qDebug() << "Creating Timers...";
    timer = new QTimer();
    timer->setInterval(50);
    connect(timer, &QTimer::timeout, this, &SuperCopRMGame::updateField);

    keyTimer = new QTimer();
    keyTimer->setInterval(5);
    connect(keyTimer, &QTimer::timeout, this, &SuperCopRMGame::pollKey);

    isUpPressed = false;
    isDownPressed = false;
    isLeftPressed = false;
    isRightPressed = false;
    gamePaused = false;
    lastKeyPress = 0;
    gamescore=0;
    location=0;

    //Initializes all Vector Elements;
    qDebug() << "Loading vectors from file...";
    this->setVecs();

    showDevOpts = false;
    resume = new QPushButton("RESUME");
    resume->hide();
    connect(resume, &QPushButton::clicked, this, &SuperCopRMGame::resumeGame);

    exit = new QPushButton("EXIT");
    exit->hide();
    connect(exit, &QPushButton::clicked, this, &SuperCopRMGame::exitGame);

    paused = new QLabel("PAUSED");
    paused->setFont(*pausedFont);
    paused->hide();

    hLayout = new QHBoxLayout();
    hLayout2 = new QHBoxLayout();
    vLayout = new QVBoxLayout();

    hLayout->addWidget(resume);
    hLayout->addWidget(exit);
    hLayout2->addSpacing(this->width() / 3);
    hLayout2->addWidget(paused);
    hLayout2->addSpacing(this->width() / 4);
    vLayout->addLayout(hLayout2);
    vLayout->addLayout(hLayout);

    setLayout(vLayout);

    timer->start();
    keyTimer->start();
    qDebug() << "Done.";
}

SuperCopRMGame::~SuperCopRMGame()
{

}

void SuperCopRMGame::keyPressEvent(QKeyEvent *evt)
{
    switch(evt->key())
    {
    case Qt::Key_Right:
        isRightPressed = true;
        break;
    case Qt::Key_Down:
        isDownPressed = true;
        break;
    case Qt::Key_Up:
        isUpPressed = true;
        break;
    case Qt::Key_Left:
        isLeftPressed = true;
        break;
    case Qt::Key_Escape:
        if(!gamePaused)
        {
            gamePaused = true;
            resume->show();
            exit->show();
            paused->show();
        }
        else
        {
            gamePaused = false;
            resume->hide();
            exit->hide();
            paused->hide();
        }
        break;
    default:
        break;
    }
}

void SuperCopRMGame::keyReleaseEvent(QKeyEvent *evt)
{
    switch(evt->key())
    {
    case Qt::Key_Right:
        isRightPressed = false;
        break;
    case Qt::Key_Down:
        isDownPressed = false;
        break;
    case Qt::Key_Up:
        isUpPressed = false;
        break;
    case Qt::Key_Left:
        isLeftPressed = false;
        break;
    default:
        break;
    }
}

void SuperCopRMGame::setLastKeyPress(int keyPress)
{
    this->lastKeyPress = keyPress;
}

void SuperCopRMGame::obstacleMovement()
{
    for(unsigned int i = 0; i < enemies.size(); i++)
    {
        for(unsigned int j = 0; j < walls.size(); j++)
        {
            QRect enemyRect, wallRect;
            enemyRect = QRect((*(enemies.at(i))).getPosX(),(*(enemies.at(i))).getPosY(),(*(enemies.at(i))).getSize().x,(*(enemies.at(i))).getSize().y);
            wallRect = QRect((*(walls.at(j))).getWallPosX(),(*(walls.at(j))).getWallPosY(),(*(walls.at(j))).getWallSize().x,(*(walls.at(j))).getWallSize().y);

            if(enemyRect.intersects(wallRect) && (*(walls.at(j))).isActive())
            {
                if(0==(*(enemies.at(i))).getDirection())
                {
                    if((*(walls.at(j))).getWallPosX() + (*(walls.at(j))).getWallSize().x > (*(enemies.at(i))).getPosX() && (*(walls.at(j))).getWallPosX() + 16 < (*(enemies.at(i))).getPosX() && (*(enemies.at(i))).getPosX() < this->width())
                    {
                        (*(enemies.at(i))).setDirection(1);
                    }
                }

                if (1 == (*(enemies.at(i))).getDirection())
                {
                    if((*(walls.at(j))).getWallPosX() < (*(enemies.at(i))).getPosX() + (*(enemies.at(i))).getSize().x && (*(walls.at(j))).getWallPosX() + 16 > (*(enemies.at(i))).getPosX() + (*(enemies.at(i))).getSize().x)
                    {
                        (*(enemies.at(i))).setDirection(0);
                    }
                }
            }
        }//Enemies turn around when they hit walls

        if((*(enemies.at(i))).getActive())
        {
            if(0 == (*(enemies.at(i))).getDirection())
            {
                (*(enemies.at(i))).setPosX((*(enemies.at(i))).getPosX() - moveSpeed - 3);
            }
            else if (1 == (*(enemies.at(i))).getDirection())
            {
                (*(enemies.at(i))).setPosX((*(enemies.at(i))).getPosX() + moveSpeed + 3);
            }
        }//enemy moves based on time
    }

    if((1 == lastKeyPress || 2 == lastKeyPress) && (player->getPosX() + player->getSize().x) >= player->getRightBound()&& levelEnd->getPosX() >= 0)
    {
        for(unsigned int i = 0; i < enemies.size(); i++)
        {
            if((*(enemies.at(i))).getActive())
            {
                (*(enemies.at(i))).setPosX((*(enemies.at(i))).getPosX() - moveSpeed);
            }
        }//enemies appear to move faster when the player runs toward them

        for(unsigned int i = 0; i < platforms.size(); i++)
        {
            if((*(platforms.at(i))).isActive())
            {
                (*(platforms.at(i))).setPlatformPosX((*(platforms.at(i))).getPlatformPosX() - moveSpeed);
            }
        }//Platforms scroll

        for(unsigned int i = 0; i < walls.size(); i++)
        {
            if((*(walls.at(i))).isActive())
            {
                (*(walls.at(i))).setWallPosX((*(walls.at(i))).getWallPosX() - moveSpeed);
            }
        }//Walls scroll

        for(unsigned int i = 0; i < donuts.size(); i++)
        {
            if((*(donuts.at(i))).getActive()){
                (*(donuts.at(i))).setPosX((*(donuts.at(i))).getPosX() - moveSpeed);
            }
        }//Donuts scroll

        location+=(moveSpeed/5);
        levelEnd->setPosX(levelEnd->getPosX() - moveSpeed);
    }

    if((4 == lastKeyPress || 2 == lastKeyPress) && (player->getPosX() <= player->getLeftBound()) && 0 < location)
    {
        for(unsigned int i = 0; i < enemies.size(); i++)
        {
            if((*(enemies.at(i))).getActive())
            {
                (*(enemies.at(i))).setPosX((*(enemies.at(i))).getPosX() + moveSpeed);
            }
        }//enemies appear to move slower when the player runs away

        for(unsigned int i = 0; i < platforms.size(); i++)
        {
            if((*(platforms.at(i))).isActive())
            {
                (*(platforms.at(i))).setPlatformPosX((*(platforms.at(i))).getPlatformPosX() + moveSpeed);
            }
        }//Platforms scroll

        for(unsigned int i = 0; i < walls.size(); i++)
        {
            if((*(walls.at(i))).isActive())
            {
                (*(walls.at(i))).setWallPosX((*(walls.at(i))).getWallPosX() + moveSpeed);
            }
        }//Walls scroll

        for(unsigned int i = 0; i < donuts.size(); i++)
        {
            if((*(donuts.at(i))).getActive())
            {
                (*(donuts.at(i))).setPosX((*(donuts.at(i))).getPosX() + moveSpeed);
            }
        }//Donuts scroll

        location-=(moveSpeed/5);
        levelEnd->setPosX(levelEnd->getPosX() + moveSpeed);
    }
}

void SuperCopRMGame::pollKey()
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
    {
        //Checks if none of the keys are pressed before checking if some of the keys are pressed.
        if(!isUpPressed && !isDownPressed && !isLeftPressed && !isRightPressed && !gamePaused)
        {
            if(3 == lastKeyPress && player->isRolling())
                lastKeyPress = 3;
            else if(2 == lastKeyPress && player->isJumping())
                lastKeyPress = 2;
            else if(1 == lastKeyPress && player->isMoveRight())
                lastKeyPress = 1;
            else if (4 == lastKeyPress && player->isMoveLeft())
                lastKeyPress = 4;
            else
                lastKeyPress = 0;
        }
        else
            lastKeyPress = 0;
    }
}

void SuperCopRMGame::updateField()
{
    if(!gamePaused)
    {
        player->playerAction(lastKeyPress);
        obstacleMovement();
    }

    this->update();
}

void SuperCopRMGame::resumeGame()
{
    resume->hide();
    exit->hide();
    paused->hide();
    gamePaused = false;
}

void SuperCopRMGame::exitGame()
{
    this->close();
}

void SuperCopRMGame::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    player->drawPlayer(painter);
    lb->drawLevelBase(painter);

    //===========================================================
    //    START PHYSICS
    //===========================================================
    QRect enemyRect, playerRect, donutRect, levelEndRect, platRect, wallRect;
    playerRect = QRect(player->getRectPosX(),player->getRectPosY(),player->getRectSizeX(),player->getRectSizeY());


//    for(unsigned int i = 0; i < donuts.size(); i++)
//    {
//        donutRect = QRect((*(donuts.at(i))).getPosX(),(*(donuts.at(i))).getPosY(), (*(donuts.at(i))).getSize().x, (*(donuts.at(i))).getSize().y);

//        if(donutspawn.at(i)<=location)
//        {
//            (*(donuts.at(i))).setActive(true);
//        }//spawns a donut at each read location

//        if((*(donuts.at(i))).getActive())
//        {
//            (*(donuts.at(i))).drawDonut(painter);
//        }//Controls whether donut is painted

//        if((*(donuts.at(i))).getActive() && playerRect.intersects(donutRect)){
//            (*(donuts.at(i))).setActive(false);
//            (*(donuts.at(i))).setPosX(-10000);//Donuts may technically be reactivated by going backward, but the player cannot go back past 0, and donuts will spawn waaaay back.
//            gamescore+=10;
//        }//handles collisions with donut

//        for(unsigned int j = 0; j < walls.size(); j++)
//        {
//            wallRect = QRect((*(walls.at(j))).getWallPosX(), (*(walls.at(j))).getWallPosY(), (*(walls.at(j))).getWallSize().x, (*(walls.at(j))).getWallSize().y);
//            if(donutRect.intersects(wallRect))
//            {
//                (*(donuts.at(i))).setActive(false);
//            }
//        }//Donuts don't spawn in walls


//    }//Handles all cases of donut objects.

//    for(unsigned int i = 0; i < enemies.size(); i++)
//    {
//        enemyRect = QRect((*(enemies.at(i))).getPosX(), (*(enemies.at(i))).getPosY(), (*(enemies.at(i))).getSize().x, (*(enemies.at(i))).getSize().y);

//        if (1 == (*(enemies.at(i))).getDirection()&&this->width()==(*(enemies.at(i))).getPosX()){
//            (*(enemies.at(i))).setDirection(0);
//        }//Enemies don't wander off the right side of the screen


//        if(enemyspawn.at(i) <= location)
//        {
//            (*(enemies.at(i))).setActive(true);
//        }//spawns an enemy at each read location

//        if((*(enemies.at(i))).getActive())
//        {
//            (*(enemies.at(i))).drawEnemy(painter);
//        }//enemy moves based on time

//        if(playerRect.intersects(enemyRect) && player->isJumping() && !player->isAscending())
//        {
//            (*(enemies.at(i))).setActive(false);
//            (*(enemies.at(i))).setPosX(-100);
//        }//Kills enemy if you jump on it

//        if(playerRect.intersects(enemyRect) && (player->isOnGround() || player->isAscending()))
//        {
//            (*(enemies.at(i))).setPosY((*(enemies.at(i))).getPosY() - 1);
//            timer->stop();
//            msg->setText("Game Over");
//            msg->exec();
//            this->setHighScores();
//            this->close();
//        }//Handles game-ending collisions


//    }//Handles all cases of enemy objects.

//    for(unsigned int i = 0; i < walls.size(); i++)
//    {
//        wallRect = QRect((*(walls.at(i))).getWallPosX(), (*(walls.at(i))).getWallPosY(), (*(walls.at(i))).getWallSize().x, (*(walls.at(i))).getWallSize().y);

//        if(wallSpawn.at(i) <= location)
//        {
//            (*(walls.at(i))).setActive(true);
//        }//Spawns a wall at each read location

//        if((*walls.at(i)).isActive())
//        {
//            (*(walls.at(i))).drawWall(painter);

//        }//Controls whether wall is drawn on screen


//    }//Handles all wall objects

    for(unsigned int i = 0; i < platforms.size(); i++)
    {
        platRect = QRect((*(platforms.at(i))).getPlatformPosX(), (*(platforms.at(i))).getPlatformPosY(), (*(platforms.at(i))).getPlatformSize().x, (*(platforms.at(i))).getPlatformSize().y);

        if(platSpawn.at(i) <= location)
        {
            (*(platforms.at(i))).setActive(true);
        }//Spawns platform of each read location

        if((*platforms.at(i)).isActive())
        {
            (*(platforms.at(i))).drawPlatform(painter);

        }//Controls whether platform is drawn on screen

        if(playerRect.intersects(platRect) && (player->getPosY() < 315) && !player->isAscending())
        {
            player->setPosY(270);
            player->setRectPosY(270);
            player->setJumping(false);
            player->setOnPlatform(true);
            player->setOnWall(false);
            player->setOnGround(false);
        }
    }//handles all platform objects

    if(playerRect.intersects(platRect) && (player->getPosY() < 315) && !player->isAscending())
    {
        player->setPosY(270);
        player->setRectPosY(270);
        player->setJumping(false);
        player->setOnPlatform(true);
        player->setOnWall(false);
        player->setOnGround(false);
    }//Platform Collision handler

    for(unsigned int i=0;i<walls.size();i++)
    {
        wallRect = QRect((*(walls.at(i))).getWallPosX(), (*(walls.at(i))).getWallPosY(), (*(walls.at(i))).getWallSize().x, (*(walls.at(i))).getWallSize().y);

        if(wallSpawn.at(i)==location){
            (*(walls.at(i))).setActive(true);
        }//spawns a wall at each read location

        if((*(walls.at(i))).isActive())
        {
            (*(walls.at(i))).drawWall(painter);
        }//Controls whether wall is painted

        if(playerRect.intersects(wallRect) && (player->getPosY() < 340) && !player->isAscending())
        {
            player->setPosY(290);
            player->setRectPosY(290);
            player->setJumping(false);
            player->setOnPlatform(false);
            player->setOnWall(true);
            player->setOnGround(false);
        }
    }//Wall Collison handler

    if((player->getPosY() >= player->getGround()) && !player->isAscending() && !player->isOnWall() && !player->isOnPlatform())
    {
        player->setPosY(player->getGround());
        player->setRectPosY(player->getGround());
        player->setJumping(false);
        player->setOnPlatform(false);
        player->setOnWall(false);
        player->setOnGround(true);
    }//Ground Collision handler
    else
    {
        player->setPosY(player->getPosY() + 10);
        player->setRectPosY(player->getRectPosY() + 10);
        //        player->setJumping(true);
        player->setOnPlatform(false);
        player->setOnWall(false);
        player->setOnGround(false);
    }//Lowers Player until collision occurs

    for(unsigned int i=0;i<walls.size();i++)
    {
        wallRect = QRect((*(walls.at(i))).getWallPosX(), (*(walls.at(i))).getWallPosY(), (*(walls.at(i))).getWallSize().x, (*(walls.at(i))).getWallSize().y);

        if((player->getPosY() + 40 > (*(walls.at(i))).getWallPosY()) && (playerRect.intersects(wallRect)) && (1 == player->getPlayerDirection()))
        {
            player->setPosX((*(walls.at(i))).getWallPosX() - player->getSize().x);
            player->setRectPosX((*(walls.at(i))).getWallPosX() - player->getSize().x);
            player->setWallCollided(true);
        }//Checks for player colliding with the left side of a wall
        else if((player->getPosY() + 40 > (*(walls.at(i))).getWallPosY()) && (playerRect.intersects(wallRect)) && (-1 == player->getPlayerDirection()))
        {
            player->setPosX((*(walls.at(i))).getWallPosX()+(*(walls.at(i))).getWallSize().x);
            player->setRectPosX((*(walls.at(i))).getWallPosX()+(*(walls.at(i))).getWallSize().x + (player->getSize().x - player->getRectSizeX()));
            player->setWallCollided(true);
        }//Checks for player colliding with the left side of a wall
        else
        {
            player->setWallCollided(false);
        }//Sets flag for when player is not colliding with a wall
    }

    //===========================================================
    //    END PHYSICS
    //===========================================================

    QPen pen = QPen(Qt::white);
    painter.setPen(pen);
    painter.setFont(*scoreFont);
    painter.drawText(10, 30, QString("Score: %1").arg(QString::number(gamescore)));

    levelEnd->drawDonut(painter);
    levelEndRect = QRect(levelEnd->getPosX(), levelEnd->getPosY(), levelEnd->getSize().x, levelEnd->getSize().y);

    if(playerRect.intersects(levelEndRect))
    {
        if(true==timer->isActive()){
            gamescore += 100;
            timer->stop();
            player->setPosX(player->getPosX() + 1);
            msg->setText("Level Beaten");
            msg->exec();
            this->setHighScores();
        }
    }//Handles game winning scenario

    if(showDevOpts)
    {
        QPainter devPaint(this);
        //PlayerRect Outline
        QPen debugPen;
        debugPen.setColor(Qt::red);
        devPaint.setPen(debugPen);
        devPaint.drawRect(playerRect);

        //Game input info
        devPaint.drawText(10, 60, QString("lastActionPressed: %1").arg(QString::number(lastKeyPress)));
    }
}//Handles Painting all elements on screen


void SuperCopRMGame::setVecs(){
    QString enemyfile("Assets/Level/enemy.txt");
    QString donutfile("Assets/Level/donut.txt");
    QString wallFile("Assets/Level/wall.txt");
    QString platFile("Assets/Level/platform.txt");

    ifstream enemyread;
    enemyread.open(enemyfile.toStdString().c_str());
    int enemynum;
    if(enemyread.is_open())
    {
        while(enemyread>>enemynum)
        {
            enemyspawn.push_back(enemynum);
        }
    }
    enemyread.close();

    for(unsigned int i = 0; i < enemyspawn.size(); i++)
    {
        Enemy *enemy;
        enemy = new Enemy();
        enemies.push_back(enemy);
    }//Enemy Vector initialization

    ifstream donutread;
    donutread.open(donutfile.toStdString().c_str());
    int donutnum;
    if(donutread.is_open())
    {
        while(donutread >> donutnum)
        {
            donutspawn.push_back(donutnum);
        }
    }
    donutread.close();

    for(unsigned int i = 0; i < donutspawn.size(); i++)
    {
        Donut *donut;
        donut= new Donut(620, this->height() - 140);
        donuts.push_back(donut);
    }//Donut Vector Initialization

    ifstream wallRead;
    wallRead.open(wallFile.toStdString().c_str());
    int wallNum;
    if(wallRead.is_open())
    {
        while(wallRead >> wallNum)
        {
            wallSpawn.push_back(wallNum);
        }
    }
    wallRead.close();

    for(unsigned int i = 0; i < wallSpawn.size(); i++)
    {
        Wall *wall;
        wall = new Wall();
        walls.push_back(wall);
    }//Wall Vector Initialization

    ifstream platRead;
    platRead.open(platFile.toStdString().c_str());
    int platNum;
    if(platRead.is_open())
    {
        while(platRead >> platNum)
        {
            platSpawn.push_back(platNum);
        }
    }
    platRead.close();

    for(unsigned int i = 0; i < platSpawn.size(); i ++)
    {
        Platform *plat = new Platform(this->width(), this->height());
        platforms.push_back(plat);
    }//Platform Vector Initialization

    levelEnd = new Donut(620, this->height() - 140);
    levelEnd->setSize(Size{40, 40});
    levelEnd->setPosX(6500);
    levelEnd->setPosY(this->height() - 200);
    //Initializes game ending Donut

}//Initializes vectors

void SuperCopRMGame::setHighScores()
{
    int scorefile = moveSpeed / 5;
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


void SuperCopRMGame::setMoveSpeed(int spd)
{
    moveSpeed = spd;
    player->setSpeedX(spd);
}//Sets movement speed

void SuperCopRMGame::setShowDevOpts(bool devOpts)
{
    this->showDevOpts = devOpts;
}//Enables the developer options.
