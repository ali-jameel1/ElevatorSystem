#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QObject>
#include <QTimer>
#include <algorithm>
#include <QList>

#include "defs.h"

#include "ECS.h"

#include "AudioSystem.h"
#include "DisplaySystem.h"
#include "Bell.h"

#include "Floor.h"
#include "Passenger.h"

using namespace std;

class Elevator : public QObject
{
    public:
        // setup
        explicit Elevator(Floor*, QObject *parent = nullptr);
        ~Elevator();

        // getters
        static ECS* getECS();

        State getState() const;
        Direction getDirection() const;
        bool isOpen() const;
        int getWeight() const;
        Floor* getCurrentFloor() const;
        QList<Floor*> getDestinations() const;

        // setters
        static void setECS(ECS*);

        void setState(State);
        void setCurrentFloor(Floor*);

        // other
        void start();

        bool isDestination(Floor*);
        void removeFloor(Floor*);    // removes specified floor from the destinations array

        void stop();
        void ringBell() const;

        void connectToSafetyService() const;
        void disconnectFromSafetyService() const;
        void call911() const;

        bool findClosestSafeFloor(Floor**) const;
        void displayWarning(const string&) const;   // calls the display() and play() functions of displaySystem and audioSystem

    private:
        Q_OBJECT

        static ECS* ecs; // one ECS is shared across all elevators

        static int nextId;
        int id;

        QTimer* runTimer;
        QTimer* timer;

        State state;
        Direction dir;

        /*
         * The following are parts of the elevator...
         * so they'll be limited to the elevator scope and
         * only accessible through the elevator
         */
        AudioSystem audioSystem;
        DisplaySystem displaySystem;
        Bell bell;

        bool disembark;
        bool moving;
        bool closing;
        bool open;
        int weight;
        Floor* currentFloor;

        QList<Floor*> destinations;
        QList<Passenger*> passengers;
        /*
         * the passengers array is for simulation purposes...
         * the irl sensors will be able to detect the weight change of the cargo,
         * but for the simulation a record must be kept to remember how much the
         * cargo weight will change from floor to floor
         *
         * therefore passengers will be limited to the elevator scope and will not
         * be accessible to the ECS (Elevator Control System)
         */

    signals:
        void floorArrival(int);
        void requestHelp();
        void overload();

    public slots:
        // setters
        void setWeight(int);

        // other
        void run();

        void addFloor(Floor*);                      // adds specified floor to the destinations array (for handling ECS elevator requests)
        void move(bool moveToSafeFloor = false);
        void arriveAtFloor();
        void openDoor();
        void closeDoor();

        void canMove();

        void help(bool);
        void doorObstacle(int);
        void fire(int);
        void dead();
        void powerOut(int);
        void reset();
};

#endif // ELEVATOR_H

