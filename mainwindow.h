#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//the items in UI
#include <QComboBox>

#include <QMainWindow>



#include "controlSystem.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    //buttons
    void fireButtonClicked();
    void obstructedButtonCLicked();
    void overloadButtonClicked();
    void powerOutButtonClicked();
    void helpButtonClicked();

    //current floor number combo box
    void floorNumberChanged(int currFloor);

    //current elevator on ui combo box
    void currElevatorChanged(int currElevator);

    //up and down buttons
    void downButtonClicked();
    void upButtonClicked();


    //creating new simulations
    void submitButtonClicked();
    void resetButtonClicked();

    //inside elevator
    void openDoorButtonClicked();
    void closeDoorButtonClicked();

    //clear browser
    void clearBrowserButtonClicked();


    //this is for figuring it out what floor numbe rin the "inside elevator" button was clicked
    void floorButtonClicked();

    //needs ot be a slot
    void requestPickUp(QString);
    void mainReceiveClearUpDownButton(int floorNo,QString);
    void mainReceiveClearFloorDestination(int,int);



public slots:
    //signals and stuff
    void mainReceiveTextSignal(const QString&);
    void mainReceiveChangeFloor(int,int);




signals:

    void mainSendElevatorandFloorNo(int elevator, int floor);



private:
    Ui::MainWindow *ui;
    int totalFloors = 7;
    int totalElevators = 3;

    int currElevator = 0;

    void changeComboBoxNo(QComboBox* combo,int newMax);
    void changeFloorButtons(int totalFloors);
    void changeTotalElevators(int newTotalElevators);
    void changingSimulation(int newElevators, int newFloors);

    void floorsToLightUp();

    void upAndDownColor(int floorToCheck); //deals with lights of up and down buttons

    //label list functions
    void deleteElevatorLabelList();
    void deleteFLoorButtonList();


    //testing
    ControlSystem* controlSystem;

    //list of elevator labels so i can change them
    vector<QLCDNumber*> elevatorLabelList;
    vector<QPushButton*> floorButtonsList;
    vector<int> upClicked;
    vector<int> downClicked;

};
#endif // MAINWINDOW_H
