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
#define VICTORY_FRAME_COUNT       5

#define PLAYER_INITIAL_X_VELOCITY 20.0f
#define PLAYER_INITIAL_Y_VELOCITY 5.0f
#define PLAYER_FALLING_X_VELOCITY 12.5f
#define PLAYER_DRAG_COEFF         0.4f
#define PLAYER_Y_PX_PER_UPDATE    9.0f
#define COEFF_OF_FRICTION         0.5f
#define GRAVITY_FACTOR            0.9f

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
    void setOnGround(bool onGround);
    void setSpeedY(int y);
    void setJumping(bool jump);
    void setWallCollided(int side, bool wallCollided);
    void clearWallCollided();
    void setSpeedX(int spd);
    void setRectPosX(int x);
    void setRectPosY(int y);
    void setInGap(bool gap);
    void SetVictory();
    void SetOnObstactle(bool onObs);

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
    bool isOnGround();
    bool isWallCollided();
    bool isOnObstacle();
    bool isInGap();

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
    void Celebrate();

    bool jumping, pause, running;
    bool onGround, playerOnObstacle, leftWallCollided, rightWallCollided, inGap;

    PlayerState lastState, pState, nextState;
    QGraphicsPixmapItem *playerPixmap;
    QGraphicsRectItem *playerBB, *fallViewBB, *jumpViewBB;

    int rectPosX, rectPosY, rectSizeX, rectSizeY;
    int posX, posY;

    Size size;
    QRect *boundingBox, *fallBB, *jumpBB;

    float speedX;
    float jumpSpeed;

    int frame;
    int leftBound, rightBound;
    int ground;
    int lastActionPressed;
    int playerDirection;
    int glideDistance;

    QString PlayerStateStrings[8] = {"IDLE", "RUNNING_RIGHT", "JUMPING", "SLIDING", "RUNNING_LEFT", "FALLING", "PAUSED", "VICTORY"};

    QString idleImagePath, jumpImagePath, runImagePath, slideImagePath, fallImagePath, victoryImagePath;
    QPixmap *image;
};

#endif // PLAYER_H
