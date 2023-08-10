#include "Passenger.h"

Passenger::Passenger(int weight, QObject *parent) : QObject{parent}, weight(weight)
{
    //this->weight = weight;
}

float Passenger::getWeight() const
{
    return weight;
}

int Passenger::getDestination() const
{
    return destination;
}
