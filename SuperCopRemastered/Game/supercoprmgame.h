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
#include <QGamepadKeyNavigation>
#include <QGamepadManager>

#include <vector>
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QDebug>
#include <fstream>

#include "datatypes.h"
#include "gameview.h"
#include "levelbase.h"
#include "player.h"
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
    void setHighScores();
    void setShowDevOpts(bool devOpts);

private:
    Player *player;
    QGraphicsPixmapItem *playerPixmap;
    QGraphicsRectItem *playerBB;
    LevelBase *lb;

    GameView *view;

    QWidget* parent;
    QTimer *timer, *renderTimer;
    QTimer *keyTimer;
    QMessageBox *msg;
    QMessageBox *pbox;
    QFont *scoreFont;
    QFont *pausedFont;

    QGamepadKeyNavigation *gpkn;
    QGamepadManager *gpm;

    int picX,picY;
    int picHeight, picWidth;
    int lastKeyPress;
    int gamescore;
    int location;

    bool isUpPressed, isDownPressed, isLeftPressed, isRightPressed, gamePaused, showDevOpts;

    vector<Donut*>donuts;
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
    void actionInput(Qt::Key key);
    void updateRender();
    void scoreUpdate(int value);
    void GameOver(bool endOfLevel);
};

#endif // SUPERCOPRMGAME_H
