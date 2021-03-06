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
    SuperCopRMGame(QWidget *parent = NULL, bool industrialGraphics = false);
    ~SuperCopRMGame();

    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *evt);
    void keyReleaseEvent(QKeyEvent *evt);
    void setLastKeyPress(int keyPress);
    void setHighScores();
    void setShowDevOpts(bool devOpts);

private:
    void InitLevel();

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

    LevelType currentLevelType;

    int picX,picY;
    int picHeight, picWidth;
    int lastKeyPress;
    int gamescore;
    int location;

    uint8_t keyPressState;

    bool isUpPressed, isDownPressed, isLeftPressed, isRightPressed, isSprintPressed, gamePaused, showDevOpts, gameOver;

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
