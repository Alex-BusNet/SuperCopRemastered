#ifndef ROBOCOPRMGAME_H
#define ROBOCOPRMGAME_H

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

#include <QTcpSocket>
#include <QTcpServer>

using namespace std;


class robocoprmgame : public QWidget
{
    Q_OBJECT
public:
    explicit robocoprmgame(QWidget *parent = 0);
    ~robocoprmgame();
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(Qt::Key key);
    void keyReleaseEvent();
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

    int picX,picY;
    int picHeight, picWidth;
    int lastKeyPress;
    int gamescore;
    int location;

    bool isUpPressed, isDownPressed, isLeftPressed, isRightPressed, gamePaused, showDevOpts, gameOver;

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

    QTcpServer *server;
    QTcpSocket *socket;
    bool connected;

public slots:
    void pollKey();
    void updateField();
    void resumeGame();
    void exitGame();
    void actionInput(Qt::Key key);
    void updateRender();
    void scoreUpdate(int value);
    void GameOver(bool endOfLevel);

private slots:
    void newConnect();
    void Disconnected();
    void readyRead();
};

#endif // ROBOCOPRMGAME_H

