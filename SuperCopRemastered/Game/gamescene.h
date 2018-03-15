#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class GameScene : public QGraphicsScene
{
public:
    GameScene(QObject *parent = 0);

    void mousePressEvent(QGraphicsSceneMouseEvent *e);
};

#endif // GAMESCENE_H
