#include "Elevator.h"

ECS* Elevator::ecs;
int Elevator::nextId = 0;

// setup
Elevator::Elevator(Floor* floor, QObject *parent) : QObject{parent}, id(nextId++), runTimer(new QTimer(this)), timer(new QTimer(this)), state(DEFAULT), dir(NONE), disembark(false), moving(false), closing(false), open(false), weight(0.0f), currentFloor(floor)
{
    ecs->addElevator(this);

    connect(this, SIGNAL(requestHelp()), ecs, SLOT(call911()));
}

Elevator::~Elevator()
{
    while (true)
    {
        Passenger* p = passengers[passengers.size()-1];
        passengers.pop_back();
        delete p;

        if (passengers.size() == 0)
        {
            break;
        }
    }

    delete runTimer;
    delete timer;
}


// getters
ECS* Elevator::getECS() { return ecs; }

State Elevator::getState() const { return state; }

Direction Elevator::getDirection() const
{
    return dir;
}

bool Elevator::isOpen() const { return open; }

int Elevator::getWeight() const { return weight; }

Floor* Elevator::getCurrentFloor() const { return currentFloor; }

QList<Floor*> Elevator::getDestinations() const { return destinations; }

void Elevator::setECS(ECS* e) { ecs = e; }


// setters
void Elevator::setState(State state) { this->state = state; }

void Elevator::setCurrentFloor(Floor* floor) { this->currentFloor = floor; }

void Elevator::setWeight(int weight)
{
    this->weight = weight;

    cout << "\nElevator " << id << " weight: " << weight << "lbs" << endl;

    if (weight > MAX_CAPACITY)
    {
        state = OVERLOAD;
        return;
    }

    state = DEFAULT;
}


// other
void Elevator::start()
{
    cout << "Elevator " << id << ": running" << endl;
    connect(runTimer, SIGNAL(timeout()), this, SLOT(run()));
    runTimer->start(2000);
}

void Elevator::run()
{
    switch (state)
    {
        case HELP:
            connectToSafetyService();
            call911();
            break;
        case DOOR_OBSTACLE:
            openDoor();
            break;
        case FIRE:
            move(true);
            break;
        case OVERLOAD:
            closeDoor();
            break;
        case POWER_OUT:
            move(true);
            break;
        case DEAD:
            dead();
            break;
        case RESET:
            reset();
            break;
        default:
            move();
            break;
    }
}




bool Elevator::isDestination(Floor* flr)
{
    return destinations.contains(flr);
}

void Elevator::addFloor(Floor* floor)
{
    cout << "Elevator " << id << ": floor added..." << endl;
    destinations.push_back(floor);
}

void Elevator::removeFloor(Floor* floor)
{
    destinations.removeOne(floor);
}




void Elevator::move(bool moveToSafeFloor)
{
    if (disembark)
    {
        return;
    }

    if (moveToSafeFloor) // finds the closest safe floor and prioritizes moving to said floor
    {
        // find the closest safe floor and put it to the front of the destinations QList
        Floor* closestSafeFlr;
        if (findClosestSafeFloor(&closestSafeFlr))
        {
            destinations.push_front(closestSafeFlr);
        }
        else
        {
            cout << "\nWARNING! None of the floors are deemed safe, so Elevator " << id << endl;
            cout << "\t will remain in place and closed until further notice"  << endl;

            cout << "\n*Please Note: elevators will automatically traverse and open up to a floor once it is deemed safe*" << endl;
        }
    }

    if (destinations.empty() || moving) // no need to move if theres no destinations to go to
    {
        return;
    }

    // decide which direction to go
    if (currentFloor->getFloor() < destinations[0]->getFloor())        // destination is ABOVE the current floor
    {
        dir = UP;
    }
    else if (currentFloor->getFloor() > destinations[0]->getFloor())   // destination is BELOW the current floor
    {
        dir = DOWN;
    }
    else                                                               // destination is the CURRENT floor
    {
        dir = NONE;
    }

    // no need to simulate time to traverse floors since the current floor is the destination
    if (dir == NONE)
    {
        arriveAtFloor();
        return;
    }

    // simulate time to traverse floors
    moving = true;

    delete timer;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() { arriveAtFloor(); });
    timer->start(MOVE_DURATION);
}

void Elevator::arriveAtFloor()
{
    // update current floor
    switch (dir)
    {
        case UP:
            currentFloor = ecs->getFloor(currentFloor->getFloor()+1);
            break;
        case DOWN:
            currentFloor = ecs->getFloor(currentFloor->getFloor()-1);
            break;
        default:
            break;
    }

    // debug
    cout << "Elevator " << id << ": current floor = " << currentFloor->getFloor() << endl;

    // update gui
    emit floorArrival(currentFloor->getFloor());
    ecs->updateElevatorGUI("Current Floor: " + QString::number(currentFloor->getFloor()));

    // check if floor is within destinations array
    if (isDestination(currentFloor))
    {
        removeFloor(currentFloor);
        ecs->updateElevatorGUI("Current Floor: " + QString::number(currentFloor->getFloor()));

        // only stop if the floor is safe
        if (currentFloor->isSafe())
        {
            if (state == FIRE || state == POWER_OUT)
            {
                disembark = true;
            }

            stop();
            return;
        }

        // unsafe floor arrival procedure
        cout << "WARNING! Elevator " << id << " is unable to open due to floor " << currentFloor->getFloor() << " being unsafe" << endl;
        delete timer;               // end floor traversal simulation since elevator has arrived at said floor
        timer = new QTimer(this);   // |
    }

    // allow elevator traversal since either the current floor is not a destination or floor arrival procedure has been completed (ie. for unsafe floors that is)
    canMove();
}

void Elevator::stop()
{
    ringBell();
    openDoor();
}

void Elevator::ringBell() const
{
    bell.ring();
}

void Elevator::openDoor()
{
    cout << "Elevator " << id << ": Door is opened..." << endl;

    // simulate duration of doors being opened
    delete timer;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(closeDoor()));
    timer->start(DOOR_DURATION);
}

void Elevator::closeDoor()
{
    closing = true;
    switch (state)
    {
        case DOOR_OBSTACLE:
            ringBell();
            cout << "Elevator " << id << ": doorway light sensors triggered, reopening door..." << endl;
            closing = false;
            break;
        case FIRE:
            displayWarning("WARNING! There has been a fire, please disembark");
            state = DEAD;
            break;
        case OVERLOAD:
            ringBell();
            cout << "Elevator " << id << ": Door is closing..." << endl;
            cout << "ERROR! Unable to close door..." << endl;
            displayWarning("WARNING! Cargo load exceeds max capacity, please reduce weight");
            break;
        case POWER_OUT:
            displayWarning("WARNING! There has been a power outage, please disembark");
            state = DEAD;
            break;
        default:
            ringBell();
            cout << "Elevator " << id << ": Door is closing..." << endl;

            // simulate duration of doors closing
            delete timer;
            timer = new QTimer(this);
            connect(timer, SIGNAL(timeout()), this, SLOT(canMove()));
            timer->start(CLOSE_DURATION);
            break;
    }
}

void Elevator::canMove()
{
    moving = false;
    closing = false;

    delete timer;
    timer = new QTimer(this);
}




void Elevator::help(bool help)
{
    if (!help)
    {
        disconnectFromSafetyService();
        cout << "\nElevator " << id << " state = DEFAULT..." << endl;
        state = DEFAULT;
        return;
    }

    cout << "\nElevator " << id << " state = HELP..." << endl;
    state = HELP;
    emit requestHelp();
}

void Elevator::doorObstacle(int doorObs)
{
    if (!doorObs)
    {
        cout << "\nElevator " << id << " state = DEFAULT..." << endl;
        state = DEFAULT;
        return;
    }

    if (!closing) // can't have a door obstacle in a situation where the door is not even closing
    {
        cout << "DEBUG ERROR: door obstacle not possible since door isn't attempting to close right now" << endl;
        return;
    }

    cout << "\nElevator " << id << " state = DOOR_OBSTACLE..." << endl;
    state = DOOR_OBSTACLE;
}

void Elevator::fire(int fire)
{
    if (!fire)
    {
        state = RESET; // reset since passengers prob don't plan on going to their original destination now
        return;
    }

    cout << "\nElevator " << id << " state = FIRE..." << endl;
    displayWarning("EMERGENCY! A fire has broken out, moving to safe floor...");
    state = FIRE;
}

void Elevator::dead()
{
    disembark = false;
    canMove();

    delete timer;
    timer = new QTimer(this);
}

void Elevator::powerOut(int powerOut)
{
    if (!powerOut)
    {
        state = RESET; // reset since passengers prob don't plan on going to their original destination now
        return;
    }

    cout << "\nElevator " << id << " state = POWER_OUT..." << endl;
    displayWarning("EMERGENCY! A power out has occured, moving to safe floor...");
    state = POWER_OUT;
}

void Elevator::reset()
{
    cout << "\n\nElevator " << id << " state = RESET..." << endl;
    cout << "Elevator " << id << " is resetting..." << endl;

    state = RESET;

    // reset elevator data (ie. destinations and passengers) *Note: passengers are simply for simulation not applicable to real life*
    destinations.clear();
    passengers.clear();     // might have to edit this later based on how passengers disembark (prob with signals)
    weight = 0;             // |
    disembark = false;

    // update gui
    ecs->updateElevatorGUI("Current Floor: " + QString::number(currentFloor->getFloor()));

    cout << "...Elevator " << id << " reset complete" << endl;
    cout << "\nElevator " << id << " state = DEFAULT..." << endl;
    state = DEFAULT;
}




bool Elevator::findClosestSafeFloor(Floor** flr) const
{
    int loopAmount = 0;

    /*
     * This if statement is to efficiently determine how many times to
     * loop up and down from the current floor in search for the closest safe floor
     *
     * *Note: I do (currentFloor->getFloor()+1)) because the floor levels starts at 0 not 1*
     */
    if (ecs->getFloorQuantity() - (currentFloor->getFloor()+1) > (currentFloor->getFloor()+1))
    {
        loopAmount = ecs->getFloorQuantity() - (currentFloor->getFloor()+1);
    }
    else
    {
        loopAmount = (currentFloor->getFloor()+1);
    }

    for (int i = 0; i < ecs->getFloorQuantity(); ++i)
    {
        if (currentFloor->getFloor()+i < ecs->getFloorQuantity() && ecs->getFloor(currentFloor->getFloor()+i)->isSafe()) // determine if index is within bounds and floor at said inde is safe
        {
            *flr = ecs->getFloor(currentFloor->getFloor()+i);
            return true;
        }

        if (currentFloor->getFloor()-i > -1 && ecs->getFloor(currentFloor->getFloor()-i)->isSafe()) // determine if index is within bounds and floor at said inde is safe
        {
            *flr = ecs->getFloor(currentFloor->getFloor()-i);
            return true;
        }
    }

    *flr = NULL;
    return false;
}

void Elevator::connectToSafetyService() const
{
    cout << "Elevator " << id << ": Connected to safety service..." << endl;
}

void Elevator::disconnectFromSafetyService() const
{
    cout << "...Elevator " << id << " disconnected from safety service" << endl;
}

void Elevator::call911() const
{
    cout << "Elevator " << id << ": elevator -> calling 911..." << endl;
}

void Elevator::displayWarning(const string& msg) const
{
    audioSystem.play(msg);
    displaySystem.display(msg);
}
