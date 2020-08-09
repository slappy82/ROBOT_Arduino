/* 
 *  Campbell Maxwell
 *  September 2019 - Present
 */

#include "Ultrasonic_Module.h"
#include "BLE_Communication_Module.h"
#include "Stepper_Movement_Module.h"
 
BLE_Communication_Module BLEModule;
Stepper_Movement_Module motorModule;
Ultrasonic_Module sonarModule;

/////////////////////////////////////////////////////////
// ROUTINE METHODS
/////////////////////////////////////////////////////////
/*
 * Base AI routine to loop, check for incoming BLE connection and check SONAR for proximity, if neither is triggered go forwards a few steps, if
 * BLE connection is made, transfer to remote control, if proximity is detected, turn left then right until clear
 */
void baseAI() {
    uint8_t BLEMoveCommand;
    while (BLEMoveCommand = BLEModule.checkBLEConnection(), BLEMoveCommand) {
        BLEModule.decodeByte(BLEMoveCommand);             // +++currently convuluted method of passing packet, need to fix android side also redundant++++
        motorModule.stepperBLEMove(BLEModule.packetData[BLEModule.LEFT], BLEModule.packetData[BLEModule.RIGHT]);   
    }
    if (sonarModule.noCollisionImminent()) {
        motorModule.stepperAutoLinear(motorModule.MOVE_FORWARD); 
    }
    else {
        motorModule.stepperAutoLeft90();
        while (!sonarModule.noCollisionImminent()){
            motorModule.stepperAutoRight90(); 
        }
    }
}

/////////////////////////////////////////////////////////
// SETUP
/////////////////////////////////////////////////////////
void setup() {
    BLEModule.BLESetup();
    motorModule.stepperSetup();
    sonarModule.ultrasonicSetup();
    console.println("Starting...");
}
/////////////////////////////////////////////////////////
// ROUTINE
/////////////////////////////////////////////////////////
void loop() {
    baseAI();
}
