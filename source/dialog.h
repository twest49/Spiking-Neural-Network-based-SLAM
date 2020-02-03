#ifndef DIALOG_H
#define DIALOG_H
#include<spikingnetwork.h>

#include<QDialog>
#include<QtCore>
#include<QtGui>
#include<QGraphicsScene>
#include<animation.h>
#include<simulation.h> 

namespace Ui
{
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit Dialog(const char *simulator,QWidget *parent = 0);
    ~Dialog();
private:
    Ui::Dialog *ui;
    QGraphicsScene *scene;
    QTimer *timer;
    double timeStep;    
    Simulation simulation;
    SpikingNetwork netw;
    Animation *ptr;
    qreal r;
    qreal num;
    int counter;
    int state;

private slots:
    void simulationStep();
public slots:
    void startSimulation();
    void endSimulation();
    void runNetwork();
};

#endif // DIALOG_H
