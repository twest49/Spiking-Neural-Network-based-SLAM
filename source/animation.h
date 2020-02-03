#ifndef ANIMATION_H
#define ANIMATION_H

#include<QPainter>
#include<QGraphicsItem>
#include<QGraphicsScene>

class Animation : public QGraphicsItem
{
public:
    Animation(QPointF *position,int radius,qreal angle);
    void setPosition(QPointF *position, qreal angle);

protected:
    void advance(int phase);
    QRectF boundingRect() const;
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);

private:
    int radius;
    qreal angle;

    void drawArrow(QPainter *painter);
};

#endif // ANIMATION_H
