#ifndef PLAYER_H
#define PLAYER_H

#include "datatypes.h"
#include "gameview.h"
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <QRect>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>

#define IDLE_FRAME_COUNT          1
#define RUN_FRAME_COUNT           3
#define JUMP_FRAME_COUNT          4
#define SLIDE_FRAME_COUNT         8
#define FALLING_FRAME_COUNT       4

#define PLAYER_INITIAL_X_VELOCITY 20.0f
#define PLAYER_INITIAL_Y_VELOCITY 5.0f
#define PLAYER_FALLING_X_VELOCITY 2.0f
#define COEFF_OF_FRICTION         0.5f
#define GRAVITY_FACTOR            0.8f

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
    QPixmap* GetImage();
    QRect *GetBoundingBox();
    QRect* GetLeftBB();
    QRect* GetRightBB();
    QRect* GetJumpBB();
    QRect* GetFallBB();

    QGraphicsPixmapItem* GetViewPixmap();
    QGraphicsRectItem* GetViewBB();
    QGraphicsRectItem* GetFallViewBB();

    void SetLevelBounds(int l, int r);

    void SetViewPixmap(QGraphicsPixmapItem* item);
    void SetViewBB(QGraphicsRectItem* item);
    void SetFallBB(QGraphicsRectItem* item);
    void SetLeftBB(QGraphicsRectItem* item);
    void SetRightBB(QGraphicsRectItem* item);
    void SetJumpBB(QGraphicsRectItem* item);

    void UpdatePlayer(GameView *view);
    void UpdateFrame();

    void setPosX(int x);
    void setPosY(int y);
    void setSize(Size s);
    void setOnGround(bool onGround);
    void setSpeedY(int y);
    void setJumping(bool jump);
    void setOnWall(bool onWall);
    void setOnPlatform(bool onPlat);
    void setWallCollided(int side, bool wallCollided);
    void clearWallCollided();
    void setSpeedX(int spd);
    void setRectPosX(int x);
    void setRectPosY(int y);
    void setInGap(bool gap);

    void SetOnObstactle(bool onObs);

    int GetPosX();
    int GetPosY();

    int GetPixmapX();
    int GetPixmapY();
    int GetBBX();
    int GetBBY();

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
    bool isOnObstacle();

public slots:
    void playerAction(int action);

private:
    void jump();
    void slide();
    void run();
    void runInverted();
    void standBy();
    void pausePlayer();
    void fall();

    bool rolling, jumping, moveLeft, moveRight, ascend, upPressed, pause, running;
    bool onGround, playerOnWall, playerOnPlatform, wallCollided;
    bool playerOnObstacle, leftWallCollided, rightWallCollided, inGap;

    PlayerState lastState, pState, nextState;
    QGraphicsPixmapItem *playerPixmap;
    QGraphicsRectItem *playerBB, *fallViewBB, *leftViewBB, *rightViewBB, *jumpViewBB;

    int rectPosX, rectPosY, rectSizeX, rectSizeY;
    int posX, posY;

    Size size;
    QRect *boundingBox, *fallBB, *leftBB, *rightBB, *jumpBB;

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
