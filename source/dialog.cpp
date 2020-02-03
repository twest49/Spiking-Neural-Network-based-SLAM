#include "dialog.h"
#include "ui_dialog.h"
#include <iostream>

namespace {
    static int x_ = 1;
    static int y_ = 1;
    static int dir_ = 0;
    static int num_ = 15;
    static int numDir_ = 8;
}


Dialog::Dialog(const char *simulator,QWidget *parent):
    QDialog(parent),
    ui(new Ui::Dialog),
    simulation(num_,num_,numDir_,x_,y_,dir_),
    netw(simulator,numDir_,num_,num_,1,1)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    //resolution
    num = num_; //number of fields in a row/column
    r = 100/num; //radius of the robot in robot coordinate (length of a field is 2*r)
    qreal len = 2*r*2*num; //length and hight of the map in world coordinate

    //add graphic scene
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    scene->setSceneRect(0,0,len,len);
    QPen myPen = QPen(Qt::black);
    QLineF topLine(scene->sceneRect().topLeft(),scene->sceneRect().topRight());
    QLineF leftLine(scene->sceneRect().topLeft(),scene->sceneRect().bottomLeft());
    QLineF rightLine(scene->sceneRect().topRight(),scene->sceneRect().bottomRight());
    QLineF bottomLine(scene->sceneRect().bottomLeft(),scene->sceneRect().bottomRight());
    scene->addLine(topLine,myPen);
    scene->addLine(leftLine,myPen);
    scene->addLine(rightLine,myPen);
    scene->addLine(bottomLine,myPen);

    //add animation
    int direction = dir_;
    int x = x_;
    int y = x_;
    qreal angle = (direction)*2*M_PI/numDir_ ;
    QPointF *startPos = new QPointF((2*(x-1)+1)*r,(2*(num-y)+1)*r);
    Animation *animation = new Animation(startPos,r,angle);
    ptr = animation;
    scene->addItem(animation);

    //add object
    qreal xPos = 5;
    qreal yPos = 5;
    QGraphicsRectItem *object = new QGraphicsRectItem((xPos-1)*4*r,len-yPos*4*r,4*r,4*r);
    scene->addItem(object);
    simulation.addObject(xPos,yPos);
    /*//add object
    xPos = 7;
    yPos = 9;
    QGraphicsRectItem *object2 = new QGraphicsRectItem((xPos-1)*4*r,len-yPos*4*r,4*r,4*r);
    scene->addItem(object2);
    simulation.addObject(xPos,yPos);*/

    timer = new QTimer(this);
    timeStep = 100.0;
    counter = 0;
    state = 0;
    std::srand(std::time(nullptr));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::startSimulation()
{
    simulation.setRobot(x_,y_,dir_);
    connect(timer,SIGNAL(timeout()),this,SLOT(simulationStep()));
    timer->start(timeStep);
}

void Dialog::endSimulation()
{
    timer->stop();
    disconnect(timer,SIGNAL(timeout()),this,SLOT(simulationStep()));
    simulation.saveToFile();
}

void Dialog::runNetwork()
{
    netw.run();
}

void Dialog::simulationStep()
{
    //controller
    int randomizer;
    std::vector<bool> sensor = simulation.getBumperSensor();


    if(sensor[0]==true && sensor[2]==true) simulation.step(0,-1); //left corner  turn right
    else if(sensor[0]==true && sensor[6]==true) simulation.step(0,1);  //right corner  turn left
    else if(sensor[0]==true) //wall in front   turn left/right
    {
        randomizer = std::rand()/((RAND_MAX + 1u)/2);
        switch (randomizer) {
        case 0:
            simulation.step(0,1);
            break;
        case 1:
            simulation.step(0,-1);
            break;
        default: break;
        }
    }
    else if(sensor[1]==true && sensor[7]==true) //in corner  turn left or right
    {
        randomizer = std::rand()/((RAND_MAX + 1u)/2);
        switch (randomizer) {
        case 0:
            simulation.step(0,1);
            break;
        case 1:
            simulation.step(0,-1);
            break;
        default: break;
        }
    }
    else if(sensor[7]==true) simulation.step(0,1); //right diagonal wall  turn left
    else if(sensor[1]==true) simulation.step(0,-1); //left diagonal wall  turn right
    else if(sensor[2]==true) //left wall  step forward or turn right
    {
        randomizer = std::rand()/((RAND_MAX + 1u)/2);
        switch (randomizer) {
        case 0:
            simulation.step(1,0);
            break;
        case 1:
            simulation.step(0,-1);
            break;
        default: break;
        }
    }
    else if(sensor[6]==true) //right wall  step forward or turn left
    {
        randomizer = std::rand()/((RAND_MAX + 1u)/2);
        switch (randomizer) {
        case 0:
            simulation.step(0,1);
            break;
        case 1:
            simulation.step(1,0);
            break;
        default: break;
        }
    }
    else if(sensor[3]==true && sensor[5]==true) //corner behind   step forward or turn left or right
    {
        randomizer = std::rand()/((RAND_MAX + 1u)/3);
        switch (randomizer) {
        case 0:
            simulation.step(1,0);
            break;
        case 1:
            simulation.step(0,-1);
            break;
        case 2:
            simulation.step(0,1);
            break;
        default: break;
        }
    }
    else if(sensor[3]==true) //left diagonal wall behind  step forward or turn right
    {
        randomizer = std::rand()/((RAND_MAX + 1u)/2);
        switch (randomizer) {
        case 0:
            simulation.step(1,0);
            break;
        case 1:
            simulation.step(0,-1);
            break;
        default: break;
        }
    }
    else if(sensor[5]==true) //right diagonal wall behind  step forward or turn left
    {
        randomizer = std::rand()/((RAND_MAX + 1u)/2);
        switch (randomizer) {
        case 0:
            simulation.step(0,1);
            break;
        case 1:
            simulation.step(1,0);
            break;
        default: break;
        }
    }
    else simulation.step(1,0);




    //simulation.step();
    //visualization
    QPointF *newPos = new QPointF((2*(simulation.getX()-1)+1)*r,(2*(num-simulation.getY())+1)*r);
    qreal newAngle= (simulation.getDirection())*2*M_PI/numDir_;
    ptr->setPosition(newPos,newAngle);
    scene->advance();
    /*counter ++;   //for changing maps
    if(counter>50){
        simulation.setMap(...);
        QPointF *startPos = new QPointF(0,0);
        Object * item = new Object(startPos,20,10);
        scene->addItem(item);
    }*/
}
