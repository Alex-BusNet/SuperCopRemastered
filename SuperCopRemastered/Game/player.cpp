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
    posY = parentHeight - 100;

    idleImagePath = QString("Assets/Idle/%1/Idle(%2).png");
    runImagePath = QString("Assets/Running/%1/Run(%2).png");
    jumpImagePath = QString("Assets/Jumping/%1/Jump(%2).png");
    slideImagePath = QString("Assets/Rolling/%1/Roll(%2).png");
    fallImagePath = QString("Assets/Falling/%1/Fall(%2).png");

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

//    boundingBox = new QRect(rectPosX, rectPosY, rectSizeX, rectSizeY);
    boundingBox = new QRect(posX, posY, size.x, size.y);

    leftBound = parentWidth / 6;
    rightBound = parentWidth - (parentWidth / 5);
    ground = parentHeight;

    rolling = false;
    jumping = false;
    moveRight = false;
    moveLeft = false;
    ascend = false;
    playerOnWall = false;
    playerOnPlatform = false;
    onGround = true;
    upPressed = false;
    wallCollided = false;
    playerOnObstacle = false;

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
}//Destructor

Size Player::getSize()
{
    return size;
}

void Player::drawPlayer(QPainter &painter)
{
//    painter.drawPixmap(posX, posY, size.x, size.y, *image);
//    painter.drawRect(posX, posY, size.x, size.y);
//    painter.drawRect(rectPosX, rectPosY, rectSizeX, rectSizeY);
    painter.drawText(15, 80, QString("Player state: %1").arg(PlayerStateStrings[pState]));
    painter.drawText(15, 90, QString("Jump Speed:   %1").arg(jumpSpeed));
    painter.drawText(15, 100, QString("Glide Distance: %1").arg(glideDistance));
    painter.drawText(15, 110, QString("X Speed:  %1").arg(speedX));
}//Draws the player


void Player::changeImage(QString str)
{
    delete image;
    image = new QPixmap(str);
    playerPixmap->setPixmap(*image);
}//Allows the player icon to be changed


void Player::playerScreenPos()
{
    //Check where player is on screen. If within a predefined rect, do not scroll screen.
    //If on edge of rect, move camera in direction player is running
    if(!jumping && RIGHT == lastActionPressed && (this->posX + speedX < rightBound) && !wallCollided)
    {
        this->setPosX(this->GetPosX() + speedX);

    }
    else if(!jumping && LEFT == lastActionPressed && (this->posX > leftBound) && !wallCollided)
    {
        this->setPosX(this->GetPosX() - speedX);
    }
    else
    {
        this->setPosX(this->GetPosX());
    }
}

PlayerState Player::getState()
{
    return pState;
}

QPixmap *Player::GetImage()
{
    return image;
}

QRect *Player::GetBoundingBox()
{
    return boundingBox;
}

QGraphicsPixmapItem *Player::GetViewPixmap()
{
    return playerPixmap;
}

QGraphicsRectItem *Player::GetViewBB()
{
    return playerBB;
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
}//Controls whether the screen moves or the player does

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
    case LONG_JUMPING:
        longJump();
        break;
    case FALLING:
        fall();
        break;
    case SLIDING:
        slide();
        break;
    case PAUSED:
        break;
    default:
        standBy();
        break;
    }

    if((posX - 200 > leftBound) && (posX + 200 < rightBound))
        view->ensureVisible(playerPixmap, 200, 10);
}

void Player::UpdateFrame()
{
    frame++;

    if(pState == JUMPING || pState == LONG_JUMPING)
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
}


void Player::playerAction(int action)
{
    //Checks which direction is being called then runs the appropriate function
    switch (action)
    {
    case RIGHT:
        if((posY + size.y) < ground && !playerOnObstacle)
            pState = FALLING;
        else
            pState = RUNNING_RIGHT;
        break;
    case UP:
        if(pState == JUMPING || lastState == JUMPING)
            break;
        if(pState == RUNNING_RIGHT || pState == RUNNING_LEFT)
            pState = LONG_JUMPING;
        else
            pState = JUMPING;
        break;
    case DOWN:
        if(lastState != SLIDING)
            pState = SLIDING;
        else if((posY + size.y) < ground)
            pState = FALLING;
        else
            pState = IDLE;
        break;
    case LEFT:
        if((posY + size.y) < ground && !playerOnObstacle)
            pState = FALLING;
        else
            pState = RUNNING_LEFT;
        break;
    case NONE:
        if((posY + size.y) < ground && !playerOnObstacle)
            pState = FALLING;
        else
            pState = IDLE;
        break;
    case PAUSE:
        pState = PAUSED;
        pausePlayer();
    default:
        pState = IDLE;
        break;
    }

    //If the new direction does not match the previous direction, reset the frame counter to zero.
    if(action != lastActionPressed)
    {
        if(!upPressed)
        {
            frame = 1;
        }

        lastActionPressed = action;
    }

    this->playerScreenPos();
}//Calls the various player controlled movement functions


void Player::jump()
{
    jumping = true;
    playerOnObstacle = false;
    onGround = false;

    QString dir = (playerDirection == EAST) ? "Right" : "Left";

    if(pState == JUMPING)
    {
        if(playerDirection == WEST && lastActionPressed == LEFT) { posX -= 20; rectPosX -= 20;}
        else if(playerDirection == EAST && lastActionPressed == RIGHT) { posX += 20; rectPosX += 20; }
    }

    if(pState != FALLING)
    {
        jumpSpeed *= GRAVITY_FACTOR;

        posY -= (30 * jumpSpeed);
        rectPosY -= (30 * jumpSpeed);

        if(jumpSpeed <= 0.5f)
        {
            frame = 1;
            pState = FALLING;
            lastState = JUMPING;
            jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
        }
    }

    changeImage(jumpImagePath.arg(dir).arg(frame));
}//Controls Player Jumps

void Player::longJump()
{

}

void Player::slide()
{
    if((posY + size.y) == ground || playerOnObstacle)
    {
        speedX *= COEFF_OF_FRICTION;

        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;

        if(playerDirection == WEST)
        {
            posX -= (10 * speedX);
            rectPosX -= (10 * speedX);
            changeImage(slideImagePath.arg("Left").arg(frame));
        }
        else if(playerDirection == EAST)
        {
            posX += (10 * speedX);
            rectPosX += (10 * speedX);
            changeImage(slideImagePath.arg("Right").arg(frame));
        }

        if(speedX <= 0.5f)
        {
            pState = IDLE;
        }
    }
    else
    {
        posY += (40 * GRAVITY_FACTOR);
        rectPosY += (40 * GRAVITY_FACTOR);

        changeImage(fallImagePath.arg((playerDirection == WEST) ? "Left" : "Right").arg(frame));

        if((posY + size.y) >= ground)
        {
            posY = ground - size.y;
            rectPosY = ground - size.y + 5;
            pState = IDLE;
            speedX = PLAYER_INITIAL_X_VELOCITY;
            jumping = false;
        }
    }
}


void Player::run()
{
    lastState = IDLE;
    running = true;

    if(jumping && pState == FALLING)
    {
        speedX = PLAYER_FALLING_X_VELOCITY;
        playerDirection = EAST;
    }
    else if(!jumping)
    {
        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;

        if(speedX < 0.5f)
            speedX = PLAYER_INITIAL_X_VELOCITY;

        changeImage(runImagePath.arg("Right").arg(frame));
        moveRight = true;
        playerDirection = 1;
    }

}//Controls Player Running right


void Player::runInverted()
{
    lastState = IDLE;
    running = true;

    if(jumping && pState == FALLING)
    {
        speedX = PLAYER_FALLING_X_VELOCITY;
        playerDirection = WEST;
    }
    else if(!jumping)
    {
        if(speedX < 0.5f)
            speedX = PLAYER_INITIAL_X_VELOCITY;

        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;

        changeImage(runImagePath.arg("Left").arg(frame));
        moveLeft = true;
        playerDirection = -1;
    }
}//Controls Player Running Left


void Player::standBy()
{
    running = false;
    jumping = false;
    speedX = PLAYER_INITIAL_X_VELOCITY;
    //Checks which direction the player was moving last then sets the appropiate standing image
    if(1 == playerDirection)
    {
        changeImage(idleImagePath.arg("Right").arg(frame++));
    }
    else if(-1 == playerDirection)
    {
        changeImage(idleImagePath.arg("Left").arg(frame++));
    }
}

void Player::pausePlayer()
{

}//Controls Player Stopped

void Player::fall()
{
    if(onGround) { return; }

    posY += (30 * GRAVITY_FACTOR);
    rectPosY += (30 * GRAVITY_FACTOR);

    changeImage(fallImagePath.arg((playerDirection == WEST) ? "Left" : "Right").arg(frame));

    if((posY + size.y) >= ground)
    {
        posY = ground - size.y;
        rectPosY = ground - size.y;
        pState = IDLE;
        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
        jumping = false;
        onGround = true;
    }

    if(jumping)
    {
        if(/*playerDirection == WEST && */lastActionPressed == LEFT)
        {
            posX -= 20;
            rectPosX -= 20;
            playerDirection = WEST;
        }
        else if(/*playerDirection == EAST && */lastActionPressed == RIGHT)
        {
            posX += 20;
            rectPosX += 20;
            playerDirection = EAST;
        }
    }
}


int Player::getFrame()
{
    return frame;
}//Accessor


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

int Player::getRectPosX()
{
    return rectPosX;
}

int Player::getRectPosY()
{
    return rectPosY;
}

int Player::getRectSizeX()
{
    return rectSizeX;
}

int Player::getRectSizeY()
{
    return rectSizeY;
}//Accessor


bool Player::isRolling()
{
    return rolling;
}//Accessor


bool Player::isJumping()
{
    return jumping;
}//Accessor


bool Player::isMoveRight()
{
    return moveRight;
}//Accessor


bool Player::isMoveLeft()
{
    return moveLeft;
}//Accessor

bool Player::isOnGround()
{
    return onGround;
}//Accessor

bool Player::isAscending()
{
    return ascend;
}//Accessor

bool Player::isOnPlatform()
{
    return playerOnPlatform;
}//Accessor

bool Player::isOnWall()
{
    return playerOnWall;
}//Accessor

bool Player::isWallCollided()
{
    return wallCollided;
}

bool Player::isOnObstacle()
{
    return playerOnObstacle;
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

void Player::SetOnObstactle(bool onObs)
{
    playerOnObstacle = onObs;
    frame = 1;

    if(onObs)
    {
        speedX = PLAYER_INITIAL_X_VELOCITY;
        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;
    }

    if(lastActionPressed == LEFT) { pState = RUNNING_LEFT; }
    else if(lastActionPressed == RIGHT) { pState = RUNNING_RIGHT; }
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
        playerBB->setRect(posX, posY, size.x, size.y);
        playerBB->setPos(0, 0);
    }

}//Mutator


void Player::setPosY(int y)
{
    posY=y;

    if(playerPixmap != NULL)
        playerPixmap->setPos(posX, posY);

    if(playerBB != NULL)
    {
        playerBB->setRect(posX, posY, size.x, size.y);
        playerBB->setPos(0, 0);
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

void Player::setOnWall(bool onWall)
{
    this->playerOnWall = onWall;
}//Mutator


void Player::setOnPlatform(bool onPlat)
{
    this->playerOnPlatform = onPlat;
}

void Player::setWallCollided(bool wallCollided)
{
    this->wallCollided = wallCollided;
}//Mutator

int Player::GetPosX()
{
    return posX;
}//Accessor


int Player::GetPosY()
{
    return posY;
}//Accessor
