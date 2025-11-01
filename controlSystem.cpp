#include "controlSystem.h"


//constructor
ControlSystem::ControlSystem(QObject* ui): QObject(ui){


}

ControlSystem::~ControlSystem(){

    for (Elevator* e: elevatorList){
        delete e;
    }
 }



void ControlSystem::ecsReceiveElevatorandFloorNo(int elevator,int floor){

    qDebug() << "Message from ControlSystem, gotten from main (no. of floors ):" << floor;
    qDebug() << "Message from ControlSystem, gotten from main (no. of elevators ):" << elevator;


    //delete whats originally there
    if(!elevatorList.empty()){
        deleteElevatorList();
    }





    //create elevator list
    for (int i = 0 ; i < elevator; i ++){
        Elevator* e = new Elevator(this);
        elevatorList.push_back(e);
        //this is to send the floor and elevtaor numvber
        connect(e,SIGNAL(elevatorSendNumberandFloor(int,int)),this,SLOT(updateElevatorFloor(int,int)));
        //to clear the up button light
        connect(e,SIGNAL(clearUpDownButton(int, QString)),this,SLOT(receiveClearUpDownButton(int, QString)));
        //for receiving messgaes
         connect(e,SIGNAL(elevatorSendText(QString)),this,SLOT(receiveTextFromElevator(QString)));
         //for clearing the floor numbered buttons
         connect(e,SIGNAL(clearFloorDestinationButton(int,int)),this,SLOT(receiveClearFloorDestination(int,int)));
         //connect for obstructed signal
         connect(e,SIGNAL(elevatorSendObstructedSignal(int)),this,SLOT(ecsReseiveObstructedSignal(int)));

    }


    //keep note of the total number of floors/elevators at any given moment
    totalFloors = floor;
    totalElevators = elevator;


}

//this is from elevator -> CS -> main
void ControlSystem::receiveClearUpDownButton(int floorNo,QString direction){
    emit ecsSendClearUpDownButton(floorNo,direction);
}

//this is from elevator -> CS -> main
void ControlSystem::receiveClearFloorDestination(int elevatorNo, int destination){
    emit ecsSendClearFloorDestination(elevatorNo,destination);
}

//when they change the number in between dealign w memory correctly
void ControlSystem::deleteElevatorList(){
    for (Elevator* e: elevatorList){
        delete e;
    }

    elevatorList.clear();

}

//all safety handlers
void ControlSystem::fireSignalHandler(){
    qDebug() << "Message from ControlSystem, gotten from main: Fire Signal Received.";

    emit ecsSendTextSignal("MESSAGE: All Elevators are moving to Floor 2 (safe floor)");
    emit ecsSendTextSignal("MESSAGE: Please Disembark when safe floor is reached.");

    moveAllElevators();

}


void ControlSystem::helpSignalHandler(int elevatorUsed){

    //since help is called, doors wont be able to open. meaning direction inactive
    QString dir  = QString("inHelp");
    elevatorList.at(elevatorUsed)->setDirection(dir);

    QString output = QString("Connecting Building Safety Service to Elevator %1.").arg(elevatorUsed+1);
    emit ecsSendTextSignal(output);


    QTimer* time =new QTimer(this);
    time->setSingleShot(true);
    time->setInterval(10000);
    connect(time,&QTimer::timeout,this,[this,elevatorUsed](){
        callBuildingSafety(elevatorUsed);
    });
    time->start();



}

//in a timer connect, has to call a new function
void ControlSystem::callBuildingSafety(int elevatorUsed){
    emit ecsSendTextSignal("BSS: Hello! How can I help?");

    //now direction should be ok
    elevatorList.at(elevatorUsed)->setDirection("INACTIVE");

}

void ControlSystem::obstacleSignalHandler(int elevatorUsed){


    QString output = QString("Light Sensor interrupted in Elevator %1.").arg(elevatorUsed+1);
    emit ecsSendTextSignal(output);

    emit ecsSendTextSignal("Stopping Door from closing.");
    emit ecsSendTextSignal("MESSAGE: Please remove obstacle blocking the door.");

    elevatorList.at(elevatorUsed)->setDirection("obstructed");


    //giving them time to close door
    QTimer* time =new QTimer(this);


    connect(time,&QTimer::timeout,this,[this,time,elevatorUsed](){
        emit ecsSendTextSignal("Attemping to close Door again.");
        closeDoorHandler(elevatorUsed);
        elevatorList.at(elevatorUsed)->setDirection("INACTIVE");
        time->stop(); //after done it stops
    });

    time->start(10000);


}



void ControlSystem::overloadSignalHandler(int elevatorUsed){


    QString output = QString("Overload signal received from Elevator %1.").arg(elevatorUsed+1);
    emit ecsSendTextSignal(output);

    QString output2 = QString("Elevator %1 door remains open.").arg(elevatorUsed+1);
    emit ecsSendTextSignal(output2);
    emit ecsSendTextSignal("MESSAGE: Please reduce load.");

    //have to wait here




    //giving them time to close door
    QTimer* time =new QTimer(this);


    connect(time,&QTimer::timeout,this,[this,time,elevatorUsed](){
        emit ecsSendTextSignal("Weight decreased.");
        elevatorList.at(elevatorUsed)->closeDoorMessage(elevatorUsed);
        elevatorList.at(elevatorUsed)->setDirection("INACTIVE");
        time->stop(); //after done it stops
    });

    time->start(10000);



}

void ControlSystem::powerOutSignalHandler(){
    qDebug() << "Message from ControlSystem, gotten from main: Power Out Signal Received.";

    emit ecsSendTextSignal("MESSAGE: There is a Power Outage.");
    emit ecsSendTextSignal("MESSAGE: All Elevators are moving to Floor 2 (safe floor)");
    emit ecsSendTextSignal("MESSAGE: Please Disembark when safe floor is reached.");

    moveAllElevators();


}


void ControlSystem::moveAllElevators(){




    for(int i = 0 ; i < totalElevators ; i++){
        qDebug() <<"elevator? " <<i;

        //updateElevatorFloor(i,2);
        QString newDirection = "INACTIVE";
        Elevator* e = elevatorList.at(i);
        int currFloor= e->getCurrFloor();


        //if its the same floor just break
        if(currFloor == 2){
            e->setDirection(newDirection);

        }

        else{
            while (e->getCurrFloor() !=  2){



                QTimer *timer = new QTimer();
                QEventLoop eventLoop;
                connect(timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
                timer->start(1000);
                eventLoop.exec();



                if(e->getCurrFloor() < 2){
    //                qInfo() <<"before Increment"<<e->getCurrFloor();
                    e->incrementFloor();
    //                qInfo() <<"After Increment"<<e->getCurrFloor();
                }
                else{
    //                qInfo() <<"before decrement"<<e->getCurrFloor();
                    e->decrementFloor();
    //                qInfo() <<"After decrement"<<e->getCurrFloor();
                }

                emit ecsSendChangeFloorSignal(i,e->getCurrFloor());





            }
        }


        e->setDirection(newDirection);
        e->openDoorMessage(i);




    }


}





void ControlSystem::updateElevatorFloor(int currElevator, int newFloor){

     QString output = QString("Elevator %1 moving to Floor %2.").arg(currElevator+1).arg(newFloor);

     //emit ecsSendTextSignal(output);
     emit ecsSendChangeFloorSignal(currElevator,newFloor);
     elevatorList.at(currElevator)->setCurrFloor(newFloor);

}

//inside elevator panel, a floor is clicked
void ControlSystem::floorNumberClickedHandler(int currElevator,int floorClicked){
    QThread *currElevatorThread = new QThread(this);


    connect(currElevatorThread, &QThread::started,this, [this,currElevatorThread,currElevator,floorClicked] (){
        elevatorList.at(currElevator)->initialMove(currElevator,floorClicked);
        currElevatorThread->quit();
     });

    currElevatorThread->start();

}

set<int> ControlSystem::getFloorStops(int currElevator){

    if (currElevator >= 0 && currElevator < (int)elevatorList.size()){

        return (elevatorList.at(currElevator))->getFloorStops();
    }
    else{
        return {};
    }

}

//now when the up/down button is clicked this deals with finding the closestELevator to that floor
int ControlSystem::findClosestElevator(int floor,QString direction){

    if(elevatorList.empty()){
        return -1; //make sure i do the check but with the way its implemented will never be used
    }

    //find elevators that COULD be used --> this means ignore all elevators that are obstructed or in help
    vector<int> validElevators;

    for(int  i = 0 ; i < (int)elevatorList.size() ; i++){

        QString currElevatorDirection = elevatorList.at(i)->getDirection();
        if(currElevatorDirection == "inhelp" || currElevatorDirection == "obstructed"){
        }
        else{
            validElevators.push_back(i);
        }

    }

    //if no valid elevators cant do anything
    if (validElevators.empty()){
        return -1;
    }

    int leastDiff = 99999;

    int leastDiffElevator = 0;

    for(int i : validElevators){

        int currDiff = abs(elevatorList.at(i)->getCurrFloor() - floor);
        QString currElevatorDirection = elevatorList.at(i)->getDirection();

        if(currElevatorDirection == direction || currElevatorDirection == "INACTIVE"){
            if (currDiff < leastDiff){
                leastDiff = currDiff;
                leastDiffElevator = i;
            }

        }


    }

    return leastDiffElevator;
}


void ControlSystem::setFloorOfElevator(int elevator, int newCurrFloor){
    elevatorList.at(elevator)->setCurrFloor(newCurrFloor);
}


//lets deal with moving passengers
void ControlSystem::requestPickUpHandler(int destination, QString direction){



    //first step: find closest Elevator
    int closestElevator = findClosestElevator(destination,direction);

    QString output = QString("The closest Elevator from Floor %1 is Elevator %2 ").arg(destination).arg(closestElevator + 1);
    qDebug() << output ;

    //second step: make it seem like that elevator is moving TO that destination
    //would have to create its own thread
    QThread *currElevatorThread = new QThread(this);


    connect(currElevatorThread, &QThread::started,this, [this,currElevatorThread,closestElevator,destination,direction] (){
            elevatorList.at(closestElevator)->requestFloor(closestElevator,destination,direction);

            currElevatorThread->quit();
     });

     currElevatorThread->start();


}



void ControlSystem::ecsReseiveObstructedSignal(int elevatorUsed){
    //this means that they tried closing door --> had to create a new function for it, toherwisde infite
    QString output = QString("Light Sensor interrupted in Elevator %1.").arg(elevatorUsed+1);
    emit ecsSendTextSignal(output);


    emit ecsSendTextSignal("Stopping Door from closing.");
    emit ecsSendTextSignal("MESSAGE: Please remove obstacle blocking the door.");


    //giving them time to close door
    QTimer* time =new QTimer(this);


    connect(time,&QTimer::timeout,this,[this,time,elevatorUsed](){


        elevatorList.at(elevatorUsed)->closeDoorMessage(elevatorUsed);
        time->stop(); //after done it stops

    });

    time->start(1000);


}


void ControlSystem::openDoorHandler(int elevator){
    Elevator* e = elevatorList.at(elevator);

    e->openDoor(elevator);

}

void ControlSystem::closeDoorHandler(int elevatorUsed){

    elevatorList.at(elevatorUsed)->closeDoor(elevatorUsed);
}


void ControlSystem::receiveTextFromElevator(QString message){
    emit ecsSendTextSignal(message);
}


