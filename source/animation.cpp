#include "animation.h"
#include<iostream>
#include <math.h>

Animation::Animation(QPointF *position,int radius,qreal angle)
{
    this->radius = radius;
    this->angle = angle;
    setPos(*position);
}

QRectF Animation::boundingRect() const
{
    return QRectF(QPointF(this->x()-2*this->radius,this->y()-2*this->radius),QSizeF(4*this->radius,4*this->radius));
}

void Animation::drawArrow(QPainter *painter)
{
    painter->setPen(QPen(Qt::black,1));
    int dx = (int)(this->radius * cos(this->angle) / 2);
    int dy = (int)(this->radius * sin(this->angle) / 2);
    int dx2 = (int)(this->radius * cos(this->angle + M_PI * 3/4) / 3);
    int dy2 = (int)(this->radius * sin(this->angle + M_PI * 3/4) / 3);
    int dx3 = (int)(this->radius * cos(this->angle + M_PI * 5/4) / 3);
    int dy3 = (int)(this->radius * sin(this->angle + M_PI * 5/4) / 3);
    painter->drawLine(this->x()+dx,this->y()-dy,this->x()-dx,this->y()+dy);
    painter->drawLine(this->x()+dx,this->y()-dy,this->x()-dx2,this->y()+dy2);
    painter->drawLine(this->x()+dx,this->y()-dy,this->x()-dx3,this->y()+dy3);
}

void Animation::paint(QPainter *painter,const QStyleOptionGraphicsItem *,QWidget *)
{
    painter->setPen(QPen(Qt::black,1));
    painter->drawEllipse(this->pos(),2*this->radius,2*this->radius);
    this->drawArrow(painter);
    //Draw bounding rect
    QRectF rec = boundingRect();
    QBrush brush(Qt::gray);
    painter->drawRect(rec);
}

void Animation::setPosition(QPointF *position, qreal angle)
{
    this->angle = angle;
    setPos(*position);
}

void Animation::advance(int phase)
{
    if (phase==1){ //Painting is updated even if bounding rect stays the same
        update(QRectF(QPointF(this->x()-2*this->radius,this->y()-2*this->radius),QSizeF(4*this->radius,4*this->radius)));
    }
}

