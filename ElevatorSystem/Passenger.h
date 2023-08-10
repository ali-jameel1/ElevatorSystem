#ifndef PASSENGER_H
#define PASSENGER_H

#include <QObject>

class Passenger : public QObject
{
    public:
        explicit Passenger(int = 100, QObject *parent = nullptr);

        float getWeight() const;
        int getDestination() const;

    private:
        Q_OBJECT

        float weight;
        int destination;

    signals:


    public slots:

};

#endif // PASSENGER_H
