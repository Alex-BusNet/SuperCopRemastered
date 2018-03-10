#ifndef SUPERCOPRMGAME_H
#define SUPERCOPRMGAME_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QPixmap>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

#include <vector>
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QDebug>
#include <fstream>

#include "datatypes.h"
#include "levelbase.h"
#include "platform.h"
#include "player.h"
#include "wall.h"
#include "enemy.h"
#include "donut.h"

using namespace std;

class SuperCopRMGame : public QWidget
{
public:
    SuperCopRMGame(QWidget *parent = NULL);
    ~SuperCopRMGame();

    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *evt);
    void keyReleaseEvent(QKeyEvent *evt);
    void setLastKeyPress(int keyPress);
    void obstacleMovement();
    void setVecs();
    void setHighScores();
    void setMoveSpeed(int spd);
    void setShowDevOpts(bool devOpts);

private:
    Player *player;
    LevelBase *lb;

    QWidget* parent;
    QTimer *timer;
    QTimer *keyTimer;
    QMessageBox *msg;
    QMessageBox *pbox;
    QFont *scoreFont;
    QFont *pausedFont;
    int picX,picY;
    int picHeight, picWidth;
    int lastKeyPress;
    int gamescore;
    int location;

    bool isUpPressed, isDownPressed, isLeftPressed, isRightPressed, gamePaused, showDevOpts;

    vector<Wall*>walls;
    vector<Platform*>platforms;
    vector<Donut*>donuts;
    vector<Enemy*>enemies;
    vector<int>donutspawn;
    vector<int>enemyspawn;
    vector<int>wallSpawn;
    vector<int>platSpawn;

    Donut *levelEnd;
    int moveSpeed;

    QPushButton *resume;
    QPushButton *exit;
    QLabel *paused;
    QHBoxLayout *hLayout;
    QHBoxLayout *hLayout2;
    QVBoxLayout *vLayout;

public slots:
    void pollKey();
    void updateField();
    void resumeGame();
    void exitGame();
};

#endif // SUPERCOPRMGAME_H
