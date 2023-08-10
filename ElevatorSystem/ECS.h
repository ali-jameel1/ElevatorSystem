#ifndef ECS_H
#define ECS_H

#include <QObject>
#include <QDebug>
#include <QList>

#include "defs.h"
#include "Floor.h"

using namespace std;

class Elevator;

class ECS : public QObject
{
    public:
        explicit ECS(int, int, QObject *parent = nullptr);
        ~ECS();

        Floor* getFloor(int) const;
        int getFloorQuantity() const;
        Elevator* getElevator(int) const;

        void addFloor(Floor*);
        void addElevator(Elevator*);

        void run() const;

        void handleRequest(Direction, Floor*) const;                    // handles elevator requests received from floors
        void findSuitableElevator(Direction, Floor*, Elevator**) const;
        void AIStrategy(Direction, Floor*, Elevator**) const;
        void elevatorCenteredStrategy(Floor*, Elevator**) const;
        void destinationknownStrategy(Direction, Floor*, Elevator**) const;
        void sendElevator(Elevator*, Floor*) const;                     // tells Elevator to use addFloor()

        void updateElevatorGUI(QString);

    private:
        Q_OBJECT

        Strategy strategy;

        QList<Floor*> floors;
        QList<Elevator*> elevators;

    signals:
        void updateCurrentFloor(QString);
        void updateDestinations();

        void enableChkFireElev(bool);

    public slots:
        void swapStrategy(int);

        void call911() const;
        void fire(int);
        void overload() const;
        void powerOut(int) const;
};

#endif // ECS_H
