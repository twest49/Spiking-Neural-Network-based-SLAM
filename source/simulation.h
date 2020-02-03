#ifndef SIMULATION_H
#define SIMULATION_H

#include <cypress/cypress.hpp>

#include <vector>

using json = cypress::Json;

class Map
{
private:
    std::vector<bool> map;
    int sizeX,sizeY;
public:
    Map(int sizeX, int sizeY);
    void addObject(int x, int y);
    void deleteObject(int x, int y);
    bool getCoordinate(int x, int y);
    void setMap(std::vector<bool> map);
};


class Simulation
{
private:
    int sizeX;
    int sizeY;
    int sizeDir;
    int x;  //1..sizeX
    int y;  //1..sizeY
    int direction;  //1..8
    int wheelEncoderAV;
    int wheelEncoderV;
    int time;
    int stepSize;
    std::vector<bool> bumperSensor;
    Map map;
    json data;

    int factorX();
    int factorY();
    std::vector<std::uint8_t> toBinaryJson();
    void toJson();


public:
    Simulation(int sizeX, int sizeY, int sizeDir, int x, int y, int direction);
    int getX();
    int getY();
    int getDirection();
    int getWheelEncoderAV();
    int getWheelEncoderV();
    void step(int velocity, int angularVelocity);
    void step();
    std::vector<bool> getBumperSensor();
    void saveToFile();
    void setMap(std::vector<bool> map);
    void addObject(int x, int y);
    void setRobot(int x, int y, int direction);

};

#endif // SIMULATION_H
