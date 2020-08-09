/*
 * Campbell Maxwell
 * August 2020 - Present
 */

#include "Stepper_Movement_Module.h"

/*
 * Constructor for Stepper_Movement_Module class
 */
Stepper_Movement_Module::Stepper_Movement_Module() : STEP_CYCLE_2_6(4),
                                                     STEP_CYCLE_3_5(8),    
                                                     BASE_MOVEMENT(28),                         // 2mm worth of steps = 37 steps
                                                     TURN_90_STEPS(500),                                                     
                                                     MOVE_REVERSE(6),        
                                                     MOVE_FORWARD(2), 
                                                     MOVE_STOP(4),  
                                                     MS_DELAY(2),         
                                                     US_DELAY(2000),     
                                                     LEFT_MOTOR_PINS({22, 23, 24, 25}),
                                                     RIGHT_MOTOR_PINS({26, 27, 28, 29}) {
    //Stepper_Movement_Module::stepperSetup();
}
/*
 * Sets up the pins which connect to each motor, and anything else this module requires
 */
void Stepper_Movement_Module::stepperSetup() {
    for (int i = 0; i < 4; i++) {
        pinMode(LEFT_MOTOR_PINS[i], OUTPUT);
        pinMode(RIGHT_MOTOR_PINS[i], OUTPUT);
    }
}
/*
 * Take an int value for left and right motor to determine direction. 4 is the centerpoint so a 4 for each motor will stop them. 1-3 will set motor 
 * in reverse, while 5-7 will go forwards.
 * @param left - The direction value for the LEFT hand motor
 * @param right - The direction value for the RIGHT hand motor
 * @param STEP_TOTAL - Takes the number of steps to be performed
 */
void Stepper_Movement_Module::stepperMove(uint8_t left, uint8_t right, const uint16_t STEP_TOTAL){
    uint8_t lMotorStep = 0; 
    uint8_t rMotorStep = 0; 
    for (uint16_t stepCount = 0; stepCount < STEP_TOTAL; stepCount++){                                                
        Stepper_Movement_Module::stepperMoveSheet(LEFT_MOTOR_PINS, lMotorStep);   
        Stepper_Movement_Module::stepperMoveSheet(RIGHT_MOTOR_PINS, rMotorStep);    
        lMotorStep = (lMotorStep + left) % 8;
        rMotorStep = (rMotorStep + right) % 8;
        delay(MS_DELAY);
    }
}
/*
 * This method can be used to pass in the packet values from the BLE module to control the movement. I set a predetermined value for the total number 
 * of steps to avoid having to constantly check the BLE module, but hopefully to still be responsive enough.
 * @param left - The direction value for the LEFT hand motor
 * @param right - The direction value for the RIGHT hand motor
 */
void Stepper_Movement_Module::stepperBLEMove(uint8_t left, uint8_t right){
    if (left > 1 && right > 1 && left != 4) {                    // replace this when I fix packets on android end, eg: can have a bit switch 
        /*Serial.println(left);
        Serial.println(right);*/
        Stepper_Movement_Module::stepperMove(left, right, BASE_MOVEMENT);   
    }
}
/*
 * This method used for Forward or Reverse for the automatic AI control
 * @param stepDirection - The direction value for BOTH motors (eg: a linear movement)
 */
void Stepper_Movement_Module::stepperAutoLinear(uint8_t stepDirection) {
    Stepper_Movement_Module::stepperMove(stepDirection, stepDirection, (BASE_MOVEMENT * 2));    // Give it more action per cycle
}
/*
 * Turn the robot to the left by 90 degrees using the stepper motors (for automatic AI control)
 */
void Stepper_Movement_Module::stepperAutoLeft90(){
    Stepper_Movement_Module::stepperMove(MOVE_REVERSE, MOVE_FORWARD, TURN_90_STEPS);
}
/*
 * Turn the robot to the right by 90 degrees using the stepper motors (for automatic AI control)
 */
void Stepper_Movement_Module::stepperAutoRight90(){
    Stepper_Movement_Module::stepperMove(MOVE_FORWARD, MOVE_REVERSE, TURN_90_STEPS);
}
/*
 * Method to control state changes of pins connected to stepper. Each case is a new step configuration
 * @param motor - Takes an array containing a set of a motor's pins
 * @param motorStep - Takes the particular step we want to use to choose the correct pin configuration
 */
void Stepper_Movement_Module::stepperMoveSheet(uint8_t motor[], int motorStep){
  switch(motorStep){
    case 0:
      digitalWrite(motor[3], HIGH);  // 4
      digitalWrite(motor[2], LOW);   // 3
      digitalWrite(motor[1], LOW);   // 2
      digitalWrite(motor[0], LOW);   // 1
      break;
    case 1:
      digitalWrite(motor[3], HIGH);
      digitalWrite(motor[2], HIGH);
      digitalWrite(motor[1], LOW);
      digitalWrite(motor[0], LOW);
      break;
    case 2:
      digitalWrite(motor[3], LOW);
      digitalWrite(motor[2], HIGH);
      digitalWrite(motor[1], LOW);
      digitalWrite(motor[0], LOW);
      break;
    case 3:
      digitalWrite(motor[3], LOW);
      digitalWrite(motor[2], HIGH);
      digitalWrite(motor[1], HIGH);
      digitalWrite(motor[0], LOW);
      break;
    case 4:
      digitalWrite(motor[3], LOW);
      digitalWrite(motor[2], LOW);
      digitalWrite(motor[1], HIGH);
      digitalWrite(motor[0], LOW);
      break;
    case 5:
      digitalWrite(motor[3], LOW);
      digitalWrite(motor[2], LOW);
      digitalWrite(motor[1], HIGH);
      digitalWrite(motor[0], HIGH);
      break;
    case 6:
      digitalWrite(motor[3], LOW);
      digitalWrite(motor[2], LOW);
      digitalWrite(motor[1], LOW);
      digitalWrite(motor[0], HIGH);
      break;
    case 7:
      digitalWrite(motor[3], HIGH);
      digitalWrite(motor[2], LOW);
      digitalWrite(motor[1], LOW);
      digitalWrite(motor[0], HIGH);
      break;
    default:
      digitalWrite(motor[3], LOW);
      digitalWrite(motor[2], LOW);
      digitalWrite(motor[1], LOW);
      digitalWrite(motor[0], LOW);
      break;
  }
}
