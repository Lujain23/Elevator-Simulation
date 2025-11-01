#include "mainwindow.h"
#include "ui_mainwindow.h"





//can remove
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //CREATE A CONTROL SYSEM
    controlSystem = new ControlSystem(this);



    //buttons
    connect(ui->fireButton,SIGNAL(released()),this,SLOT(fireButtonClicked()));
    connect(ui->obstructedButton,SIGNAL(released()),this,SLOT(obstructedButtonCLicked()));
    connect(ui->overloadButton,SIGNAL(released()),this,SLOT(overloadButtonClicked()));
    connect(ui->powerOutButton,SIGNAL(released()),this,SLOT(powerOutButtonClicked()));
    connect(ui->helpButton,SIGNAL(released()),this,SLOT(helpButtonClicked()));

    //combobox the default - that shows the current floor

    ui->downButton->hide();//when program first started it is on floor 1, so no down button
    ui->currFloor->setCurrentIndex(0); // when program first starts, on floor 1
    ui->currElevator->setCurrentIndex(0); // when program first starts, on elevator 1





    //adding functionality to the up and down buttons
    connect(ui->upButton,SIGNAL(released()),this,SLOT(upButtonClicked()));
    connect(ui->downButton,SIGNAL(released()),this,SLOT(downButtonClicked()));


    //inside elevator panel
    //adding functionality to open and close door push buttons
    connect(ui->openDoor,SIGNAL(released()),this,SLOT(openDoorButtonClicked()));
    connect(ui->closeDoor,SIGNAL(released()),this,SLOT(closeDoorButtonClicked()));


    //submit and reset button for changing simulation
    connect(ui->submitButton,SIGNAL(released()),this,SLOT(submitButtonClicked()));
    connect(ui->resetButton,SIGNAL(released()),this,SLOT(resetButtonClicked()));

    //clear browswer button
    connect(ui->clearBrowser,SIGNAL(released()),this,SLOT(clearBrowserButtonClicked()));




    //main wimdow - > ECS
   // connect(this,SIGNAL(mainSendTextSignal(const QString&)),controlSystem,SLOT(ecsReceiveTextSignal(const QString&))); //TESTING



    //ECS - > Main window
    connect(controlSystem,SIGNAL(ecsSendTextSignal(const QString&)),this,SLOT(mainReceiveTextSignal(const QString&))); //TESTING


    //send the number of elevator and floors to control system to create vector of elevators based on that number & change the total floors
    connect(this,SIGNAL(mainSendElevatorandFloorNo(int,int)),controlSystem,SLOT(ecsReceiveElevatorandFloorNo(int,int)));

    //fire signal sent
    //connect(this,SIGNAL(mainSendFireSignal()),controlSystem,SLOT(ecsReceiveFireSignal()));
    connect(controlSystem,SIGNAL(ecsSendChangeFloorSignal(int,int)),this,SLOT(mainReceiveChangeFloor(int,int)));

    //help signal sent - takes the index of the elevator that clicked the button
    //connect(this,SIGNAL(mainSendHelpSignal(int)),controlSystem,SLOT(ecsReceiveHelpSignal(int)));

    //build simulation based on default, which is 3 elevators and 7 floors
    resetButtonClicked(); //just a loophole, itll do the same thing as typing it again

    //slot signal for if the current floor on UI changes
    connect(ui->currFloor,SIGNAL(currentIndexChanged(int)),this,SLOT(floorNumberChanged(int)));

    //whenever the currentElevator on UI changes, change it in the local variable
    connect(ui->currElevator,SIGNAL(currentIndexChanged(int)),this,SLOT(currElevatorChanged(int)));

    //change the light up buttons of up and down
    connect(controlSystem,SIGNAL(ecsSendClearUpDownButton(int,QString)),this,SLOT(mainReceiveClearUpDownButton(int,QString)));

    //change the numbered buttons
    connect(controlSystem,SIGNAL(ecsSendClearFloorDestination(int,int)),this,SLOT(mainReceiveClearFloorDestination(int,int)));



}

//deconstructor already provided
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clearBrowserButtonClicked(){
    ui->browser->clear();
}

void MainWindow::fireButtonClicked(){
    ui->browser->append("Fire signal sent to controller.");

    //tell the controller
    controlSystem->fireSignalHandler();

}
void MainWindow::obstructedButtonCLicked(){

    controlSystem->obstacleSignalHandler(currElevator);

}
void MainWindow::overloadButtonClicked(){
    ui->browser->append("Max weight reached!");
    controlSystem->overloadSignalHandler(currElevator);

}
void MainWindow::powerOutButtonClicked(){
    ui->browser->append("Power out signal sent to controller!");
    controlSystem->powerOutSignalHandler();



}
void MainWindow::helpButtonClicked(){

    ui->browser->append("HELP signal sent to controller.");

    //need to get the elevator that called the help
    //tell the controller
    controlSystem->helpSignalHandler(currElevator);
    //emit mainSendHelpSignal(currElevator);

}

//this function is so that when the current index is changed in the combo box dealing with floor, the up and dowb buttons gets disabled depending on that.
void MainWindow::floorNumberChanged(int currFLoor){

    //deal with which buttons to show
    if (currFLoor == 0){
        ui->upButton->show();
        ui->downButton->hide();
    }
    else if(currFLoor == totalFloors - 1){
        ui->upButton->hide();
        ui->downButton->show();
    }
    else{
         //all floors in between
         ui->upButton->show();
         ui->downButton->show();
    }


    //deal with the lights

    if(totalFloors > 0 || totalFloors < (int)upClicked.size()){

        upAndDownColor(currFLoor);
    }
}

void MainWindow::upAndDownColor(int floorToCheck){
//    //if it was clicked before:
//    qDebug() << "floorto check value "<<floorToCheck;
//    qDebug() << "size: " << upClicked.size();

    if(floorToCheck == -1){
        return;
    }

     if(upClicked.at(floorToCheck)){
        ui->upButton->setStyleSheet("background-color: rgb(255, 190, 111);");
    }
    else{
        ui->upButton->setStyleSheet("background-color:rgb(255, 255, 255)");
    }

    if(downClicked.at(floorToCheck)){
        ui->downButton->setStyleSheet("background-color: rgb(255, 190, 111);");
    }
    else{
        ui->downButton->setStyleSheet("background-color:rgb(255, 255, 255)");
    }
}

void MainWindow::currElevatorChanged(int changedElevator){

    currElevator = changedElevator; //saved as the correct index (elevator 1 = > 0 , etc)

    floorsToLightUp();
}

//the 2 functions below deal with getting
void MainWindow::upButtonClicked(){
    int currFloor = ui->currFloor->currentIndex();

    ui->upButton->setStyleSheet("background-color: rgb(255, 190, 111);");
    upClicked.at(currFloor) = 1;
    requestPickUp("UP");
    upClicked.at(currFloor) = 0;

}

void MainWindow::downButtonClicked(){
    int currFloor = ui->currFloor->currentIndex();


    ui->downButton->setStyleSheet("background-color: rgb(255, 190, 111);");
    downClicked.at(currFloor) = 1;
    requestPickUp("DOWN");
    downClicked.at(currFloor) = 0;


}

void MainWindow::requestPickUp(QString direction){

    //current floor on screen
    int currFloor = ui->currFloor->currentIndex();



    //call control system
    controlSystem->requestPickUpHandler(currFloor+1,direction);


}



//inside elevator panel
void MainWindow::openDoorButtonClicked(){
    controlSystem->openDoorHandler(currElevator);
}

void MainWindow::closeDoorButtonClicked(){
   controlSystem->closeDoorHandler(currElevator);
}

//reset button stuff
void MainWindow::resetButtonClicked(){

     changingSimulation(3,7);

}

//submit button stuff
void MainWindow::submitButtonClicked(){

    int newTotalFLoors = ui->totalFloorsOnUI->value();
    int newTotalElevator = ui->totalElevatorOnUI->value();
    changingSimulation(newTotalElevator,newTotalFLoors);



}


void MainWindow::changingSimulation(int newTotalElevator, int newTotalFLoors){



    //first dealing with the total floors
    //change the variable
    totalFloors = newTotalFLoors;
    totalElevators = newTotalElevator;


    //clear browser
    ui->browser->clear();

    //change the drop down comboBox
    changeComboBoxNo(ui->currFloor,newTotalFLoors);

    changeComboBoxNo(ui->currElevator,newTotalElevator);


    upClicked.clear();
    downClicked.clear();

    //reset the list that holds the open door/close door
    for(int i = 0 ; i < totalFloors ; i++){
        upClicked.push_back(0);
        downClicked.push_back(0);

    }

    //delette floor buttons alreayd in list
    deleteFLoorButtonList();
    //changes the floor buttons there
    changeFloorButtons(newTotalFLoors);


    //delete the elevator Label List originally there.
    deleteElevatorLabelList();
    //changes the elevators
    changeTotalElevators(newTotalElevator);

    //also would have to change the numbers in the spinBox
    ui->totalFloorsOnUI->setValue(totalFloors);
    ui->totalElevatorOnUI->setValue(totalElevators);


    //tell the control system to change elevator and floor number
    emit mainSendElevatorandFloorNo(newTotalElevator,newTotalFLoors);

    //testing default
    int defaultFloor = 1;


    for (int i = 0 ; i < totalElevators ; i++){

        //TESTING FOR SUCCESS SCENARIO 1
//        if (i == 0){
//            defaultFloor = 6;
//        }
//        else if(i == 1){
//            defaultFloor = 2;
//        }
//        else if(i == 2){
//            defaultFloor = 5;
//        }
//        else{
//            defaultFloor = 1;
//        }

        //TESTING FOR SUCCESS SCENARIO 2
//        if (i == 0){
//            defaultFloor = 1;
//        }
//        else if(i == 1){
//            defaultFloor = 3;
//        }
//        else if(i == 2){
//            defaultFloor = 6;
//        }
//        else{
//            defaultFloor = 1;
//        }

        //TESTING FOR DEFAULT

        //has to be there  always
        controlSystem->setFloorOfElevator(i,defaultFloor);

    }



}
void MainWindow::deleteElevatorLabelList(){
    for (QLCDNumber* l:elevatorLabelList){
        delete l;
    }
    elevatorLabelList.clear();
}

void MainWindow::deleteFLoorButtonList(){
    for (QPushButton* b :floorButtonsList){
        delete b;
    }
    floorButtonsList.clear();
}

void MainWindow::changeTotalElevators(int newTotalElevators){
    //default floor(1)
    int defaultFloor;

    //clear elevators that already exist
    QLayoutItem* item_to_delete;
    while((item_to_delete = ui->allElevatorsLayout->takeAt(0))){
        if(item_to_delete->widget()){
            delete item_to_delete->widget();
        }
        delete item_to_delete;

    }

    //add new elevators
    for(int i = 1 ;  i <= newTotalElevators ; i ++){ //theres no leak of memory since when deleting i delete the item AND memory(at the top)

        //TESTING FOR SUCCESS SCENARIO 1
//        if (i == 1){
//            defaultFloor = 6;
//        }
//        else if(i == 2){
//            defaultFloor = 2;
//        }
//        else if(i ==3){
//            defaultFloor = 5;
//        }
//        else{
//            defaultFloor = 1;
//        }
        //TESTING FOR SUCCESS SCENARIO 2

//        if (i == 1){
//            defaultFloor = 1;
//        }
//        else if(i == 2){
//            defaultFloor = 3;
//        }
//        else if(i == 3){
//            defaultFloor = 6;
//        }
//        else{
//            defaultFloor = 1;
//        }

        //default  always
        defaultFloor =1;




        //this is the illusion of the elevator
        QFrame* elevator =  new QFrame(this);
        elevator->setObjectName(QString("Elevator%1").arg(i));
        elevator->setStyleSheet("background-color:rgb(192, 191, 188)");
        //elevator->setFixedSize(200,100);

        //each elevator should have a label & the current floor theyre on
        //first label

        QLabel* elevatorNumber = new QLabel(QString("Elevator %1").arg(i), this);
        //floor number

        QLCDNumber* elevatorFloorLCD = new QLCDNumber(defaultFloor, this);
        //so it handles different digit numbers
        elevatorFloorLCD->setDigitCount(2);

        //just the style of it

        elevatorFloorLCD->setStyleSheet("background-color: rgb(255, 255, 255);  QLCDNumber { color: black; font-weight: bold; }");


//        elevatorFloorLCD->setSegmentStyle(QLCDNumber::Flat);
        elevatorFloorLCD->display(defaultFloor);

        //putting the labels in a vector so i can keep chaning them throughtout
        //elevatorLabelList.push_back(elevatorFloor); //CHANGED
        elevatorLabelList.push_back(elevatorFloorLCD);

        //need to create a new "layout" that puts it all together, didnt work without it
        QVBoxLayout* currElevator = new QVBoxLayout(elevator);
        currElevator->addWidget(elevatorNumber);
        currElevator->addWidget(elevatorFloorLCD);


        ui->allElevatorsLayout->addWidget(elevator);




    }

}

//this is when the total floor numbers change to change inside elevator panel
void MainWindow::changeFloorButtons(int newTotalFloors){
    //clear the buttons already there
    QLayoutItem* item_to_delete;
    while ((item_to_delete = ui->floorNumbers->takeAt(0))){
        if(item_to_delete->widget()){
            delete item_to_delete->widget();
        }
        delete item_to_delete;
    }

    //add the new buttons
    for(int i = 1;  i <= newTotalFloors ; i ++){ //theres no leak of memory since when deleting i delete the item AND memory(at the top)
        QPushButton* floorButton =  new QPushButton(QString("%1").arg(i),this);
        floorButton->setObjectName(QString("%1").arg(i));
        floorButton->setStyleSheet("background-color:rgb(255,255,255)");
        floorButtonsList.push_back(floorButton);

        //need to create signals for when they are clicked
        connect(floorButton,SIGNAL(released()),this,SLOT(floorButtonClicked()));


        ui->floorNumbers->addWidget(floorButton);
    }




}

void MainWindow::floorsToLightUp(){

    set<int> floors =controlSystem->getFloorStops(currElevator);
    //clear out whats colored
    for(size_t i = 0 ; i < floorButtonsList.size() ; i++){
        floorButtonsList.at(i)->setStyleSheet("background-color:rgb(255, 255, 255)");
    }

    if (floors.empty()){
        //do nothing
    }

    else{

        //not empty, so there were stops in that elevator
        //for (size_t i = 0 ; i < floors.size() ; i ++){
        for(int i : floors){
            int floorNo = i;


            (floorButtonsList.at(floorNo -1))->setStyleSheet("background-color: rgb(255, 190, 111);");

        }

    }



}

void MainWindow::floorButtonClicked(){
    //who sent the signal? => said to look use obj cast to be able to use sender();
    QPushButton* clickedFloor = qobject_cast<QPushButton*>(sender());


   // qInfo() << "this is what was clicked: "<<  clickedFloor->objectName();
    //also need to find which elevator it was: have a variable currElevator

    //qInfo()<< "the curr elevetor IN FLOOR BUTTON CLICKED " <<currElevator;
    //when the button is clicked give an indication
    clickedFloor->setStyleSheet("background-color: rgb(255, 190, 111);");
    int floorNo = (clickedFloor->objectName()).toInt();
    //qInfo() <<"floor number clicked is: "<< floorNo;
    controlSystem->floorNumberClickedHandler(currElevator,floorNo);

}


//helper function for adding the floor numbers in comboBox
void MainWindow::changeComboBoxNo(QComboBox* combo,int newMax){

    combo->clear();

    for(int i = 1 ; i <= newMax ; i++){
        QString toAdd = QString::number(i);

//        if(combo->findText(toAdd) == -1){ //wasnt found
//            combo->addItem(QString::number(i));
//        }
        combo->addItem(QString::number(i));

    }


}





//signals
void  MainWindow::mainReceiveTextSignal(const QString& text){
    ui->browser->append(text);
    ui->browser->append("--------------------------------------------");
}


void MainWindow::mainReceiveChangeFloor(int elevatorNo,int floorChange){
    //arguments => the elevator label * the new floor change

    QString output =QString(" Elevator %1 moving to floor %2").arg(elevatorNo).arg(floorChange);
   // qDebug()<<output;
    elevatorLabelList.at(elevatorNo)->display(floorChange);

}


void MainWindow::mainReceiveClearUpDownButton(int floorNo,QString direction){


    if (direction == "UP"){
        upClicked.at(floorNo-1) =0;
        ui->upButton->setStyleSheet("background-color:rgb(255, 255, 255);");
    }
    else{
        downClicked.at(floorNo-1)=0;
        ui->downButton->setStyleSheet("background-color:rgb(255, 255, 255);");
    }
}


void MainWindow::mainReceiveClearFloorDestination(int elevatorUsed,int destination){
    //only if it hasnt switched
    if(elevatorUsed == currElevator){
        floorButtonsList.at(destination - 1)->setStyleSheet("background-color:rgb(255, 255, 255);");
    }

}

