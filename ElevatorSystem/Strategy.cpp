#include "Strategy.h"

Strategy::Strategy(const string& name): name(name)
{

}

string Strategy::getName() const
{
    return name;
}


// -----------------------------------------------------------------------------------------

AIStrategy::AIStrategy(): Strategy("AI")
{

}

void AIStrategy::findSuitableElevator(Direction, Floor *, Elevator **, QList<Elevator*>) const
{

}


// -----------------------------------------------------------------------------------------


ElevatorCenteredStrategy::ElevatorCenteredStrategy(): Strategy("Elevator-Centered")
{

}

void ElevatorCenteredStrategy::findSuitableElevator(Direction, Floor *, Elevator **, QList<Elevator*>) const
{

}


// -----------------------------------------------------------------------------------------


DestinationKnownStrategy::DestinationKnownStrategy(): Strategy("Destination-Known")
{

}

void DestinationKnownStrategy::findSuitableElevator(Direction, Floor *, Elevator **, QList<Elevator*>) const
{

}
