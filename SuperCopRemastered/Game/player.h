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
#include <chrono>

#define IDLE_FRAME_COUNT          1
#define RUN_FRAME_COUNT           3
#define JUMP_FRAME_COUNT          4
#define SLIDE_FRAME_COUNT         8
#define FALLING_FRAME_COUNT       4
#define VICTORY_FRAME_COUNT       5

#define PLAYER_INITIAL_X_VELOCITY 1.0f
#define PLAYER_WALK_VELOCITY      7.0f
#define PLAYER_MAX_X_VELOCITY     15.0f

// Used for drifting
#define PLAYER_FALLING_X_VELOCITY 7.0f
#define PLAYER_X_PX_PER_UPDATE    9.0f
#define PLAYER_IDLE_VELOCITY      0.0f
// X px per update is only used when drifting
// Used with sliding. Currently disabled
#define COEFF_OF_FRICTION         0.5f
// Higher drag coeff = reach max speed faster
#define PLAYER_DRAG_COEFF         0.25f

// Vy = 18.182 units/s
#define PLAYER_INITIAL_Y_VELOCITY 18.182f //5.0f
#define PLAYER_Y_PX_PER_UPDATE    9.0f

// Pixels per unit where 1 unit is a single block
#define UNIT_SCALE_FACTOR         70.0f
// Gravity is in px/s^2 where 70px = 1 unit
#define GRAVITY_FACTOR            3080.0f

class Player
{
public:
    Player();
    Player(int parentWidth, int parentHeight);
    ~Player();

    void drawPlayer(QPainter &painter, bool devMode);
    void changeImage(QString str);

    void playerScreenPos();
    void Reset();

    PlayerState getState();
    PlayerState GetLastState();
    QPixmap* GetImage();
    QRect *GetBoundingBox();
    QRect* GetLeftBB();
    QRect* GetRightBB();
    QRect* GetJumpBB();
    QRect* GetFallBB();

    QGraphicsPixmapItem* GetViewPixmap();
    QGraphicsRectItem* GetViewBB();
    QGraphicsRectItem* GetFallViewBB();
    QGraphicsRectItem* GetJumpViewBB();

    void SetLevelBounds(int l, int r);

    void SetViewPixmap(QGraphicsPixmapItem* item);
    void SetViewBB(QGraphicsRectItem* item);
    void SetFallBB(QGraphicsRectItem* item);
    void SetJumpBB(QGraphicsRectItem* item);

    void UpdatePlayer(GameView *view);
    void UpdateFrame();

    void setPosX(int x);
    void setPosY(int y);
    void setSize(Size s);
    void setSpeedY(int y);
    void setJumping(bool jump);
    void setWallCollided(int side, bool wallCollided);
    void clearWallCollided();
    void setSpeedX(int spd);
    void setRectPosX(int x);
    void setRectPosY(int y);
    void SetVictory();
    void SetOnObstactle(bool onObs, int obsY);

    int GetPosX();
    int GetPosY();

    int GetPixmapX();
    int GetPixmapY();
    int GetBBX();
    int GetBBY();

    int getLeftBound();
    int getRightBound();
    int getPlayerDirection();
    int getGround();

    Size getSize();

    bool isJumping();
    bool isWallCollided();
    bool isOnObstacle();

public slots:
    void playerAction(int action, bool sprint, bool bonusHit = false);

private:
    void jump();
    void slide();
    void run();
    void runInverted();
    void standBy();
    void pausePlayer();
    void fall();
    void Celebrate();

    bool shouldPlayerSprint;

    bool jumping, pause, running;
    bool playerOnObstacle, leftWallCollided, rightWallCollided;

    PlayerState lastState, pState, nextState;
    QGraphicsPixmapItem *playerPixmap;
    QGraphicsRectItem *playerBB, *fallViewBB, *jumpViewBB;

    int framePerSecondCount;

    int rectPosX, rectPosY, rectSizeX, rectSizeY;
    int posX, posY;

    Size size;
    QRect *boundingBox, *fallBB, *jumpBB;

    float speedX, speedY;
    float jumpSpeed;
    float heightDelta, lastHeight;

    int frame;
    int leftBound, rightBound;
    int ground;
    int lastActionPressed;
    int playerDirection;
    int glideDistance;

//    std::chrono::steady_clock::time_point jumpStart;
    int jumpStart;

    QString PlayerStateStrings[8] = {"IDLE", "RUNNING_RIGHT", "JUMPING", "SLIDING", "RUNNING_LEFT", "FALLING", "PAUSED", "VICTORY"};

    QString idleImagePath, jumpImagePath, runImagePath, slideImagePath, fallImagePath, victoryImagePath;
    QPixmap *image;
};

#endif // PLAYER_H
