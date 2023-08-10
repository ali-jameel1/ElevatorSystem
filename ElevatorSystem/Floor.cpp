#include "Floor.h"

int Floor::nextFloor = 0;

Floor::Floor(QObject *parent) : QObject{parent}, floor(nextFloor++), safe(true)
{

}

int Floor::getFloor() const
{
    return floor;
}

int Floor::isSafe() const
{
    return safe;
}

void Floor::updateSafety()
{
    safe = !safe;
    cout << "Floor " << floor << " safety = " << safe << endl;
}

