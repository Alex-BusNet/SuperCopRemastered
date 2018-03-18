#include "supercoprmgame.h"

SuperCopRMGame::SuperCopRMGame(QWidget *parent)
    : QWidget(parent)
{
//    QWidget::setWindowState(Qt::WindowFullScreen);
    QWidget::setFixedSize(1280, 720);
    qDebug() << "Height: " << this->height();

    qDebug() << "Creating LevelBase...";
    lb = new LevelBase(this->width(), this->height());

    qDebug() << "Loading player";
    player = new Player(this->width(), lb->getGround());

    qDebug() << "Loading Level...";
    view = new GameView(this);
    lb->LoadLevel(1, view);

    qDebug() << "Loading player to Scene...";
    player->SetViewPixmap(view->addPixmap(*(player->GetImage())));
    player->SetViewBB(view->addRect(*player->GetBoundingBox(), QPen(Qt::transparent)));
    player->SetFallBB(view->addRect(*player->GetFallBB(), QPen(Qt::transparent)));

//    player->SetLeftBB(view->addRect(*player->GetLeftBB()));
//    player->SetRightBB(view->addRect(*player->GetRightBB()));
//    player->SetJumpBB(view->addRect(*player->GetJumpBB()));

    qDebug() << "Setting player start position...";
    player->setPosX(lb->GetPlayerStart().x());
    player->setPosY(lb->GetPlayerStart().y());

    player->SetLevelBounds(0, lb->GetLevelRightBound());

    msg = new QMessageBox();
    pbox = new QMessageBox();

    scoreFont = new QFont(this->font());
    scoreFont->setPointSize(24);

    pausedFont = new QFont(this->font());
    pausedFont->setPointSize(48);

    connect(lb, &LevelBase::EnemyDefeated, this, &SuperCopRMGame::scoreUpdate);
    connect(lb, &LevelBase::EndOfGame, this, &SuperCopRMGame::GameOver);

    qDebug() << "Setting Background...";
    QPixmap bkgnd("Assets/UI/background.png");
    bkgnd = bkgnd.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    qDebug() << "Creating Timers...";
    timer = new QTimer();
    timer->setInterval(65);
    connect(timer, &QTimer::timeout, this, &SuperCopRMGame::updateField);

    renderTimer = new QTimer();
    renderTimer->setInterval(17);
    connect(renderTimer, &QTimer::timeout, this, &SuperCopRMGame::updateRender);

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

    //------------------------------------------------------------------------------------------
    // Gamepad configuration
    gpm = QGamepadManager::instance();

    gpkn = new QGamepadKeyNavigation();
    gpkn->setUpKey(Qt::Key_Up);
    gpkn->setDownKey(Qt::Key_Down);
    gpkn->setLeftKey(Qt::Key_Left);
    gpkn->setRightKey(Qt::Key_Right);

    connect(gpkn, &QGamepadKeyNavigation::upKeyChanged, this, &SuperCopRMGame::actionInput);
    connect(gpkn, &QGamepadKeyNavigation::downKeyChanged, this, &SuperCopRMGame::actionInput);
    connect(gpkn, &QGamepadKeyNavigation::leftKeyChanged, this, &SuperCopRMGame::actionInput);
    connect(gpkn, &QGamepadKeyNavigation::rightKeyChanged, this, &SuperCopRMGame::actionInput);

    //------------------------------------------------------------------------------------------

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
    vLayout->addWidget(view);
    vLayout->addLayout(hLayout);

    setLayout(vLayout);

    timer->start();
    keyTimer->start();
    qDebug() << "Done.";
}

SuperCopRMGame::~SuperCopRMGame()
{
    lb->ClearView(view);
    delete lb;

    delete player;

    delete view;
}

void SuperCopRMGame::keyPressEvent(QKeyEvent *evt)
{
    switch(evt->key())
    {
    case Qt::Key_Right:
        isRightPressed = true;
        break;
//    case Qt::Key_Down:
//        isDownPressed = true;
//        break;
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
//    case Qt::Key_Down:
//        isDownPressed = false;
//        break;
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

    if((1 == lastKeyPress || 2 == lastKeyPress) && (player->GetPosX() + player->getSize().x) >= player->getRightBound()&& levelEnd->getPosX() >= 0)
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

    if((4 == lastKeyPress || 2 == lastKeyPress) && (player->GetPosX() <= player->getLeftBound()) && 0 < location)
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
        lastKeyPress = 0;
    }
}

void SuperCopRMGame::updateField()
{
    if(!gamePaused)
    {
        player->playerAction(lastKeyPress);
        player->UpdateFrame();
        player->UpdatePlayer(view);
        lb->UpdateLevel(player, view);
        this->update();
    }

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

void SuperCopRMGame::actionInput(Qt::Key key)
{
    qDebug() << "Action Input";
    switch(key)
    {
    case Qt::Key_Left:
        player->playerAction(LEFT);
        break;
    case Qt::Key_Right:
        player->playerAction(RIGHT);
        break;
    case Qt::Key_Up:
        player->playerAction(UP);
        break;
    case Qt::Key_Down:
        player->playerAction(DOWN);
        break;
    default:
        player->playerAction(NONE);
        break;
    }
}

void SuperCopRMGame::updateRender()
{
    this->update();
}

void SuperCopRMGame::scoreUpdate(int value)
{
    gamescore += value;
}

void SuperCopRMGame::GameOver(bool endOfLevel)
{
    if(!endOfLevel)
    {
        QMessageBox *gameOverBox = new QMessageBox();
        gameOverBox->setText("YOU LOSE!");
        gameOverBox->exec();
        delete gameOverBox;
        timer->stop();
        keyTimer->stop();
        this->close();
    }
    else
    {
        // Victory Stuff goes here.
    }
}

void SuperCopRMGame::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QPainter painter(this);
    player->drawPlayer(painter);
    lb->drawLevelBase(painter);

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
     *          ||                          || |
     *          ||                          || |
     *          ||    Apr.2015 - Mar.2018   || |
     *          ||                          ||/
     *          //==========================//
     *         //==========================//
     *        //==========================//
     *       //==========================//
     *      //==========================//
     *     //==========================//
     */

    //===========================================================
    //    END PHYSICS
    //===========================================================

    QPen pen = QPen(Qt::white);
    painter.setPen(pen);
    painter.setFont(*scoreFont);
    painter.drawText(10, 30, QString("Score: %1").arg(QString::number(gamescore)));

    if(showDevOpts)
    {
        QPainter devPaint(this);
        //PlayerRect Outline
        QPen debugPen;
        debugPen.setColor(Qt::red);
        devPaint.setPen(debugPen);

        //Game input info
        devPaint.drawText(15, 60, QString("lastActionPressed: %1").arg(QString::number(lastKeyPress)));
    }
}//Handles Painting all elements on screen

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

void SuperCopRMGame::setShowDevOpts(bool devOpts)
{
    this->showDevOpts = devOpts;
}//Enables the developer options.
