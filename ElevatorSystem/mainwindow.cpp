#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), openTimer(new QTimer(this))
{
    ui->setupUi(this);

    // settings prompt
    int input = QInputDialog::getInt(NULL, "Elevator Control System Simulation", "Please enter an integer from 2 to 7 (inclusive), this will represent the number of floors.");
    while (input == 0 || !(1 < input && input < 8)) // get input for the floor quantity
    {
        input = QInputDialog::getInt(NULL, "Elevator Control System Simulation", "Please enter an integer from 2 to 7 (inclusive), this will represent the number of floors.");
    }
    int numFlrs = input; // design ver. uses 4

    input = QInputDialog::getInt(NULL, "Elevator Control System Simulation", "Please enter an integer from 2 to 5 (inclusive), this will represent the number of elevators.");
    while (input == 0 || !(1 < input && input < 6)) // get input for the elevator quantity
    {
        input = QInputDialog::getInt(NULL, "Elevator Control System Simulation", "Please enter an integer from 2 to 5 (inclusive), this will represent the number of elevators.");
    }
    int numElevs = input; // design ver. uses 2

    ecs = new ECS(numFlrs, numElevs);

    // removing design view parts
    ui->Elevators->layout()->removeWidget(ui->Elevator0);   // sliders for elevator visuals
    ui->Elevators->layout()->removeWidget(ui->Elevator1);   // |
    delete ui->Elevator0;                                   // |
    delete ui->Elevator1;                                   // |

    ui->FSDCheckBoxes->layout()->removeWidget(ui->chkB);    // safeness checkboxes for each floor
    ui->FSDCheckBoxes->layout()->removeWidget(ui->chk1);    // |
    ui->FSDCheckBoxes->layout()->removeWidget(ui->chk2);    // |
    ui->FSDCheckBoxes->layout()->removeWidget(ui->chk3);    // |
    delete ui->chkB;                                        // |
    delete ui->chk1;                                        // |
    delete ui->chk2;                                        // |
    delete ui->chk3;                                        // |

    ui->ECPPVFloors->layout()->removeWidget(ui->rdo0);      // floor buttons for the elevator's control panel
    ui->ECPPVFloors->layout()->removeWidget(ui->rdo1);      // |
    ui->ECPPVFloors->layout()->removeWidget(ui->rdo2);      // |
    ui->ECPPVFloors->layout()->removeWidget(ui->rdo3);      // |
    delete ui->rdo0;                                        // |
    delete ui->rdo1;                                        // |
    delete ui->rdo2;                                        // |
    delete ui->rdo3;                                        // |

    // setup the UI
    setupUI(numFlrs, numElevs);

    // run simulation
    ecs->run();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete ecs;

    // not sure if this is needed
    /*
    while (!elevators.isEmpty()) // deallocate previous siders and empty elevators
    {
        delete elevators.back();
        elevators.pop_back();
    }

    while (!floorSafetyCheckBoxes.isEmpty()) // deallocate previous checkboxes and empty floorSafetyCheckBoxes
    {
        delete floorSafetyCheckBoxes.back();
        floorSafetyCheckBoxes.pop_back();
    }

    while (!floorDestinationRadioButtons.isEmpty()) // deallocate previous radio buttons and empty floorDestinationRadioButtons
    {
        delete floorDestinationRadioButtons.back();
        floorDestinationRadioButtons.pop_back();
    }
    */
}

void MainWindow::setupUI(int numFlrs, int numElevs)
{
    setupFlrView(numFlrs);
    setupElevView(numFlrs, numElevs);
    setupECSView();


    // setup elevator visuals
    for (int i = 0; i < elevators.size(); ++i) // clear any previous elevator sliders
    {
        ui->Elevators->layout()->removeWidget(elevators[i]);
    }

    while (!elevators.isEmpty()) // deallocate previous siders and empty elevators
    {
        delete elevators.back();
        elevators.pop_back();
    }

    for (int i = 0; i < numElevs; ++i) // create and add new elevator sliders
    {
        // create slider
        QSlider* sldr = new QSlider(Qt::Vertical);

        // setup slider
        sldr->setObjectName("Elevator" + QString::number(i));

        sldr->setMinimum(0);
        sldr->setMaximum(numFlrs-1);

        sldr->setTickPosition(QSlider::TicksLeft);
        sldr->setTickInterval(1);

        sldr->setValue(ecs->getElevator(i)->getCurrentFloor()->getFloor());

        sldr->setAttribute(Qt::WA_TransparentForMouseEvents);

        // add slider
        elevators.push_back(sldr);
        ui->Elevators->layout()->addWidget(sldr);

        // connect slider
        connect(ecs->getElevator(i), SIGNAL(floorArrival(int)), sldr, SLOT(setValue(int)));
    }


    // setup floor safety debug
    for (int i = 0; i < floorSafetyCheckBoxes.size(); ++i) // clear any previous floor safety check boxes
    {
        ui->FSDCheckBoxes->layout()->removeWidget(floorSafetyCheckBoxes[i]);
    }

    while (!floorSafetyCheckBoxes.isEmpty()) // deallocate previous checkboxes and empty floorSafetyCheckBoxes
    {
        delete floorSafetyCheckBoxes.back();
        floorSafetyCheckBoxes.pop_back();
    }

    for (int i = 0; i < numFlrs; ++i) // create and add new floor safety check boxes
    {
        // create checkbox
        QCheckBox* chk;
        switch (i)
        {
            case 0:
                chk = new QCheckBox("B");
                break;
            default:
                chk = new QCheckBox(QString::number(i));
                break;
        }

        // setup checkbox
        chk->setChecked(true);

        // add checkbox
        floorSafetyCheckBoxes.push_back(chk);
        ui->FSDCheckBoxes->layout()->addWidget(chk);

        // connect checkbox
        connect(chk, SIGNAL(stateChanged(int)), ecs->getFloor(i), SLOT(updateSafety()));
    }
}

void MainWindow::setupFlrView(int numFlrs)
{
    // setup floor combo box
    ui->cboFloors->clear();
    for (int i = 0; i < numFlrs; ++i)
    {
        switch (i)
        {
            case 0:
                ui->cboFloors->addItem("B");
                break;
            default:
                ui->cboFloors->addItem(QString::number(i));
                break;
        }
    }
    ui->cboFloors->setCurrentIndex(1); // sets inital floor to floor 1 (ie. ground floor)

    flr = ecs->getFloor(ui->cboFloors->currentIndex()); // get floor

    connect(ui->cboFloors, SIGNAL(activated(int)), this, SLOT(updateFloor(int)));
    connect(ui->btnUp, SIGNAL(pressed()), this, SLOT(elevatorRequested()));
    connect(ui->btnDown, SIGNAL(pressed()), this, SLOT(elevatorRequested()));
}

void MainWindow::setupElevView(int numFlrs, int numElevs)
{
    // setup elevator combo box
    ui->cboElevators->clear();
    for (int i = 0; i < numElevs; ++i)
    {
        ui->cboElevators->addItem(QString::number(i));
    }
    connect(ui->cboElevators, SIGNAL(activated(int)), this, SLOT(updateElevatorGUI(int)));


    // get elevator
    elev = ecs->getElevator(ui->cboElevators->currentIndex());


    // setup elevator's floor display
    ui->ECPFloor->setText("Current Floor: " + QString::number(elev->getCurrentFloor()->getFloor()));
    connect(ecs, SIGNAL(updateCurrentFloor(QString)), ui->ECPFloor, SLOT(setText(QString)));  // update current floor displau


    // setup floor destination buttons
    for (int i = 0; i < floorDestinationRadioButtons.size(); ++i) // clear any previous floor destination radio buttons
    {
        ui->ECPPVFloors->layout()->removeWidget(floorDestinationRadioButtons[i]);
    }

    while (!floorDestinationRadioButtons.isEmpty()) // deallocate previous radio buttons and empty floorDestinationRadioButtons
    {
        delete floorDestinationRadioButtons.back();
        floorDestinationRadioButtons.pop_back();
    }

    for (int i = 0; i < numFlrs; ++i) // create and add new floor destination radio buttons
    {
        // create radio button
        QRadioButton* rdo = new QRadioButton();

        // setup radio button
        rdo->setObjectName("rdo" + QString::number(i));
        rdo->setLayoutDirection(Qt::RightToLeft);
        rdo->setAutoExclusive(false);
        switch (i)
        {
            case 0:
                rdo->setText("B");
                break;
            default:
                rdo->setText(QString::number(i));
                break;
        }

        // add radio button
        floorDestinationRadioButtons.push_back(rdo);
        ui->ECPPVFloors->layout()->addWidget(rdo);

        // connect radio button
        connect(rdo, SIGNAL(released()), this, SLOT(addDestination()));
    }


    // setup elevators
    connect(ecs, SIGNAL(updateDestinations()), this, SLOT(updateLitFloorButtons()));  // update lit floor buttons

    connect(ui->btnOpen, SIGNAL(pressed()), this, SLOT(openDoorPressed()));     // open button (held depressed)
    connect(ui->btnOpen, SIGNAL(released()), this, SLOT(openDoorReleased()));   // |
    connect(ui->btnClose, SIGNAL(clicked(bool)), this, SLOT(closeDoor()));          // close button (pressed)
    connect(ui->btnHelp, SIGNAL(toggled(bool)), this, SLOT(help(bool)));        // help button

    connect(ui->chkDoorObs, SIGNAL(stateChanged(int)), this, SLOT(doorObstacle(int)));  // door obstacle checkbox
    connect(ui->chkFireElev, SIGNAL(stateChanged(int)), this, SLOT(fire(int)));         // elevator fire checkbox
    connect(ui->sldrWeight, SIGNAL(valueChanged(int)), this, SLOT(updateWeight(int)));  // cargo weight slider
}

void MainWindow::setupECSView()
{
    // setup strategy combo box
    ui->cboStrats->clear();
    ui->cboStrats->addItem("AI");
    ui->cboStrats->addItem("Elevator-Centered");
    ui->cboStrats->addItem("Destination-Known");

    connect(ui->chkFireBuld, SIGNAL(stateChanged(int)), ecs, SLOT(fire(int)));                  // building fire checkbox
    connect(ecs, SIGNAL(enableChkFireElev(bool)), ui->chkFireElev, SLOT(setEnabled(bool)));    // |
    connect(ui->chkPowerOut, SIGNAL(stateChanged(int)), ecs, SLOT(powerOut(int)));              // power out checkbox
    connect(ui->cboStrats, SIGNAL(activated(int)), ecs, SLOT(swapStrategy(int)));               // strategy combo box
}




void MainWindow::updateFloor(int f)
{
    flr = ecs->getFloor(f);

    // determine which floor buttons are enabled
    if (flr->getFloor() == 0)                               // disable down button for bottom floor
    {
        ui->btnUp->setEnabled(true);
        ui->btnDown->setEnabled(false);
    }
    else if (flr->getFloor() == ecs->getFloorQuantity()-1)  // disable down button for top floor
    {
        ui->btnUp->setEnabled(false);
        ui->btnDown->setEnabled(true);
    }
    else
    {
        ui->btnDown->setEnabled(true);
        ui->btnUp->setEnabled(true);
    }
}




void MainWindow::elevatorRequested()
{
    if (sender()->objectName() == "btnUp") // floor request up
    {
        ecs->handleRequest(UP, flr);
        updateLitFloorButtons();
    }
    else // floor request down
    {
        ecs->handleRequest(DOWN, flr);
        updateLitFloorButtons();
    }
}

void MainWindow::addDestination()
{
    int f = sender()->objectName().remove(0, 3).toInt();
    elev->addFloor(ecs->getFloor(f));
    updateLitFloorButtons();
}




void MainWindow::openDoorPressed()
{
    cout << "Keeping door opened..." << endl;

    // ui
    ui->btnOpen->setChecked(true);

    // start timer
    connect(openTimer, SIGNAL(timeout()), elev, SLOT(openDoor()));
    openTimer->start(REOPEN_DOOR_INTERVAL);
}

void MainWindow::openDoorReleased()
{
    cout << "...no longer keeping door opened" << endl;

    // ui
    ui->btnOpen->setChecked(false);

    // reset timer
    delete openTimer;
    openTimer = new QTimer(this);
}

void MainWindow::closeDoor()
{
    elev->closeDoor();
    ui->btnClose->setChecked(false);
}




void MainWindow::help(bool help)
{
    elev->help(help);
}

void MainWindow::doorObstacle(int obstacle)
{
    elev->doorObstacle(obstacle);
}

void MainWindow::fire(int fire)
{
    elev->fire(fire);
}

void MainWindow::updateWeight(int weight)
{
    elev->setWeight(weight);
}




void MainWindow::updateElevatorGUI(int e)
{
    elev = ecs->getElevator(e);

    // passenger view
    ui->ECPFloor->setText("Current Floor: " + QString::number(elev->getCurrentFloor()->getFloor()));
    updateLitFloorButtons();

    // debug
    ui->sldrWeight->setValue(elev->getWeight());
    ui->chkDoorObs->setChecked(elev->getState() == DOOR_OBSTACLE);
    ui->chkFireElev->setChecked(elev->getState() == FIRE);
}

void MainWindow::updateLitFloorButtons()
{
    for (int i = 0; i < floorDestinationRadioButtons.size(); ++i)
    {
        floorDestinationRadioButtons[i]->setChecked(elev->isDestination(ecs->getFloor(i)));
    }
}
