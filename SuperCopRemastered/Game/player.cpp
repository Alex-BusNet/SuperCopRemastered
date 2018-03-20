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

    image = new QPixmap(idleImagePath.arg(frame));
    lastActionPressed = 0;
    playerDirection = 1;
    speedX = PLAYER_INITIAL_X_VELOCITY;
    jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;

    rectPosX = posX + 15;// + 2;
    rectPosY = posY + 8; //parent->height() - 140;
    rectSizeX = size.x - 30;// - 5;
    rectSizeY = size.y - 15;

    boundingBox = new QRect(posX + 8, posY, 30, size.y);
    jumpBB = new QRect(posX + 13, posY - 5, 30, 5);
    jumpViewBB = NULL;
    fallBB = new QRect(posX + 13, posY + size.y - 20, 30, 40);
    fallViewBB = NULL;

    leftBound = parentWidth / 6;
    rightBound = parentWidth - (parentWidth / 5);
    ground = parentHeight;

    jumping = false;
    onGround = true;
    playerOnObstacle = false;
    inGap = false;
    pState = lastState = IDLE;

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
        painter.drawText(20, 90, QString("Jump Speed:   %1").arg(jumpSpeed));
        painter.drawText(20, 100, QString("X Speed:  %1").arg(speedX));
        painter.drawText(20, 110, QString("Left Wall collision: %1").arg(leftWallCollided));
        painter.drawText(20, 120, QString("Right wall collision: %1").arg(rightWallCollided));
        painter.drawText(20, 130, QString("On ground: %1").arg(onGround));
        painter.drawText(20, 140, QString("On obstacle: %1").arg(playerOnObstacle));
        painter.drawText(20, 150, QString("Jumping: %1").arg(jumping));
        painter.drawText(20, 160, QString("In Gap: %1").arg(inGap));
    }

}//Draws the player


void Player::changeImage(QString str)
{
    delete image;
    image = new QPixmap(str);
    playerPixmap->setPixmap(*image);
}//Allows the player icon to be changed


void Player::playerScreenPos()
{
    if(!jumping && (RIGHT == lastActionPressed) && (this->posX + speedX < rightBound) && !rightWallCollided && (pState != IDLE))
    {
        this->setPosX(this->GetPosX() + speedX);

    }
    else if(!jumping && (LEFT == lastActionPressed) && (this->posX > leftBound) && !leftWallCollided && (pState != IDLE))
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
    lastActionPressed = NONE;
    playerDirection = EAST;
    jumping = false;
    onGround = true;
    playerOnObstacle = false;
    inGap = false;
    speedX = PLAYER_INITIAL_X_VELOCITY;
    jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
    frame = 1;
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
    if(inGap && pState != JUMPING)
        fall();
    else
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
            break;
        case VICTORY:
            Celebrate();
            break;
        }
    }

    // Scrolls the screen left or right as long as the player is not near the edge of the level.
    if((posX - 200 > leftBound) && (posX + 200 < rightBound) && (posY < (ground + (4 * 70))));
        view->ensureVisible(playerPixmap, 200, 70);
}

void Player::UpdateFrame()
{
    if(pState != VICTORY)
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

void Player::playerAction(int action)
{
    if(pState != VICTORY)
    {
        if(action == DOWN)
            action = NONE;

        //Checks which direction is being called then runs the appropriate function
        switch (action)
        {
        case RIGHT:
            if(((posY + size.y) < ground || inGap) && !playerOnObstacle && !onGround)
            {
                if(pState != FALLING)
                {
                    lastState = pState;
                    pState = FALLING;
                }
            }
            else
            {
                if(pState != RUNNING_RIGHT)
                {
                    lastState = pState;
                    pState = RUNNING_RIGHT;
                }
            }
            break;
        case UP:
            if(pState == JUMPING || pState == FALLING)
                break;
            else if(pState == RUNNING_RIGHT || pState == RUNNING_LEFT)
            {
                lastState = pState;
                pState = JUMPING;
            }
            else
            {
                pState = JUMPING;
            }
            break;
        case DOWN:
            if(pState != SLIDING && lastState != SLIDING)
            {
                if(pState != SLIDING)
                {
                    lastState = pState;
                    pState = SLIDING;
                }
            }
            else if((posY + size.y) < ground)
            {
                if(pState != FALLING)
                {
                    lastState = pState;
                    pState = FALLING;
                }
            }
            else
            {
                pState = IDLE;
            }
            break;
        case LEFT:
            if(((posY + size.y) < ground || inGap) && !playerOnObstacle && !onGround)
            {
                if(pState != FALLING)
                {
                    lastState = pState;
                    pState = FALLING;
                }
            }
            else
            {
                if(pState != RUNNING_LEFT)
                {
                    lastState = pState;
                    pState = RUNNING_LEFT;
                }
            }
            break;
        case NONE:
            if(((posY + size.y) < ground && !playerOnObstacle && !onGround) || inGap)
            {
                if(pState != FALLING)
                {
                    lastState = pState;
                    pState = FALLING;
                }
            }
            else
            {
                lastState = pState;
                pState = IDLE;
            }
            break;
        case PAUSE:
            pState = PAUSED;
            pausePlayer();
        }

        //If the new direction does not match the previous direction, reset the frame counter to zero.
        if(action != lastActionPressed)
        {
            lastActionPressed = action;
        }

    }

    this->playerScreenPos();
}//Calls the various player controlled movement functions


void Player::jump()
{
    jumping = true;
    playerOnObstacle = false;
    onGround = false;
    inGap = false;
    clearWallCollided();

    QString dir = (playerDirection == EAST) ? "Right" : "Left";

    if(pState == JUMPING)
    {
        if(lastState == RUNNING_LEFT && !leftWallCollided) { posX -= 20; rectPosX -= 20;}
        else if(lastState == RUNNING_RIGHT && !rightWallCollided) { posX += 20; rectPosX += 20; }
    }

    if(pState != FALLING)
    {
        jumpSpeed *= GRAVITY_FACTOR;

        posY -= (PLAYER_Y_PX_PER_UPDATE * jumpSpeed);
        rectPosY -= (PLAYER_Y_PX_PER_UPDATE * jumpSpeed);

        if(jumpSpeed <= 0.5f)
        {
            frame = 1;
            pState = FALLING;
            jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
        }
    }

    changeImage(jumpImagePath.arg(dir).arg(frame));
}//Controls Player Jumps

void Player::slide()
{
    if(((posY + size.y) == ground || playerOnObstacle) && (!leftWallCollided || !rightWallCollided))
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
    else
    {
        posY += (40 * GRAVITY_FACTOR);
        rectPosY += (40 * GRAVITY_FACTOR);

        changeImage(fallImagePath.arg((playerDirection == WEST) ? "Left" : "Right").arg(frame));

        if((posY + size.y) >= ground && !inGap)
        {
            this->setPosY(ground - 89);
            rectPosY = ground - 89;
            lastState = FALLING;
            pState = IDLE;
            speedX = PLAYER_INITIAL_X_VELOCITY;
            jumping = false;
            onGround = true;
        }
    }
}


void Player::run()
{
    running = true;
    leftWallCollided = false;

    if(pState == FALLING)
    {
        speedX = PLAYER_FALLING_X_VELOCITY;
        playerDirection = EAST;
    }
    else if(!jumping)
    {
        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;

//        if(speedX < 0.5f)
            speedX = PLAYER_INITIAL_X_VELOCITY;

        changeImage(runImagePath.arg("Right").arg(frame));
        playerDirection = EAST;
    }

}//Controls Player Running right


void Player::runInverted()
{
    running = true;
    rightWallCollided = false;

    if(pState == FALLING)
    {
        speedX = PLAYER_FALLING_X_VELOCITY;
        playerDirection = WEST;
    }
    else if(!jumping)
    {
//        if(speedX < 0.5f)
            speedX = PLAYER_INITIAL_X_VELOCITY;

        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;

        changeImage(runImagePath.arg("Left").arg(frame));
        playerDirection = WEST;
    }
}//Controls Player Running Left


void Player::standBy()
{
    running = false;
    jumping = false;
    speedX = PLAYER_INITIAL_X_VELOCITY;
    jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;

    //Checks which direction the player was moving last then sets the appropiate standing image
    if(EAST == playerDirection)
    {
        changeImage(idleImagePath.arg("Right").arg(frame++));
    }
    else if(WEST == playerDirection)
    {
        changeImage(idleImagePath.arg("Left").arg(frame++));
    }
}

void Player::pausePlayer()
{

}//Controls Player Stopped

void Player::fall()
{
    // Reset the jumpSpeed for falling.
    // If we don't, the player will drop like a rock
    if(jumpSpeed >= PLAYER_INITIAL_Y_VELOCITY)
        jumpSpeed = 2.4f;

    jumpSpeed *= 1 / GRAVITY_FACTOR;
    posY += (PLAYER_Y_PX_PER_UPDATE * jumpSpeed);
    rectPosY += (PLAYER_Y_PX_PER_UPDATE * jumpSpeed);

    changeImage(fallImagePath.arg((playerDirection == WEST) ? "Left" : "Right").arg(frame));

    if((posY + size.y) >= ground && !inGap)
    {
        qDebug() << "Fall() ground collision";
        posY = ground - 89;
        rectPosY = ground - 89;

        if(lastState == RUNNING_LEFT) pState = RUNNING_LEFT;
        else if(lastState == RUNNING_RIGHT) pState = RUNNING_RIGHT;
        else if(pState != VICTORY) pState = IDLE;

        lastState = FALLING;

        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
        speedX = PLAYER_INITIAL_X_VELOCITY;
        jumping = false;
        playerOnObstacle = false;
        onGround = true;
    }

    if(jumping)
    {
        if((lastActionPressed == LEFT) && !leftWallCollided)
        {
            posX -= 20;
            rectPosX -= 20;
            playerDirection = WEST;
        }
        else if((lastActionPressed == RIGHT) && !rightWallCollided)
        {
            posX += 20;
            rectPosX += 20;
            playerDirection = EAST;
        }
    }
}

void Player::Celebrate()
{
    static int celebrationCount = 0, totalCelebrations = 0;
    if(!onGround)
    {
        posY += (40 * GRAVITY_FACTOR);

        if((posY + size.y) >= ground)
        {
            onGround = true;
            posY = (ground - 89);
            posX += 70;
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

bool Player::isOnGround()
{
    return onGround;
}//Accessor

bool Player::isOnObstacle()
{
    return playerOnObstacle;
}

bool Player::isInGap()
{
    return inGap;
}//Accessor

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

void Player::setInGap(bool gap)
{
    inGap = gap;
}

void Player::SetVictory()
{
    pState = VICTORY;
}

void Player::SetOnObstactle(bool onObs)
{
    playerOnObstacle = onObs;
    onGround = false;
    inGap = false;
    jumping = false;
    frame = 1;

    if(onObs)
    {
        speedX = PLAYER_INITIAL_X_VELOCITY;
        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
    }

    if(lastActionPressed == LEFT) { pState = RUNNING_LEFT; }
    else if(lastActionPressed == RIGHT) { pState = RUNNING_RIGHT; }
    else if (!onObs) { pState = FALLING; jumping = true; speedX = PLAYER_FALLING_X_VELOCITY; }
    else { pState = IDLE; }
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
        playerBB->setRect(posX + 8, posY, 50, size.y);
        playerBB->setPos(0, 0);
    }
    if(jumpViewBB != NULL)
    {
        jumpViewBB->setRect(posX + 13, posY-5, 40, 10);
        jumpViewBB->setPos(0,0);
    }

    if(fallViewBB != NULL)
    {
        fallViewBB->setRect(posX + 8, posY + size.y-15, 50, 30);
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
        playerBB->setRect(posX + 8, posY, 50, size.y);
        playerBB->setPos(0, 0);
    }

    if(jumpViewBB != NULL)
    {
        jumpViewBB->setRect(posX + 13, posY-5, 40, 10);
        jumpViewBB->setPos(0,0);
    }

    if(fallViewBB != NULL)
    {
        fallViewBB->setRect(posX + 8, posY + size.y-15, 50, 30);
        fallViewBB->setPos(0,0);
    }
}

void Player::setSize(Size s)
{
    size = s;
}//Mutator

void Player::setOnGround(bool onGround)
{
    this->onGround = onGround;
}//Mutator


void Player::setJumping(bool jump)
{
    this->jumping = jump;
}//Mutator

void Player::setWallCollided(int side, bool wallCollided)
{
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
