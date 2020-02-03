#include "spikingnetwork.cpp"
#include "simulation.cpp"
#include <iostream>

using namespace std;
/*
namespace {
void g(Simulation& robot){robot.step(1,0);}
void f(Simulation& robot){robot.step(0,0);
                          robot.step(0,0);}
void l(Simulation& robot){robot.step(0,1);
                          robot.step(0,1);}
void r(Simulation& robot){robot.step(0,-1);
                          robot.step(0,-1);}
}
int main()
{

    Simulation robot(15,15,8,1,1,1);
    robot.setRobot(1,1,0);

    int state = 1;
    std::vector<bool> bs;
    for(int i=0;i<1000;i++){
        bs=robot.getBumperSensor();
        switch (state) {
        case 1:
            if(robot.getBumperSensor()[0]==false) g(robot);
            else if(robot.getBumperSensor()[2]==false) state = 2;
            else {
                r(robot);
                state = 3;
            }
            break;
        case 2:
            l(robot);
            g(robot);
            l(robot);
            state = 3;
            break;
        case 3:
            if(robot.getBumperSensor()[0]==false) g(robot);
            else if(robot.getBumperSensor()[6]==false) state = 4;
            else {
                l(robot);
                state = 1;
            }
            break;
        case 4:
            r(robot);
            g(robot);
            r(robot);
            state = 1;
            break;
        default:
            break;
        }
    }

    robot.step(0,0);
    robot.step(0,0);

    robot.saveToFile();

    SpikingNetwork netw = SpikingNetwork("pynn.nest",8,15,15,1,1);
    netw.run();

    std::cout<<"Finished"<<std::endl;
}*/


int main(int argc, char *argv[])
{
    SpikingNetwork netw = SpikingNetwork(argv[1],8,15,15,1,1);
    netw.run();
}

