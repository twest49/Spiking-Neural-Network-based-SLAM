#include "simulation.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <ctime>

using json = nlohmann::json;

Map::Map(int sizeX, int sizeY)
{
    this->sizeX = sizeX;
    this->sizeY = sizeY;
    for(int row=0; row<=sizeY+1; row++){ //additional elements for the wall
        for(int col=0; col<=sizeX+1; col++){
            if ((row==0)||(row==sizeY+1)||(col==0)||(col==sizeX+1)) map.push_back(true); //sets wall
            else map.push_back(false);

        }
    }
}

void Map::addObject(int x, int y)
{
    map[y*(sizeX+2)+x] = true;
}

void Map::deleteObject(int x, int y)
{
    map[y*(sizeX+2)+x] = false;
}

bool Map::getCoordinate(int x, int y)
{
    return map[y*(sizeX+2)+x];
}

void Map::setMap(std::vector<bool> map)
{
    this->map = map;
}


Simulation::Simulation(int sizeX, int sizeY, int sizeDir, int x , int y, int direction):
    map(sizeX,sizeY)
{
    this->sizeX = sizeX;
    this->sizeY = sizeY;
    if(sizeDir%8==0) this->sizeDir = sizeDir;
    else this->sizeDir = 8;

    if (x < 1) this->x = 1;
    else if (x > sizeX) this->x = sizeX;
    else this->x = x;

    if (y < 1) this->y = 1;
    if (y > sizeY) this->y = sizeY;
    else this->y = y;

    if (direction < 0) this->direction = 0;
    if (direction > this->sizeDir-1) this->direction = this->sizeDir-1;
    else this->direction = direction;

    stepSize = (int) (this->sizeDir/8);

    wheelEncoderAV = 0;
    wheelEncoderV = 0;
    bumperSensor = {false,false,false,false,false,false,false,false};
    time = 0;
    data={};
    toJson();
    std::srand(std::time(nullptr));
}

void Simulation::setRobot(int x, int y, int direction)
{
    if (x < 1) this->x = 1;
    else if (x > sizeX) this->x = sizeX;
    else this->x = x;

    if (y < 1) this->y = 1;
    if (y > sizeY) this->y = sizeY;
    else this->y = y;

    if (direction < 0) this->direction = 0;
    if (direction > this->sizeDir-1) this->direction = sizeDir-1;
    else this->direction = direction;

    bumperSensor = {false,false,false,false,false,false,false,false};
    wheelEncoderAV = 0;
    wheelEncoderV = 0;
    time = 0;
    data={};
    toJson();
}

int Simulation::getX()
{
    return x;
}

int Simulation::getY()
{
    return y;
}

int Simulation::getDirection()
{
    return direction;
}

int Simulation::getWheelEncoderAV()
{
    return wheelEncoderAV;
}

int Simulation::getWheelEncoderV()
{
    return wheelEncoderV;
}

int Simulation::factorX()
//returns the x-component of movement based on heading direction
{
    float argument = 2*M_PI*((float)direction/(float)sizeDir);
    return (int)round((float)stepSize* cos(argument));

    /*if ((direction == 2)||(direction == 6)) return 0;
    else if ((direction < 2)||(direction == 7)) return 1;
    else if ((direction > 2)&&(direction < 6)) return -1;
    else return 0; //error*/
}

int Simulation::factorY()
//returns the y-component of movement based on heading direction
{
    float argument = 2*M_PI*((float)direction/(float)sizeDir);
    return (int)round((float)stepSize* sin(argument));

    /*if ((direction == 0)||(direction == 4)) return 0;
    else if (direction < 4) return 1;
    else if (direction > 4) return -1;
    else return 0; //error*/
}

std::vector<std::uint8_t> Simulation::toBinaryJson()
{
    //serialize to binary json
    std::vector<std::uint8_t> v_bson;// = json::to_bson(toJson());
    return v_bson;
}

void Simulation::toJson()
{
    getBumperSensor();
    bool bs1= bumperSensor[0];
    bool bs2= bumperSensor[1];
    bool bs3= bumperSensor[2];
    bool bs4= bumperSensor[3];
    bool bs5= bumperSensor[4];
    bool bs6= bumperSensor[5];
    bool bs7= bumperSensor[6];
    bool bs8= bumperSensor[7];
    std::cout<<"t:"<<time*10<<" ";
    std::cout<<"d:"<<direction<<" ";
    std::cout<<"x:"<<x-1<<" ";
    std::cout<<"y:"<<y-1<<" ";
    std::cout<<"P:"<<(x-1)+(y-1)*sizeX;
    std::cout<<std::endl;

    data [std::to_string(time*10)] = { //time stretched for network
        {"x",x-1},
        {"y",y-1},
        {"dir",direction},
        {"av",wheelEncoderAV},
        {"v",wheelEncoderV},
        {"bumper",{
             {"1",bs1},
             {"2",bs2},
             {"3",bs3},
             {"4",bs4},
             {"5",bs5},
             {"6",bs6},
             {"7",bs7},
             {"8",bs8}/*
             {"1",bumperSensor[0]},
             {"2",bumperSensor[1]},
             {"3",bumperSensor[2]},
             {"4",bumperSensor[3]},
             {"5",bumperSensor[4]},
             {"6",bumperSensor[5]},
             {"7",bumperSensor[6]},
             {"8",bumperSensor[7]}*/
         }}
    };
}

void Simulation::saveToFile() //currently saves json directly
{
    std::ofstream file;

    file.open("../result/simulation", std::ofstream::out);
    if (!file.good()) {
        file.open("../../result/simulation", std::ofstream::out);
        if (!file.good()) throw std::runtime_error("Could not open file");
    }
    /*std::vector<std::uint8_t> binary = toBinaryJson();
    for(int i=0;i<binary.size();i++) file.write((char*)&binary[i],sizeof(std::uint8_t));
    file << toJson() << std::endl; */
    data["simulationTime"] = time*10;  //time stretched for network
    file << data;
    file.close();
    std::cout<<"successfully written to file"<<std::endl;
}

void Simulation::setMap(std::vector<bool> map)
{
    this->map.setMap(map);
}

void Simulation::addObject(int x, int y){
    map.addObject(x,y);
}

std::vector<bool> Simulation::getBumperSensor()
{
    bumperSensor.clear();
    //bumperSensor when direction=1

    bumperSensor.push_back(map.getCoordinate(x+1,y));
    bumperSensor.push_back(false);
    bumperSensor.push_back(map.getCoordinate(x,y+1));
    bumperSensor.push_back(false);
    bumperSensor.push_back(map.getCoordinate(x-1,y));
    bumperSensor.push_back(false);
    bumperSensor.push_back(map.getCoordinate(x,y-1));
    bumperSensor.push_back(false);
    int sum=0;
    for(int i=0;i<bumperSensor.size();i++) if(bumperSensor[i]==true) sum++;
    if(sum==0){
        bumperSensor.clear();
        bumperSensor.push_back(false);
        bumperSensor.push_back(map.getCoordinate(x+1,y+1));
        bumperSensor.push_back(false);
        bumperSensor.push_back(map.getCoordinate(x-1,y+1));
        bumperSensor.push_back(false);
        bumperSensor.push_back(map.getCoordinate(x-1,y-1));
        bumperSensor.push_back(false);
        bumperSensor.push_back(map.getCoordinate(x+1,y-1));
    }
    //turn in current direction
    for(int i=0;i<direction;i++){
        bumperSensor.push_back(bumperSensor[0]);
        bumperSensor.erase(bumperSensor.begin());
    }
    return bumperSensor;
}

void Simulation::step(int velocity, int angularVelocity) //case of hitting obstacles except the wall is currently not implemented
{
    if ((angularVelocity < sizeDir-1)&&(angularVelocity > -(sizeDir-1))){
        time += 1;

        //x and y components of movement
        int dx = 0;
        int dy = 0;
        if(velocity>=0){
            dx = factorX();
            dy = factorY();
        }
        else{
            dx = -factorX();
            dy = -factorY();
        }

        for(wheelEncoderV = 0; wheelEncoderV<abs(velocity);wheelEncoderV++){
            x += dx;
            y += dy;
            if (map.getCoordinate(x,y)==true){
                x -=dx;
                y -=dy;
                break;
            }
        }
        if(velocity<0) wheelEncoderV = -wheelEncoderV;

        //direction
        direction += angularVelocity;
        //clipps direction to range 0..sizeDir-1
        if (direction > sizeDir-1) direction -= sizeDir;
        else if (direction < 0) direction += sizeDir;
        wheelEncoderAV = angularVelocity;
    }
    toJson();
}

void Simulation::step()
{
    //int velocity = std::rand()/((RAND_MAX + 1u)/3) - 1;
    int velocity = 1;
    int angularVelocity = std::rand()/((RAND_MAX + 1u)/3) - 1;
    time += 1;

    //x and y components of movement
    int dx = 0;
    int dy = 0;
    if(velocity>=0){
        dx = factorX();
        dy = factorY();
    }
    else{
        dx = -factorX();
        dy = -factorY();
    }

    for(wheelEncoderV = 0; wheelEncoderV<abs(velocity);wheelEncoderV++){
        x += dx;
        y += dy;
        if (map.getCoordinate(x,y)==true){
            x -=dx;
            y -=dy;
            break;
        }
    }
    if(velocity<0) wheelEncoderV = -wheelEncoderV;

    //direction
    direction += angularVelocity;
    //clipps direction to range 0..sizeDir-1
    if (direction > sizeDir-1) direction -= sizeDir;
    else if(direction < 0) direction += sizeDir;
    wheelEncoderAV = angularVelocity;
    toJson();
}

