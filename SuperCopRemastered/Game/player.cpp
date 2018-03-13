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
    runImagePath = QString("Assets/Running/Test/%1/Run(%2).png");
    jumpImagePath = QString("Assets/Jumping/Test/%1/Jump(%2).png");
    slideImagePath = QString("Assets/Sliding/%1/Slide(%2).png");
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

    qDebug() << "Ground: " << ground;
    qDebug() << "Player height: " << size.y;
    qDebug() << "Player pos (Y): " << posY;
}

Player::~Player()
{
    delete image;
}//Destructor

Size Player::getSize()
{
    return size;
}

void Player::drawPlayer(QPainter &painter)
{
    painter.drawPixmap(posX, posY, size.x, size.y, *image);
    painter.drawRect(posX, posY, size.x, size.y);
    painter.drawRect(rectPosX, rectPosY, rectSizeX, rectSizeY);
    painter.drawText(10, 80, QString("Player state: %1").arg(PlayerStateStrings[pState]));
    painter.drawText(10, 90, QString("Jump Speed:   %1").arg(jumpSpeed));
    painter.drawText(10, 100, QString("Glide Distance: %1").arg(glideDistance));
    painter.drawText(10, 110, QString("X Speed:  %1").arg(speedX));
}//Draws the player


void Player::changeImage(QString str)
{
    delete image;
    image = new QPixmap(str);
}//Allows the player icon to be changed


void Player::playerScreenPos()
{
    //Check where player is on screen. If within a predefined rect, do not scroll screen.
    //If on edge of rect, move camera in direction player is running
    if(!jumping && 1 == lastActionPressed && (this->posX + 25 < rightBound) && !wallCollided)
    {
        this->setPosX(this->getPosX() + speedX);
        this->setRectPosX(this->getRectPosX() + speedX);
    }
    else if(!jumping && 4 == lastActionPressed && (this->posX > leftBound) && !wallCollided)
    {
        this->setPosX(this->getPosX() - speedX);
        this->setRectPosX(this->getRectPosX() - speedX);
    }
    else
    {
        this->setPosX(this->getPosX());
        this->setRectPosX(this->getRectPosX());
    }
}

PlayerState Player::getState()
{
    return pState;
}//Controls whether the screen moves or the player does

void Player::UpdatePlayer()
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
}

void Player::UpdateFrame()
{
    frame++;

    if(pState == JUMPING || pState == LONG_JUMPING)
    {
        if(frame >= JUMP_FRAME_COUNT) { frame = 1; }
    }
    else if(pState == RUNNING_LEFT || pState == RUNNING_RIGHT)
    {
        if(frame >= RUN_FRAME_COUNT) { frame = 1; }
    }
    else if(pState == IDLE)
    {
        if(frame >= IDLE_FRAME_COUNT) { frame = 1; }
    }
    else if (pState == SLIDING)
    {
        if(frame >= SLIDE_FRAME_COUNT) { frame = 1; pState = IDLE; lastState = SLIDING;}
    }
    else if (pState == FALLING)
    {
        if(frame >= FALLING_FRAME_COUNT) { frame = FALLING_FRAME_COUNT; pState = FALLING; }
    }
}


void Player::playerAction(int action)
{
    //Checks which direction is being called then runs the appropriate function
    switch (action)
    {
    case RIGHT:
        if((posY + size.y) < ground)
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
        if((posY + size.y) < ground)
            pState = FALLING;
        else
            pState = RUNNING_LEFT;
        break;
    case NONE:
        if((posY + size.y) < ground)
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
    if(frame == 1)
    {
        jumping = true;
        glideDistance = 0;
    }

    QString dir = (playerDirection == EAST) ? "Right" : "Left";

    if(pState == JUMPING)
    {
        if(playerDirection == WEST && lastActionPressed == LEFT) { posX -= 23; rectPosX -= 23;}
        else if(playerDirection == EAST && lastActionPressed == RIGHT) { posX += 23; rectPosX += 23; }
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
    if((posY + size.y) == ground)
    {
        speedX *= COEFF_OF_FRICTION;

        jumpSpeed = PLAYER_INITIAL_Y_VELOCITY;

        if(playerDirection == WEST)
        {
            posX -= (20 * speedX);
            rectPosX -= (20 * speedX);
            changeImage(slideImagePath.arg("Left").arg(frame));
        }
        else if(playerDirection == EAST)
        {
            posX += (20 * speedX);
            rectPosX += (20 * speedX);
            changeImage(slideImagePath.arg("Right").arg(frame));
        }

        if(speedX <= 0.5f)
        {
            pState = IDLE;
        }
    }
    else
    {
        posY += (30 * GRAVITY_FACTOR);
        rectPosY += (30 * GRAVITY_FACTOR);

        changeImage(fallImagePath.arg((playerDirection == WEST) ? "Left" : "Right").arg(frame));

        if((posY + size.y) >= ground)
        {
            posY = ground - size.y;
            rectPosY = ground - size.y + 10;
            pState = IDLE;
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
    }
    else if(!jumping)
    {
        glideDistance = 0;
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
    }
    else if(!jumping)
    {
        glideDistance = 0;

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
    glideDistance = 0;
    jumping = false;
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
    posY += (20 * GRAVITY_FACTOR);
    rectPosY += (20 * GRAVITY_FACTOR);

    changeImage(fallImagePath.arg((playerDirection == WEST) ? "Left" : "Right").arg(frame));

    if((posY + size.y) >= ground)
    {
        posY = ground - size.y;
        rectPosY = ground - size.y + 10;
        pState = IDLE;
    }

    if(jumping && abs(glideDistance) < 69)
    {
        if(playerDirection == WEST && lastActionPressed == LEFT)
        {
            posX -= 23;
            rectPosX -= 23;
            glideDistance -= 23;
        }
        else if(playerDirection == EAST && lastActionPressed == RIGHT)
        {
            posX += 23;
            rectPosX += 23;
            glideDistance += 23;
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
}//Accessor

void Player::setPosX(int x)
{
    posX=x;
}//Mutator


void Player::setPosY(int y)
{
    posY=y;
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

int Player::getPosX()
{
    return posX;
}//Accessor


int Player::getPosY()
{
    return posY;
}//Accessor
