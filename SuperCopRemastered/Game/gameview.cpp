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

QGraphicsTextItem *GameView::addText(QString text)
{
    return this->scene->addText(text);
}

QGraphicsLineItem *GameView::addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen)
{
    return this->scene->addLine(x1, y1, x2, y2, pen);
}

void GameView::removePixmap(QGraphicsPixmapItem *pixmap)
{
    if(pixmap != NULL)
        this->scene->removeItem(pixmap);
}

void GameView::removeRect(QGraphicsRectItem *rect)
{
    if(rect != NULL)
        this->scene->removeItem(rect);
}

GameScene *GameView::GetScene()
{
    return scene;
}

void GameView::ClearScene()
{
    if(scene != NULL)
        scene->clear();
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
