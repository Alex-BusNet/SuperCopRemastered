#ifndef PLAYER_H
#define PLAYER_H

#include "datatypes.h"
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <QRect>

#define IDLE_FRAME_COUNT          10
#define RUN_FRAME_COUNT           8
#define JUMP_FRAME_COUNT          5
#define SLIDE_FRAME_COUNT         10
#define FALLING_FRAME_COUNT       5

#define PLAYER_INITIAL_X_VELOCITY 8.0f
#define PLAYER_INITIAL_Y_VELOCITY 5.0f
#define PLAYER_FALLING_X_VELOCITY 2.0f
#define COEFF_OF_FRICTION         0.5f
#define GRAVITY_FACTOR            0.75f

class Player
{
public:
    Player();
    Player(int parentWidth, int parentHeight);
    ~Player();

    void drawPlayer(QPainter &painter);
    void changeImage(QString str);
    void playerScreenPos();

    PlayerState getState();

    void UpdatePlayer();
    void UpdateFrame();

    void jump();
    void longJump();
    void slide();
    void run();
    void runInverted();
    void standBy();
    void pausePlayer();
    void fall();

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

    PlayerState lastState, pState, nextState;

    int rectPosX, rectPosY, rectSizeX, rectSizeY;
    int posX, posY;

    Size size;

    float speedX;
    float jumpSpeed;

    int frame;
    int leftBound, rightBound;
    int ground;
    int lastActionPressed;
    int playerDirection;
    int glideDistance;

    QString PlayerStateStrings[8] = {"IDLE", "RUNNING_RIGHT", "JUMPING", "SLIDING", "RUNNING_LEFT", "LONG_JUMPING", "FALLING", "PAUSED"};


    QString idleImagePath, jumpImagePath, runImagePath, slideImagePath, fallImagePath;
    QPixmap *image;
};

#endif // PLAYER_H
