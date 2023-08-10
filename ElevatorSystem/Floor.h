#ifndef FLOOR_H
#define FLOOR_H

#include <QObject>

#include <iostream>

using namespace std;

class Floor : public QObject
{
    Q_OBJECT
    public:
        explicit Floor(QObject *parent = nullptr);

        int getFloor() const;
        int isSafe() const;

    private:
        static int nextFloor;
        int floor;
        bool safe;

    signals:
        void requestElevator();

    public slots:
        void updateSafety();
};

#endif // FLOOR_H
