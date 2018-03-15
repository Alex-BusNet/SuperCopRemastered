#include "gamescene.h"
#include <QDebug>

GameScene::GameScene(QObject *parent) : QGraphicsScene(parent)
{

}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    qDebug() << "Screen Pos: " << e->screenPos();
    qDebug() << "Scene Pos: " << e->scenePos();
}
