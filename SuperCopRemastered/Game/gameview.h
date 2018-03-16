#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include "gamescene.h"

class GameView : public QGraphicsView
{
public:
    GameView(QWidget *parent = 0);

    QGraphicsPixmapItem *addPixmap(const QPixmap &pixmap);
    QGraphicsRectItem *addRect(const QRect &rect = QRect(), const QPen &pen = QPen(Qt::black), const QPixmap &texture = QPixmap());
    QGraphicsTextItem *addText(QString text);
    QGraphicsLineItem* addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen = QPen(Qt::black));
    void removePixmap(QGraphicsPixmapItem *pixmap);
    void removeRect(QGraphicsRectItem *rect);

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private:
    GameScene *scene;
    QWidget *parent;
};

#endif // GAMEVIEW_H
