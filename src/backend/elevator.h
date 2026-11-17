
#ifndef ELEVATOR_H
#define ELEVATOR_H

//string printing stuff
#include <string>
#include <sstream>
#include <iostream>
#include <QString>
#include <QDebug>

//data management
#include <QSet>
#include <set>
#include <vector>


//ui stuff
#include <QObject>
#include <QLayoutItem>
#include <QLayout>
#include <QPushButton>
#include <QFrame>
#include <QLCDNumber>
#include <QLabel>

//concurrency stuff
#include <QThread>
#include<QTimer>
#include <QEventLoop>



using namespace std;

class Elevator: public QObject   {

    Q_OBJECT

public:
		//constructors
        Elevator(QObject* ui);


		//deconstructor
        ~Elevator();


		//getters and setters
        set<int>& getFloorStops();
        QString getDirection() {return direction; }
        int getCurrFloor() {return currFloor;}
        void setDirection(QString direction);
        void setCurrFloor(int newFloor);


        void incrementFloor() {currFloor++; }
        void decrementFloor() {currFloor--; }


        //functions
        void addFloor(int floorNo);
        void printStops();

        //door realted
        void closeDoor(int elevatorUsed);
        void openDoor(int elevatorNo);
        void openDoorMessage(int elevatorNo);
        void closeDoorMessage(int elevatorNo);
        bool isObstructed();

        //moving elevator functions
        void requestFloor(int elevatorNo,int destination,QString direction);
        void moveFloors(set<int>& setToUse,int elevatorNo, int destination,QString direction);
        void initialMove(int elevatorNo, int destination);


        
private slots:
          void openDoorClickedAgain(int elevatorNo);


signals:
         void elevatorSendNumberandFloor(int ,int);
         void clearUpDownButton(int floorNo,QString direction);
         void elevatorSendText(QString);
         void clearFloorDestinationButton(int elevatorNo,int destination);
         void elevatorSendObstructedSignal(int elevatorUsed);



private:

        set<int> floorStops;
        set<int> requestStops;
        int currFloor; //this is the floor the elevators on rn
        QString direction;
        bool obstructed;
        QTimer* openDoorTimer = nullptr;




};

#endif
