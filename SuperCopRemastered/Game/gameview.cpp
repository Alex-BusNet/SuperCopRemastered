#include "gameview.h"

GameView::GameView(QWidget *parent)
{
    this->parent = parent;
    scene = new GameScene(this);
    this->setScene(scene);
    this->setMouseTracking(false);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setDragMode(QGraphicsView::NoDrag);
    this->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    this->setStyleSheet(QString("QGraphicsView { background-color: transparent; }"));
}

QGraphicsPixmapItem *GameView::addPixmap(const QPixmap &pixmap)
{
    return this->scene->addPixmap(pixmap);
}

QGraphicsRectItem *GameView::addRect(const QRect &rect, const QPen &pen, const QPixmap &texture)
{
    return this->scene->addRect(rect, pen, QBrush(texture));
}

void GameView::removePixmap(QGraphicsPixmapItem *pixmap)
{
    this->scene->removeItem(pixmap);
}

void GameView::removeRect(QGraphicsRectItem *rect)
{
    this->scene->removeItem(rect);
}

void GameView::keyPressEvent(QKeyEvent *e)
{
    // Passes the KeyPressEvent call to the parent
    e->ignore();
}

void GameView::keyReleaseEvent(QKeyEvent *e)
{
    // Passes the KeyReleaseEvent call to the parent
    e->ignore();
}
