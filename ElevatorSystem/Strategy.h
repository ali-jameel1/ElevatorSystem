#ifndef STRATEGY_H
#define STRATEGY_H

#include <QList>

#include "defs.h"
#include "Floor.h"
#include "Elevator.h"

using namespace std;

class Strategy
{

    public:
        explicit Strategy(const string& name = "Strategy");
        string getName() const;
        void findSuitableElevator(Direction, Floor*, Elevator**, QList<Elevator*>) const;

    protected:
        string name;

    signals:

};

class AIStrategy : public Strategy
{
    public:
        AIStrategy();
        virtual void findSuitableElevator(Direction, Floor*, Elevator**, QList<Elevator*>) const;
};

class ElevatorCenteredStrategy : public Strategy
{
    public:
        ElevatorCenteredStrategy();
        virtual void findSuitableElevator(Direction, Floor*, Elevator**, QList<Elevator*>) const;
};

class DestinationKnownStrategy : public Strategy
{
    public:
        DestinationKnownStrategy();
        virtual void findSuitableElevator(Direction, Floor*, Elevator**, QList<Elevator*>) const;
};

#endif // STRATEGY_H
