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
    posY = parentHeight - 95;

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

    boundingBox = new QRect(posX + 30, posY + 45, 42, 35);
    jumpBB = new QRect(posX + 25, posY, 52, 45);
    jumpViewBB = NULL;
    fallBB = new QRect(posX + 40, posY + 75, 26, 29);
    fallViewBB = NULL;

    leftBound = parentWidth / 6;
    rightBound = parentWidth - (parentWidth / 5);
    ground = parentHeight;

    jumping = false;
    playerOnObstacle = true;
    canJump = true;
    pState = lastState = nextState = IDLE;

    playerPixmap = NULL;
    playerBB = NULL;

    qDebug() << "Ground: " << ground;
    qDebug() << "Player height: " << size.y;
    qDebug() << "Player pos (Y): " << posY;
}

Player::~Player()
{
    if(playerPixmap != NULL)
        delete playerPixmap;

    if(playerBB != NULL)
        delete playerBB;

    if(fallBB != NULL)
        delete fallBB;

    if(jumpBB != NULL)
        delete jumpBB;

    if(boundingBox != NULL)
        delete boundingBox;

    if(jumpViewBB != NULL)
        delete jumpViewBB;

    if(fallViewBB != NULL)
        delete fallViewBB;

    if(image != NULL)
        delete image;


}//Destructor

Size Player::getSize()
{
    return size;
}

void Player::drawPlayer(QPainter &painter, bool devMode)
{
    painter.setPen(QPen(Qt::black));

    if(devMode)
    {
        // Displays various bits of info about the player.
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
        painter.drawText(20, 180, QString("Last jump height: %1").arg(lastHeight));
        painter.drawText(20, 190, QString("Player Pos Y: %2").arg(posY));

        // Draws the Controls in the upper right hand corner.
        painter.drawText(1050, 70, QString( "[A]\t= Move Left"));
        painter.drawText(1050, 80, QString( "[D]\t= Move Right"));
        painter.drawText(1050, 90, QString( "[W]\t= JUMP"));
        painter.drawText(1050, 100, QString("[LShift]\t= Sprint"));
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
    if((((lastActionPressed & 0b0010) == 0b0010) && pState != RUNNING_LEFT && pState != JUMPING) && (this->posX + 100 < rightBound) && !rightWallCollided)
    {
        this->setPosX(this->GetPosX() + speedX);
    }
    else if((((lastActionPressed & 0b0001) == 0b0001) && pState != RUNNING_RIGHT && pState != JUMPING) && (this->posX > leftBound) && !leftWallCollided)
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
    canJump = true;
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

    playerBB->setRect(posX + 30, posY + 45, 42, 35);
    playerBB->setPos(0, 0);
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
    if(((posX - 500) > leftBound) && ((posX + 600) < rightBound) && (posY < (ground + (4 * 70))) && (posY > 75))
        view->ensureVisible(playerPixmap, 600, 70);
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

void Player::playerAction(uint8_t action, bool sprint, bool bonusHit)
{
    framePerSecondCount++;
    if(framePerSecondCount > 60) framePerSecondCount = 0;

    if(pState != VICTORY)
    {
        if(action == DOWN)
            action = NONE;

        shouldPlayerSprint = sprint;

        // action format: 0b[blank][jump][right][left]

        if((action & 0b0010) == 0b0010) // Is the right key pressed?
        {
            if(pState == FALLING || pState == JUMPING)
            {
                nextState = RUNNING_RIGHT;
                speedX = PLAYER_INITIAL_X_VELOCITY;
            }
            else if(pState != RUNNING_RIGHT)
            {
                lastState = pState;
                pState = RUNNING_RIGHT;
                if(speedX < PLAYER_INITIAL_X_VELOCITY)
                    speedX = PLAYER_INITIAL_X_VELOCITY;
            }
        }
        else if((action & 0b0001) == 0b0001) // Is the left key pressed?
        {
            if(pState == FALLING || pState == JUMPING)
            {
                nextState = RUNNING_LEFT;
                speedX = PLAYER_INITIAL_X_VELOCITY;
            }
            else if(pState != RUNNING_LEFT)
            {
                lastState = pState;
                pState = RUNNING_LEFT;
                if(speedX < PLAYER_INITIAL_X_VELOCITY)
                    speedX = PLAYER_INITIAL_X_VELOCITY;
            }
        }
        else
        {
            if(nextState == RUNNING_RIGHT && (action & 0b0010) == 0b0010)
                nextState = IDLE;
            else if(nextState == RUNNING_LEFT && ((action & 0b0001) == 0b0001))
                nextState = IDLE;

            if(pState == RUNNING_RIGHT || pState == RUNNING_LEFT) pState = IDLE;
        }

        if((action & 0b0100) == 0b0100) // Is jump pressed?
        {
            if(canJump)
            {
                canJump = false;
                if(pState == RUNNING_LEFT || pState == RUNNING_RIGHT)
                {
                    nextState = pState;
                    pState = JUMPING;
                    if(shouldPlayerSprint)
                        speedY = PLAYER_MAX_Y_VELOCITY;
                    else
                        speedY = PLAYER_MID_Y_VELOCITY;
                    jumpStart = framePerSecondCount;
                }
                else if(pState == IDLE)
                {
                    pState = JUMPING;
                    speedY = PLAYER_MID_Y_VELOCITY;
                    jumpStart = framePerSecondCount;
                }
                else
                {
                    if(pState != FALLING && !jumping)
                    {
                        pState = JUMPING;
                        jumpStart = framePerSecondCount;
                    }
                    else
                    {
                        pState == FALLING;
                    }
                }
            }

        }
        else
        {
            // Ensures minimum jump height
            if(pState == JUMPING && (lastHeight > (MIN_JUMP_HEIGHT * UNIT_SCALE_FACTOR)))
            {
                qDebug() << "Min jump reached";
                pState = FALLING;
            }
            canJump = true;
        }

        if((action & 0b1111) == 0b0000) // Are no keys pressed?
        {
            if (pState == JUMPING && (lastHeight > (MIN_JUMP_HEIGHT * UNIT_SCALE_FACTOR)))
            {
                qDebug() << "Idle min jump reached";
                nextState = IDLE;
                lastState = pState;
                pState = IDLE;
            }
            else if(!playerOnObstacle && pState != JUMPING)
            {
                qDebug() << "Idle Fall";
                pState = FALLING;
                nextState = IDLE;
            }
            else if(pState == JUMPING && bonusHit)
            {
                qDebug() << "Bonus hit";
                pState = FALLING;
            }
            else if(pState == RUNNING_LEFT || pState == RUNNING_RIGHT || pState == FALLING)
            {
                nextState = IDLE;
                lastState = pState;
                pState = IDLE;
            }
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
        if(currentFrame < jumpStart) { currentFrame += 60; }
        float frameDelta = ((float)(currentFrame - jumpStart)) / 60.0f;

        float height = UNIT_SCALE_FACTOR * ((speedY * frameDelta) - (std::pow(frameDelta, 2.0f) * GRAVITY_FACTOR * 0.5f));
        heightDelta = height - lastHeight;
//        qDebug() << "Height: " << height << "\theightDelta: " << heightDelta << "\tTimeDelta: " << (currentFrame - jumpStart) << "\tFrame delta: " << frameDelta;

        if(speedX > PLAYER_IDLE_VELOCITY)
        {
            if(playerDirection == EAST && !leftWallCollided)
                setPosX(posX + PLAYER_FALLING_X_VELOCITY);
            else if(playerDirection == WEST && !rightWallCollided)
                setPosX(posX - PLAYER_FALLING_X_VELOCITY);

            if(nextState == RUNNING_LEFT) playerDirection = WEST;
            else if(nextState == RUNNING_RIGHT) playerDirection = EAST;
        }

        if((lastHeight != 0) && (heightDelta <= 0))
        {
            frame = 1;
            pState = FALLING;
            lastState = JUMPING;
            heightDelta = 0;
            lastHeight = 0;
        }
        else
        {
            setPosY(posY - heightDelta);
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
            posX += PLAYER_X_PX_PER_UPDATE * 1.5;
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

bool Player::hasReachedMinJump()
{
    return lastHeight > (MIN_JUMP_HEIGHT * UNIT_SCALE_FACTOR);
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
            if(shouldPlayerSprint)
                speedX = PLAYER_MAX_X_VELOCITY;
            else
                speedX = PLAYER_INITIAL_X_VELOCITY;
        }
        else if(nextState == RUNNING_RIGHT)
        {
            pState = RUNNING_RIGHT;
            if(shouldPlayerSprint)
                speedX = PLAYER_MAX_X_VELOCITY;
            else
                speedX = PLAYER_INITIAL_X_VELOCITY;
        }
        else if(pState != VICTORY)
        {
            pState = IDLE;
            speedX = PLAYER_IDLE_VELOCITY;
        }

        lastState = FALLING;
        nextState = IDLE;

        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
        setPosY(obsY - 95);
        heightDelta = 0;
        lastHeight = 0;
    }
    else
    {
        pState = FALLING;
//        jumping = true;
//        speedX = PLAYER_FALLING_X_VELOCITY;
    }
}

QPointF Player::GetMiddle()
{
    playerBB->rect().center();
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
        playerBB->setRect(posX + 30, posY + 45, 42, 35);
        playerBB->setPos(0, 0);
    }

    if(jumpViewBB != NULL)
    {
        jumpViewBB->setRect(posX + 25, posY, 52, 45);
        jumpViewBB->setPos(0,0);
    }

    if(fallViewBB != NULL)
    {
        fallViewBB->setRect(posX + 40, posY + 75, 26, 29);
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
        playerBB->setRect(posX + 30, posY + 45, 42, 35);
        playerBB->setPos(0, 0);
    }

    if(jumpViewBB != NULL)
    {
        jumpViewBB->setRect(posX + 25, posY, 52, 45);
        jumpViewBB->setPos(0,0);
    }

    if(fallViewBB != NULL)
    {
        fallViewBB->setRect(posX + 40, posY + 75, 26, 29);
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
