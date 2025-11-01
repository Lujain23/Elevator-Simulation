#ifndef CONTROLSYSTEM_H
#define CONTROLSYSTEM_H

//normal includes


#include "elevator.h"



using namespace std;

class ControlSystem: public QObject {

    Q_OBJECT

public:
        //constructors
        ControlSystem(QObject* ui);



        //setters
        ~ControlSystem();


        //getters and setters
        set<int> getFloorStops(int elevator);
        void setFloorOfElevator(int elevator, int newCurrFloor); // this is when the floor changes it instantly changes here too




        //safety handlers
        void fireSignalHandler();
        void helpSignalHandler(int elevatorUsed);
        void powerOutSignalHandler();
        void obstacleSignalHandler(int elevatorUsed);
        void overloadSignalHandler(int elevatorUsed);


        //open and close door
        void openDoorHandler(int elevatorUsed);




        //moving elevator
        int findClosestElevator(int floor,QString direction);
        void floorNumberClickedHandler(int elevatorNo,int floorNO ); //will take the elevator and the floor that was clicked



private:

        //variables
        int totalFloors;
        int totalElevators;

        //started adding myself
        vector<Elevator*> elevatorList;
        int safeFloor = 2;

        //functions
        void deleteElevatorList();
        void moveAllElevators(); //helper function bcz i realized power out & fire move all elevators to safe floor.

public slots:
        void requestPickUpHandler(int destination, QString direction);
        void receiveClearUpDownButton(int floorNo,QString);
        void receiveTextFromElevator(QString);
        void receiveClearFloorDestination(int elevatorNo, int destination);
        void closeDoorHandler(int);





private slots:
        void ecsReceiveElevatorandFloorNo(int,int);
        void callBuildingSafety(int); //called after a a timer signal is sent
        void updateElevatorFloor(int currElevator, int newFloor); //called after a timer signal
        void ecsReseiveObstructedSignal(int elevatorUsed);




//means this is sent from the ECS to main
signals:
        void ecsSendTextSignal(const QString&);
        void ecsSendChangeFloorSignal(int elevatorNo,int newFloor);
        void ecsSendClearUpDownButton(int floorNo,QString);
        void ecsSendClearFloorDestination(int,int);


};



#endif // CONTROLSYSTEM_H
