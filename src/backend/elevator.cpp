#include "elevator.h"
#include <QThread>

//construct elevator
Elevator::Elevator(QObject* ui): QObject(ui) {
    currFloor = 1;
    direction = "INACTIVE";


}

//decontructor
Elevator::~Elevator() {

}



void Elevator::printStops(){

    for(int i: floorStops){
        qDebug() <<"Stops are: "<< i;
    }

}

void Elevator::addFloor(int floorNo){
    floorStops.insert(floorNo);
}


set<int>& Elevator::getFloorStops(){
    return floorStops;
}

void Elevator::setCurrFloor(int newFloor){
    this->currFloor = newFloor;
}

void Elevator::setDirection(QString newDirection){
    this->direction = newDirection;
}

void Elevator::closeDoor(int elevatorUsed){

    obstructed  = isObstructed();

    //door cant close if its obstructed:
    if(obstructed){
        qDebug() << "in the if obstructed";
        //call the obstructed signal
        emit elevatorSendObstructedSignal(elevatorUsed);
        obstructed = false;

    }
    else{
        closeDoorMessage(elevatorUsed);

    }

}

void Elevator::openDoor(int elevatorNo){
    QString inactive = "INACTIVE";
    //first elevator should not be moving.

     qDebug() << "elevator direction is: " << this->direction;



     //if its help => doors can not be opened
     if ((this->direction).compare("inHelp") == 0){
         emit elevatorSendText("Elevator in help. Door can not be opened.");
     }


     //if its stationary
    else if(this->direction == inactive){


         //first to check if this is the first time open door clicked
         if(!openDoorTimer){
             // qDebug() << "first time being clicked";
             openDoorTimer = new QTimer(this);
             openDoorTimer->setSingleShot(true);

             connect(openDoorTimer,&QTimer::timeout,this,[this,elevatorNo](){
                 openDoorClickedAgain(elevatorNo);
             });


         }

         //if it already running
         if(openDoorTimer->remainingTime() > 0){
             openDoorTimer->stop();
             openDoorTimer->start(openDoorTimer->remainingTime() + 10000);
             emit elevatorSendText("Added 10 seconds");

         }



         //if its been clicked before, but the timer is done now
         //this would mean just start it noormally
         else{
             openDoorMessage(elevatorNo);
             qDebug() <<"ended before";
             openDoorTimer->start(10000);


         }


     }

     //last option, elevator is moving up or down
     else{
         emit elevatorSendText("Elevator is Moving. Door can not be opened.");
     }
}






void Elevator::openDoorClickedAgain(int elevatorNo){
    closeDoorMessage(elevatorNo);

}

void Elevator::openDoorMessage(int elevatorNo){
    QString output2 = QString("RING RING RING Elevator %1 Door opened.").arg(elevatorNo +1);
    emit elevatorSendText(output2);
}

void Elevator::closeDoorMessage(int elevatorNo){
    QString output1 = QString("RING RING RING Elevator %1 Door closed.").arg(elevatorNo +1);
    emit elevatorSendText(output1);
}



void Elevator::requestFloor(int elevatorNo,int destination,QString floorDirection){

    requestStops.insert(destination);

    if (currFloor == destination){

        //wait for 2 seconds b4 clearing
        QTimer timeRequestSameFloor;
        timeRequestSameFloor.setSingleShot(true);
        QEventLoop loopRequestSameFloor;
        connect(&timeRequestSameFloor, &QTimer::timeout, &loopRequestSameFloor, &QEventLoop::quit);
        timeRequestSameFloor.start(1000);
        loopRequestSameFloor.exec();


        emit clearUpDownButton(destination,floorDirection);
        requestStops.erase(destination);

        openDoorMessage(elevatorNo);

        //stimulate a wait
        QTimer timerForDoor;
        QEventLoop loopForDoor;
        timerForDoor.setSingleShot(true);
        QObject::connect(&timerForDoor,&QTimer::timeout,&loopForDoor,&QEventLoop::quit);
        timerForDoor.start(5000);
        loopForDoor.exec();

        closeDoorMessage(elevatorNo);


    }


    //if theres only IS a request for the elevator
    else if(!(requestStops.empty())){
        moveFloors(requestStops,elevatorNo,destination,floorDirection);
        direction ="INACTIVE";
    }

}

void Elevator::initialMove(int elevatorNo, int destination){

     floorStops.insert(destination);
     //if its the same floor
    if(currFloor ==  destination){
        //do everything normally
        //wait for 2 seconds b4 clearing
        QTimer timeStopSameFloor;
        timeStopSameFloor.setSingleShot(true);
        QEventLoop loopStopSameFloor;
        connect(&timeStopSameFloor, &QTimer::timeout, &loopStopSameFloor, &QEventLoop::quit);
        timeStopSameFloor.start(1000);
        loopStopSameFloor.exec();


        emit clearFloorDestinationButton(elevatorNo,currFloor);
        floorStops.erase(destination);

        openDoorMessage(elevatorNo);

        //stimulate a wait
        QTimer timerForDoor;
        QEventLoop loopForDoor;
        timerForDoor.setSingleShot(true);
        QObject::connect(&timerForDoor,&QTimer::timeout,&loopForDoor,&QEventLoop::quit);
        timerForDoor.start(5000);
        loopForDoor.exec();

        closeDoorMessage(elevatorNo);

    }


     else if (floorStops.size() == 1){
        moveFloors(floorStops,elevatorNo,destination,"NONE");
        direction ="INACTIVE";
    }
}



void Elevator::moveFloors(set<int> &setToUse, int elevatorNo, int destination, QString newDirection){


        while (!setToUse.empty()){

            while (currFloor != destination){

                QTimer *timer = new QTimer(this);
                QEventLoop eventLoop;
                connect(timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
                timer->start(1000);
                eventLoop.exec();



                if (currFloor < destination){
                    direction = "UP";

                    currFloor++;
                }

                else {
                    direction = "DOWN";
                    currFloor--;
                }


                emit elevatorSendNumberandFloor(elevatorNo,currFloor);

                if (setToUse.find(currFloor) != setToUse.end()){

//                  qDebug()<<"number is stoped at:" <<currFloor;


                    setToUse.erase(currFloor);
                    emit elevatorSendNumberandFloor(elevatorNo,currFloor);


                    if (newDirection == "NONE"){
                        emit clearFloorDestinationButton(elevatorNo,currFloor);
                    }
                    else{
                        emit clearUpDownButton(destination,newDirection);
                    }



                    openDoorMessage(elevatorNo);

                    //stimulate a wait
                    QTimer timerForDoor;
                    QEventLoop loopForDoor;
                    timerForDoor.setSingleShot(true);
                    QObject::connect(&timerForDoor,&QTimer::timeout,&loopForDoor,&QEventLoop::quit);
                    timerForDoor.start(5000);
                    loopForDoor.exec();

                    closeDoorMessage(elevatorNo);



                }


            }

            if (!setToUse.empty()){

//                qDebug()<< "this is the thing rn";
//                for(int i : setToUse){
//                    qDebug() <<i;
//                }
                destination = *setToUse.rbegin();
            }


        }
}






bool Elevator::isObstructed(){
    //randomize it
//    int num  = rand() % 11;
//    return (num % 2 == 0);  //this just works if its even return true (door is obstructed), even odd => door is not obstructed

    return false;//TESTING


}



