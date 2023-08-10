#include "ECS.h"
#include "Elevator.h"

ECS::ECS(int numFlrs, int numElevs, QObject *parent) : QObject{parent}, strategy(AI)
{
    for (int i = 0; i < numFlrs; ++i)
    {
        Floor* flr = new Floor();
        addFloor(flr);
    }

    Elevator::setECS(this);

    /*
     * gives a warning for potential memory leak...
     * this is because in this source file elev isn't
     * saved to anything so it can be considered lost,
     * but if you go to the elevator constructor it actually
     * saves the elevator to the class' static ecs' array of elevators
     *
     * but in line 12 this ecs is set to the Elevator class' static ecs member,
     * meaning that it is actually added to the QList and not lost
     *
     * this design was just to prevent the possibility of elevators having a
     * different ecs per instance, so I just removed this possibility by making
     * a static ECS member to the Elevator class
     */
    for (int i = 0; i < numElevs; ++i)
    {
        Elevator* elev = new Elevator(floors[1]); // set the elevators current floor to floor 1 initially
    }
}

ECS::~ECS()
{
    while (true) // deallocate floors
    {
        Floor* flr = floors[floors.size()-1];
        floors.pop_back();
        delete flr;

        if (floors.size() == 0)
        {
            break;
        }
    }

    while (true) // deallocate elevators
    {
        Elevator* elev = elevators[elevators.size()-1];
        elevators.pop_back();
        delete elev;

        if (elevators.size() == 0)
        {
            break;
        }
    }
}




Floor *ECS::getFloor(int i) const
{
    return floors[i];
}

int ECS::getFloorQuantity() const
{
    return floors.size();
}

Elevator *ECS::getElevator(int i) const
{
    return elevators[i];
}





void ECS::addFloor(Floor* flr)
{
    floors.push_back(flr);
}

void ECS::addElevator(Elevator* elev)
{
    elevators.push_back(elev);
}




void ECS::run() const
{
    for (int i = 0; i < elevators.size(); ++i)
    {
        elevators[i]->start();
    }
}




void ECS::handleRequest(Direction dir, Floor* flr) const
{
    cout << "handling request..." << endl;
    Elevator* elev = NULL;
    findSuitableElevator(dir, flr, &elev);
    sendElevator(elev, flr);
}

void ECS::findSuitableElevator(Direction dir, Floor* flr, Elevator** elev) const
{
    cout << "finding suitable elevator..." << endl;

    switch (strategy)
    {
        case AI:
            AIStrategy(dir, flr, elev);
            break;
        case ELEVATOR_CENTERED:
            elevatorCenteredStrategy(flr, elev);
            break;
        case DESTINATION_KNOWN:
            destinationknownStrategy(dir, flr, elev);
            break;
        default:
            break;
    }

}

void ECS::swapStrategy(int strat)
{
    switch (strat)
    {
        case 0:
            cout << "strategy has been swapped to the AI strategy..." << endl;
            strategy = AI;
            break;
        case 1:
            cout << "strategy has been swapped to the Elevator-Centered strategy..." << endl;
            strategy = ELEVATOR_CENTERED;
            break;
        case 2:
            cout << "strategy has been swapped to the Destination-Known strategy..." << endl;
            strategy = DESTINATION_KNOWN;
            break;
        default:
            break;
    }
}

void ECS::AIStrategy(Direction dir, Floor* flr, Elevator** elev) const
{
    QList<Elevator*> candidates;

    // get closest elevator(s)
    candidates.push_back(elevators[0]);
    for (int i = 1; i < elevators.size(); ++i)
    {
        if (flr->getFloor() - elevators[i]->getCurrentFloor()->getFloor() == 0) // elevator already on floor so send this one
        {
            *elev = elevators[i];
            return;
        }

        if (abs(flr->getFloor() - elevators[i]->getCurrentFloor()->getFloor()) < abs(flr->getFloor() - candidates[0]->getCurrentFloor()->getFloor())) // reset list of candidates to the new closest one so far
        {
            candidates.clear();
            candidates.push_back(elevators[i]);
        }
        else if (abs(flr->getFloor() - elevators[i]->getCurrentFloor()->getFloor()) == abs(flr->getFloor() - candidates[0]->getCurrentFloor()->getFloor())) // add on to the the list of candidates
        {
            candidates.push_back(elevators[i]);
        }
    }

    // send the elevator already going that direction
    for (int i = 0; i < candidates.size(); ++i)
    {
        if (flr->getFloor() > candidates[i]->getCurrentFloor()->getFloor() && candidates[i]->getDirection() == UP)
        {
            *elev = candidates[i];
            return;
        }
        else if (flr->getFloor() < candidates[i]->getCurrentFloor()->getFloor() && candidates[i]->getDirection() == DOWN)
        {
            *elev = candidates[i];
            return;
        }
    }

    // since the floor is out of the way for all elevators, just send the first candidate
    *elev = candidates[0];
}

void ECS::elevatorCenteredStrategy(Floor* flr, Elevator** elev) const
{
    Elevator* candidate = elevators[0];

    for (int i = 0; i < elevators.size(); ++i)
    {
        if (flr->getFloor() - elevators[i]->getCurrentFloor()->getFloor() == 0) // elevator already on floor so send this one
        {
            *elev = elevators[i];
            return;
        }

        if (abs(flr->getFloor() - elevators[i]->getCurrentFloor()->getFloor()) < abs(flr->getFloor() - candidate->getCurrentFloor()->getFloor())) // change candidate if a closer elevator is found
        {
            candidate = elevators[i];
        }
    }

    *elev = candidate;
}

void ECS::destinationknownStrategy(Direction dir, Floor* flr, Elevator** elev) const
{
    bool elevatorOnRoute = false;
    for (int i = 0; i < elevators.size(); ++i)
    {
        if (elevators[i]->isDestination(flr))
        {
            elevatorOnRoute = true;
        }
    }

    if (elevatorOnRoute)
    {
        return;
    }

    AIStrategy(dir, flr, elev);
}

void ECS::sendElevator(Elevator* elev, Floor* flr) const
{
    cout << "sending elevator..." << endl;
    elev->addFloor(flr);
}




void ECS::updateElevatorGUI(QString currentFloorDisplay)
{
    emit updateCurrentFloor(currentFloorDisplay);
    emit updateDestinations();
}




void ECS::call911() const
{
    cout << "ecs -> calling 911..." << endl;
}

void ECS::fire(int fire)
{
    emit enableChkFireElev(!fire);

    for (int i = 0; i < elevators.size(); ++i)
    {
        elevators[i]->fire(fire);
    }
}

void ECS::overload() const
{
    /*
     * tell elevator to go to overload state?
     *
     * ...shouldn't the elevator tell itself that its overloaded?
     * ...not implemented since it doesn't really make sense rn
     */
}

void ECS::powerOut(int powerOut) const
{
    for (int i = 0; i < elevators.size(); ++i)
    {
        elevators[i]->powerOut(powerOut);
    }
}
