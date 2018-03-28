#include "supercoprmgame.h"

SuperCopRMGame::SuperCopRMGame(QWidget *parent, bool industrialGraphics)
    : QWidget(parent)
{
    showDevOpts = true;

//    QWidget::setWindowState(Qt::WindowFullScreen);
    QWidget::setFixedSize(1280, 720);

    if(showDevOpts)
    {
        qDebug() << "Height: " << this->height();
        qDebug() << "Creating LevelBase...";
    }

    lb = new LevelBase(this->width(), this->height());
    if(industrialGraphics)
        currentLevelType = INDUSTRIAL;
    else
        currentLevelType = GRASS;

    if(showDevOpts)
        qDebug() << "Loading player";

    player = new Player(this->width(), lb->getGround());

    if(showDevOpts)
        qDebug() << "Loading Level...";

    view = new GameView(this);
    view->setGeometry(0, 0 , 1280, 720);

    InitLevel(currentLevelType);

    msg = new QMessageBox();
    pbox = new QMessageBox();

    scoreFont = new QFont(this->font());
    scoreFont->setPointSize(24);

    pausedFont = new QFont(this->font());
    pausedFont->setPointSize(48);

    connect(lb, &LevelBase::EnemyDefeated, this, &SuperCopRMGame::scoreUpdate);
    connect(lb, &LevelBase::EndOfGame, this, &SuperCopRMGame::GameOver);

    if(showDevOpts)
        qDebug() << "Setting Background...";

    QPalette palette;

    if(!industrialGraphics)
    {
        QPixmap bkgnd("Assets/UI/grass_background.png");
        bkgnd = bkgnd.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
        palette.setBrush(QPalette::Background, bkgnd);
    }
    else
    {
        QPixmap bkgnd("Assets/UI/fire_background.png");
        bkgnd = bkgnd.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio);
        palette.setBrush(QPalette::Background, bkgnd);
    }

    this->setPalette(palette);

    if(showDevOpts)
        qDebug() << "Creating Timers...";

    timer = new QTimer();
    timer->setInterval(1000/60);
    connect(timer, &QTimer::timeout, this, &SuperCopRMGame::updateField);

//    renderTimer = new QTimer();
//    renderTimer->setInterval(17);
//    connect(renderTimer, &QTimer::timeout, this, &SuperCopRMGame::updateRender);

    keyTimer = new QTimer();
    keyTimer->setInterval(5);
    connect(keyTimer, &QTimer::timeout, this, &SuperCopRMGame::pollKey);

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
    vLayout->addWidget(view);
    vLayout->addLayout(hLayout);

    setLayout(vLayout);

    timer->start();
    keyTimer->start();

    if(showDevOpts)
        qDebug() << "Done.";
}

SuperCopRMGame::~SuperCopRMGame()
{
    delete lb;

    delete player;

    delete view;
}

void SuperCopRMGame::keyPressEvent(QKeyEvent *evt)
{
    switch(evt->key())
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
    case Qt::Key_Space:
        keyPressState |= 0b0100;
        isUpPressed = true;
        break;
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

void SuperCopRMGame::keyReleaseEvent(QKeyEvent *evt)
{
    switch(evt->key())
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
    case Qt::Key_Space:
        keyPressState &= 0b1011;
        isUpPressed = false;
        break;
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
        isDownPressed = false;
        break;
    default:
        break;
    }
}

void SuperCopRMGame::setLastKeyPress(int keyPress)
{
    this->lastKeyPress = keyPress;
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
        lastKeyPress = 0;
}

void SuperCopRMGame::updateField()
{
    if(!gamePaused)
    {
        player->playerAction(keyPressState, isSprintPressed);
        player->UpdateFrame();
        player->UpdatePlayer(view);
        lb->UpdateLevel(player, view, showDevOpts);
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
    timer->stop();
    keyTimer->stop();
    this->close();
}

void SuperCopRMGame::actionInput(Qt::Key key)
{
    qDebug() << "Action Input";
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

        lb->ClearView(view);
        InitLevel(currentLevelType);
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

void SuperCopRMGame::paintEvent(QPaintEvent *e)
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
        if(currentLevelType == GRASS)
            painter.setPen(QPen(Qt::black));
        else
            painter.setPen(QPen(Qt::white));

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
}

void SuperCopRMGame::InitLevel(LevelType lt)
{
    lb->LoadLevel(1, view, showDevOpts);

    if(lt != GRASS) // Since we only have one level, GRASS is its default.
        lb->SetLevelType(lt);

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
