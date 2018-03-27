#include "player.h"
#include <QDebug>

Player::Player()
{

}

Player::Player(int parentWidth, int parentHeight)
{
    posX = 130; //(parent->width() / 5) + 10;
    size.x = 95;
    size.y = 93;
    posY = parentHeight - 89;

    idleImagePath = QString("Assets/Idle/%1/Idle(%2).png");
    runImagePath = QString("Assets/Running/%1/Run(%2).png");
    jumpImagePath = QString("Assets/Jumping/%1/Jump(%2).png");
    slideImagePath = QString("Assets/Rolling/%1/Roll(%2).png");
    fallImagePath = QString("Assets/Falling/%1/Fall(%2).png");
    victoryImagePath = QString("Assets/Victory/Victory(%1).png");

    frame = 1;
    framePerSecondCount = 0;
    heightDelta = 0;
    lastHeight = 0;

    image = new QPixmap(idleImagePath.arg(frame));
    lastActionPressed = 0;
    playerDirection = 1;
    speedX = PLAYER_IDLE_VELOCITY;
    jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
    speedY = PLAYER_INITIAL_Y_VELOCITY;

    rectPosX = posX + 15;// + 2;
    rectPosY = posY + 8; //parent->height() - 140;
    rectSizeX = size.x - 30;// - 5;
    rectSizeY = size.y - 15;

    boundingBox = new QRect(posX + 8, posY, 30, size.y);
    jumpBB = new QRect(posX + 13, posY - 5, 30, 5);
    jumpViewBB = NULL;
    fallBB = new QRect(posX + 13, posY + size.y, 30, 40);
    fallViewBB = NULL;

    leftBound = parentWidth / 6;
    rightBound = parentWidth - (parentWidth / 5);
    ground = parentHeight;

    jumping = false;
    playerOnObstacle = true;
    pState = lastState = nextState = IDLE;

    playerPixmap = NULL;
    playerBB = NULL;

    qDebug() << "Ground: " << ground;
    qDebug() << "Player height: " << size.y;
    qDebug() << "Player pos (Y): " << posY;
}

Player::~Player()
{
    delete image;
    delete playerPixmap;
    delete playerBB;
    delete fallBB;
    delete jumpBB;
    delete boundingBox;

}//Destructor

Size Player::getSize()
{
    return size;
}

void Player::drawPlayer(QPainter &painter, bool devMode)
{
    if(devMode)
    {
        painter.drawText(20, 70, QString("Player last state: %1").arg(PlayerStateStrings[lastState]));
        painter.drawText(20, 80, QString("Player state: %1").arg(PlayerStateStrings[pState]));
        painter.drawText(20, 90, QString("Player next state: %1").arg(PlayerStateStrings[nextState]));
        painter.drawText(20, 100, QString("Jump Speed:   %1").arg(jumpSpeed));
        painter.drawText(20, 110, QString("X Speed:  %1").arg(speedX));
        painter.drawText(20, 120, QString("Y Speed: %1").arg(speedY));
        painter.drawText(20, 130, QString("Left Wall collision: %1").arg(leftWallCollided));
        painter.drawText(20, 140, QString("Right wall collision: %1").arg(rightWallCollided));
        painter.drawText(20, 150, QString("On obstacle: %1").arg(playerOnObstacle));
        painter.drawText(20, 160, QString("Jumping: %1").arg(jumping));
        painter.drawText(20, 170, QString("Should player spring: %1").arg(shouldPlayerSprint));
        painter.drawText(20, 180, QString("Frame per second count: %1").arg(framePerSecondCount));
    }

    painter.drawText(950, 70, QString( "[A]\t= Move Left"));
    painter.drawText(950, 80, QString( "[D]\t= Move Right"));
    painter.drawText(950, 90, QString( "[W]/[Space]\t= JUMP"));
    painter.drawText(950, 100, QString("[LShift]\t= Sprint"));

}//Draws the player


void Player::changeImage(QString str)
{
    delete image;
    image = new QPixmap(str);
    playerPixmap->setPixmap(*image);
}//Allows the player icon to be changed


void Player::playerScreenPos()
{
    if(((RIGHT == lastActionPressed) && pState != RUNNING_LEFT && pState != JUMPING) && (this->posX + speedX < rightBound) && !rightWallCollided)
    {
        this->setPosX(this->GetPosX() + speedX);
    }
    else if(((LEFT == lastActionPressed) && pState != RUNNING_RIGHT && pState != JUMPING) && (this->posX > leftBound) && !leftWallCollided)
    {
        this->setPosX(this->GetPosX() - speedX);
    }
    else
    {
        this->setPosX(this->GetPosX());
    }
}

void Player::Reset()
{
    lastState = IDLE;
    pState = IDLE;
    nextState = IDLE;
    lastActionPressed = NONE;
    playerDirection = EAST;
    jumping = false;
    playerOnObstacle = true;
    speedX = PLAYER_INITIAL_X_VELOCITY;
    speedY = PLAYER_INITIAL_Y_VELOCITY;
    jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
    frame = 1;
    heightDelta = 0;
    lastHeight = 0;
    framePerSecondCount = 0;
    clearWallCollided();
}

PlayerState Player::getState()
{
    return pState;
}

PlayerState Player::GetLastState()
{
    return lastState;
}

QPixmap *Player::GetImage()
{
    return image;
}

QRect *Player::GetBoundingBox()
{
    return boundingBox;
}

QRect *Player::GetJumpBB()
{
    return jumpBB;
}

QRect *Player::GetFallBB()
{
    return fallBB;
}

QGraphicsPixmapItem *Player::GetViewPixmap()
{
    return playerPixmap;
}

QGraphicsRectItem *Player::GetViewBB()
{
    return playerBB;
}

QGraphicsRectItem *Player::GetFallViewBB()
{
    return fallViewBB;
}

QGraphicsRectItem *Player::GetJumpViewBB()
{
    return jumpViewBB;
}

void Player::SetLevelBounds(int l, int r)
{
    leftBound = l;
    rightBound = r;
}

void Player::SetViewPixmap(QGraphicsPixmapItem *item)
{
    this->playerPixmap = item;
    playerPixmap->setScale(0.5);
    playerPixmap->setPos(posX, posY);
}

void Player::SetViewBB(QGraphicsRectItem *item)
{
    this->playerBB = item;
    playerBB->setRect(posX, posY, 40, 93);
    playerBB->setPos(0,0);
}

void Player::SetFallBB(QGraphicsRectItem *item)
{
    fallViewBB = item;
}

void Player::SetJumpBB(QGraphicsRectItem *item)
{
    jumpViewBB = item;
}

void Player::UpdatePlayer(GameView *view)
{
    switch(pState)
    {
    case IDLE:
        standBy();
        break;
    case RUNNING_RIGHT:
        run();
        break;
    case RUNNING_LEFT:
        runInverted();
        break;
    case JUMPING:
        jump();
        break;
    case FALLING:
        fall();
        break;
    case SLIDING:
        slide();
        break;
    case PAUSED:
        pausePlayer();
        break;
    case VICTORY:
        Celebrate();
        break;
    }

    // Scrolls the screen left or right as long as the player is not near the edge of the level.
    if((posX - 200 > leftBound) && (posX + 200 < rightBound) && (posY < (ground + (4 * 70))));
        view->ensureVisible(playerPixmap, 200, 70);
}

void Player::UpdateFrame()
{
    if(pState != VICTORY)
        if(framePerSecondCount % 5 == 0)
            frame++;

    if(pState == JUMPING)
    {
        if(frame > JUMP_FRAME_COUNT) { frame = 1; }
    }
    else if(pState == RUNNING_LEFT || pState == RUNNING_RIGHT)
    {
        if(frame > RUN_FRAME_COUNT) { frame = 1; }
    }
    else if(pState == IDLE)
    {
        if(frame > IDLE_FRAME_COUNT) { frame = 1; }
    }
    else if (pState == SLIDING)
    {
        if(frame > SLIDE_FRAME_COUNT) { frame = 1; pState = IDLE; lastState = SLIDING;}
    }
    else if (pState == FALLING)
    {
        if(frame > FALLING_FRAME_COUNT) { frame = FALLING_FRAME_COUNT; pState = FALLING; }
    }
    else if (pState == VICTORY)
    {
        if(frame > VICTORY_FRAME_COUNT) { frame = 1; }
    }
}

void Player::playerAction(int action, bool sprint, bool bonusHit)
{
    framePerSecondCount++;
    if(framePerSecondCount > 60) framePerSecondCount = 0;

    if(pState != VICTORY)
    {
        if(action == DOWN)
            action = NONE;

        shouldPlayerSprint = sprint;

        switch(action)
        {
        case RIGHT:
            if(pState == FALLING || pState == JUMPING) { nextState = RUNNING_RIGHT; speedX = PLAYER_INITIAL_X_VELOCITY; break; }
            else if(pState != RUNNING_RIGHT)
            {
                lastState = pState;
                pState = RUNNING_RIGHT;
                if(speedX < PLAYER_INITIAL_X_VELOCITY)
                    speedX = PLAYER_INITIAL_X_VELOCITY;
            }
            break;
        case UP:
            if(pState == JUMPING) { break; }
            else if(pState == FALLING && jumping) { break; }
            else if(pState == RUNNING_LEFT || pState == RUNNING_RIGHT) { nextState = pState; pState = JUMPING; speedY = PLAYER_INITIAL_Y_VELOCITY;}
            else if (pState == IDLE) { pState = JUMPING; speedY = PLAYER_INITIAL_Y_VELOCITY; }
            else { pState = JUMPING; }
            jumpStart = framePerSecondCount;
            break;
        case LEFT:
            if(pState == FALLING || pState == JUMPING) { nextState = RUNNING_LEFT; speedX = PLAYER_INITIAL_X_VELOCITY; break; }
            else if(pState != RUNNING_LEFT)
            {
                lastState = pState;
                pState = RUNNING_LEFT;
                if(speedX < PLAYER_INITIAL_X_VELOCITY)
                    speedX = PLAYER_INITIAL_X_VELOCITY;
            }
            break;
        case NONE:
            if(!playerOnObstacle && pState != JUMPING) { pState = FALLING; nextState = IDLE; }
            else if(pState == JUMPING && bonusHit) { pState = FALLING; }
            else if(pState == JUMPING && (lastHeight < 2* UNIT_SCALE_FACTOR)) { break; }
            else if(pState != IDLE) { nextState = IDLE; lastState = pState; pState = IDLE; }
            break;
        case PAUSE:
            if(pState != PAUSED) { pState = PAUSED; }
            break;
        default:
            break;
        }

        if(action != lastActionPressed)
        {
            lastActionPressed = action;
        }

    }

    this->playerScreenPos();
}//Calls the various player controlled movement functions


void Player::jump()
{
    if(pState != FALLING)
    {
        playerOnObstacle = false;
        jumping = true;
        QString dir = (playerDirection == EAST) ? "Right" : "Left";

        int currentFrame = framePerSecondCount;
        if(currentFrame < jumpStart) currentFrame += 60;
        float height = (std::pow(((float)(currentFrame - jumpStart) / 60.0f), 2.0f) * GRAVITY_FACTOR) / 2.0f;
        heightDelta = height - lastHeight;
        qDebug() << "Height: " << height << " heightDelta: " << heightDelta << " TimeDelta: " << (currentFrame - jumpStart);

        if(speedX > PLAYER_IDLE_VELOCITY)
        {
            if(nextState == RUNNING_LEFT) playerDirection = WEST;
            else if(nextState == RUNNING_RIGHT) playerDirection = EAST;

            if(playerDirection == EAST && !leftWallCollided)
                setPosX(posX + PLAYER_FALLING_X_VELOCITY);
            else if(!rightWallCollided)
                setPosX(posX - PLAYER_FALLING_X_VELOCITY);
        }

        setPosY(posY - heightDelta);

        if(height >= (5.5*UNIT_SCALE_FACTOR))
        {
            frame = 1;
            pState = FALLING;
            lastState = JUMPING;
            heightDelta = 0;
            lastHeight = 0;
        }

        lastHeight = height;


        changeImage(jumpImagePath.arg(dir).arg(frame));
    }
}//Controls Player Jumps

void Player::slide()
{
    if(/*((posY + size.y) == ground || */playerOnObstacle/*)*/ && (!leftWallCollided || !rightWallCollided))
    {
        speedX *= COEFF_OF_FRICTION;

        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;

        if(playerDirection == WEST && !leftWallCollided)
        {
            posX -= (5 * speedX);
            rectPosX -= (5 * speedX);
            changeImage(slideImagePath.arg("Left").arg(frame));
        }
        else if(playerDirection == EAST && !rightWallCollided)
        {
            posX += (5 * speedX);
            rectPosX += (5 * speedX);
            changeImage(slideImagePath.arg("Right").arg(frame));
        }

        if(speedX <= 0.5f)
        {
            lastState = SLIDING;
            pState = IDLE;
        }
    }
//    else
//    {
//        posY += (40 * GRAVITY_FACTOR);
//        rectPosY += (40 * GRAVITY_FACTOR);

//        changeImage(fallImagePath.arg((playerDirection == WEST) ? "Left" : "Right").arg(frame));

//        if((posY + size.y) >= ground && !inGap)
//        {
//            this->setPosY(ground - 89);
//            rectPosY = ground - 89;
//            lastState = FALLING;
//            pState = IDLE;
//            speedX = PLAYER_INITIAL_X_VELOCITY;
//            jumping = false;
//            onGround = true;
//        }
//    }
}


void Player::run()
{
    leftWallCollided = false;

    if(pState == FALLING)
    {
        posX += PLAYER_FALLING_X_VELOCITY;
        playerDirection = EAST;
    }
    else if(!jumping)
    {
        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
        speedY = PLAYER_INITIAL_Y_VELOCITY;

        if(shouldPlayerSprint)
        {
            speedX += (speedX * PLAYER_DRAG_COEFF);
            if(speedX >= PLAYER_MAX_X_VELOCITY) { speedX = PLAYER_MAX_X_VELOCITY; }
        }
        else
        {
            speedX += (speedX * PLAYER_DRAG_COEFF);
            if(speedX >= PLAYER_WALK_VELOCITY) { speedX = PLAYER_WALK_VELOCITY; }
        }

        changeImage(runImagePath.arg("Right").arg(frame));
        playerDirection = EAST;
    }

}//Controls Player Running right


void Player::runInverted()
{
    rightWallCollided = false;

    if(pState == FALLING)
    {
        posX -= PLAYER_FALLING_X_VELOCITY;
        playerDirection = WEST;
    }
    else if(!jumping)
    {
        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
        speedY = PLAYER_INITIAL_Y_VELOCITY;

        if(shouldPlayerSprint)
        {
            speedX += (speedX * PLAYER_DRAG_COEFF);
            if(speedX >= PLAYER_MAX_X_VELOCITY) { speedX = PLAYER_MAX_X_VELOCITY; }
        }
        else
        {
            speedX += (speedX * PLAYER_DRAG_COEFF);
            if(speedX >= PLAYER_WALK_VELOCITY) { speedX = PLAYER_WALK_VELOCITY; }
        }

        changeImage(runImagePath.arg("Left").arg(frame));
        playerDirection = WEST;
    }
}//Controls Player Running Left


void Player::standBy()
{
    running = false;
    jumping = false;
    speedX = PLAYER_IDLE_VELOCITY;
    jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
    speedY = PLAYER_INITIAL_Y_VELOCITY;
    lastHeight = 0;
    heightDelta = 0;

    //Checks which direction the player was moving last then sets the appropiate standing image
    if(EAST == playerDirection)
    {
        changeImage(idleImagePath.arg("Right").arg(frame));
    }
    else if(WEST == playerDirection)
    {
        changeImage(idleImagePath.arg("Left").arg(frame));
    }
}

void Player::pausePlayer()
{

}//Controls Player Stopped

void Player::fall()
{
    if(!playerOnObstacle)
    {
        setPosY(posY + (PLAYER_Y_PX_PER_UPDATE * 2));

        changeImage(fallImagePath.arg((playerDirection == WEST) ? "Left" : "Right").arg(frame));

        if((nextState == RUNNING_LEFT) && !leftWallCollided)
        {
            setPosX(posX - PLAYER_FALLING_X_VELOCITY);
            playerDirection = WEST;
        }
        else if((nextState == RUNNING_RIGHT) && !rightWallCollided)
        {
            setPosX(posX + PLAYER_FALLING_X_VELOCITY);
            playerDirection = EAST;
        }
    }
}

void Player::Celebrate()
{
    static int celebrationCount = 0, totalCelebrations = 0;
    if(!playerOnObstacle)
    {
        setPosY(posY + (PLAYER_Y_PX_PER_UPDATE * 2));

        if((posY + size.y) >= ground)
        {
            SetOnObstactle(true, ground);
            posX += PLAYER_X_PX_PER_UPDATE;
        }
    }
    else
    {
        changeImage(victoryImagePath.arg(frame));
        celebrationCount++;

        if(celebrationCount == 5)
        {
            celebrationCount = 0;
            frame = ((frame + 1) % VICTORY_FRAME_COUNT) + 1;

            if(frame == VICTORY_FRAME_COUNT)
                totalCelebrations++;
        }

        if(totalCelebrations == 4)
            pState = IDLE;
    }
}

int Player::getLeftBound()
{
    return leftBound;
}//Accessor


int Player::getRightBound()
{
    return rightBound;
}//Accessor


int Player::getPlayerDirection()
{
    return playerDirection;
}//Accessor


int Player::getGround()
{
    return ground;
}

bool Player::isJumping()
{
    return jumping;
}//Accessor

bool Player::isOnObstacle()
{
    return playerOnObstacle;
}

void Player::setSpeedX(int spd)
{
    speedX=spd;
}

void Player::setRectPosX(int x)
{
    this->rectPosX = x;
}

void Player::setRectPosY(int y)
{
    this->rectPosY = y;
}

void Player::SetVictory()
{
    pState = VICTORY;
}

void Player::SetOnObstactle(bool onObs, int obsY)
{
//    qDebug() << "Setting on obstacle" << onObs;
    playerOnObstacle = onObs;
    jumping = false;
    frame = 1;

    if(onObs)
    {
        if(nextState == RUNNING_LEFT)
        {
            pState = RUNNING_LEFT;
        }
        else if(nextState == RUNNING_RIGHT)
        {
            pState = RUNNING_RIGHT;
        }
        else if(pState != VICTORY)
        {
            pState = IDLE;
            speedX = PLAYER_IDLE_VELOCITY;
        }

        lastState = FALLING;
        nextState = IDLE;

        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
        setPosY(obsY - 89);
        heightDelta = 0;
        lastHeight = 0;
    }
    else
    {
        pState = FALLING;
        jumping = true;
        speedX = PLAYER_FALLING_X_VELOCITY;
    }
}

int Player::GetPixmapX()
{
    return playerPixmap->x();
}

int Player::GetPixmapY()
{
    return playerPixmap->y();
}

int Player::GetBBX()
{
    return playerBB->x();
}

int Player::GetBBY()
{
    return playerBB->y();
}//Accessor

void Player::setPosX(int x)
{
    posX=x;

    if(playerPixmap != NULL)
        playerPixmap->setPos(posX, posY);

    if(playerBB != NULL)
    {
        playerBB->setRect(posX + 13, posY + 40, 34, 28);
        playerBB->setPos(0, 0);
    }

    if(jumpViewBB != NULL)
    {
        jumpViewBB->setRect(posX + 8, posY, 44, 40);
        jumpViewBB->setPos(0,0);
    }

    if(fallViewBB != NULL)
    {
        fallViewBB->setRect(posX + 15, posY + 69, 30, 30);
        fallViewBB->setPos(0,0);
    }

}//Mutator


void Player::setPosY(int y)
{
    posY=y;

    if(playerPixmap != NULL)
        playerPixmap->setPos(posX, posY);

    if(playerBB != NULL)
    {
        playerBB->setRect(posX + 13, posY + 40, 34, 28);
        playerBB->setPos(0, 0);
    }

    if(jumpViewBB != NULL)
    {
        jumpViewBB->setRect(posX + 8, posY, 44, 40);
        jumpViewBB->setPos(0,0);
    }

    if(fallViewBB != NULL)
    {
        fallViewBB->setRect(posX + 15, posY + 69, 30, 30);
        fallViewBB->setPos(0,0);
    }
}

void Player::setSize(Size s)
{
    size = s;
}//Mutator

void Player::setJumping(bool jump)
{
    this->jumping = jump;
}//Mutator

void Player::setWallCollided(int side, bool wallCollided)
{
    qDebug() << "Setting wall collided: " << side << " " << wallCollided;
    if(side == WEST)
    {
        this->leftWallCollided = wallCollided;
        this->rightWallCollided = !wallCollided;
    }
    else if(side == EAST)
    {
        this->rightWallCollided = wallCollided;
        this->leftWallCollided = !wallCollided;
    }
}

void Player::clearWallCollided()
{
    this->leftWallCollided = false;
    this->rightWallCollided = false;
}//Mutator

int Player::GetPosX()
{
    return posX;
}//Accessor


int Player::GetPosY()
{
    return posY;
}//Accessor
