#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QRadioButton>
#include <QCheckBox>
#include <QSlider>
#include <QInputDialog>

#include <iostream>

#include "ECS.h"
#include "Floor.h"
#include "Elevator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        void setupUI(int, int);
        void setupFlrView(int);
        void setupElevView(int, int);
        void setupECSView();

    private:
        Ui::MainWindow *ui;

        QList<QRadioButton*> floorDestinationRadioButtons;
        QList<QCheckBox*> floorSafetyCheckBoxes;
        QList<QSlider*> elevators;

        ECS* ecs;
        Floor* flr;
        Elevator* elev;

        QTimer* openTimer;

    signals:


    public slots:
        // ui to simulation
        void updateFloor(int);

        void elevatorRequested();
        void addDestination();

        void openDoorPressed();
        void openDoorReleased();
        void closeDoor();

        void help(bool);
        void doorObstacle(int);
        void fire(int);
        void updateWeight(int);

        // simulation to ui
        void updateElevatorGUI(int);
        void updateLitFloorButtons();
};

#endif // MAINWINDOW_H
