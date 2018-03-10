#ifndef PLAYER_H
#define PLAYER_H

#include "datatypes.h"
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <QRect>

class Player
{
public:
    Player();
    Player(int parentWidth, int parentHeight);
    ~Player();

    void drawPlayer(QPainter &painter);
    void changeImage(QString str);
    void playerScreenPos();

    void jump();
    void roll();
    void run();
    void runInverted();
    void standBy();
    void pausePlayer();

    void setPosX(int x);
    void setPosY(int y);
    void setSize(Size s);
    void setOnGround(bool onGround);
    void setSpeedY(int y);
    void setJumping(bool jump);
    void setOnWall(bool onWall);
    void setOnPlatform(bool onPlat);
    void setWallCollided(bool wallCollided);
    void setSpeedX(int spd);
    void setRectPosX(int x);
    void setRectPosY(int y);

    int getPosX();
    int getPosY();
    int getFrame();
    int getLeftBound();
    int getRightBound();
    int getPlayerDirection();
    int getGround();
    int getSpeedY();
    int getRectPosX();
    int getRectPosY();
    int getRectSizeX();
    int getRectSizeY();

    Size getSize();

    bool isRolling();
    bool isJumping();
    bool isMoveRight();
    bool isMoveLeft();
    bool isOnGround();
    bool isAscending();
    bool isOnPlatform();
    bool isOnWall();
    bool isWallCollided();

public slots:
    void playerAction(int action);

private:

    bool rolling, jumping, moveLeft, moveRight, ascend, upPressed, pause, running;
    bool onGround, playerOnWall, playerOnPlatform, wallCollided;
    int rectPosX, rectPosY, rectSizeX, rectSizeY;
    int posX, posY;
    Size size;
    int frame;
    int leftBound, rightBound;
    int ground, speedX;
    int lastActionPressed;
    int playerDirection;
    QPixmap *image;
};

#endif // PLAYER_H
