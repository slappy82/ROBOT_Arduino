/*
 * Campbell Maxwell
 * August 2020 - Present
 * Module responsible for all movement of the robotics project via the twin stepper motors.
 */

 #ifndef STEPPER_MOVEMENT_MODULE_H
 #define STEPPER_MOVEMENT_MODULE_H

 #include <Arduino.h>

 class Stepper_Movement_Module {
    private:
    // PINS
        const uint8_t LEFT_MOTOR_PINS[4];                       // Contains the set of pins connected to the left motor
        const uint8_t RIGHT_MOTOR_PINS[4];                      // Contains the set of pins connected to the right motor
    // VARIABLES
        const uint8_t STEP_CYCLE_2_6;                           // Amount of steps in each stepperMove loop using 2 and 6 as forward and reverse (eg every second step)
        const uint8_t STEP_CYCLE_3_5;                           // Amount of steps using 3 and 5 (eg every 3rd step - needs longer delay)
        const uint8_t BASE_MOVEMENT;                            // Predetermined amount of steps to use as a base movement amount (2mm = 37 steps)
        const uint8_t MS_DELAY;                                 // Amount of delay (in milliseconds) between motor steps
        const uint16_t US_DELAY;                                // Amount of delay (in microseconds)
        const uint16_t TURN_90_STEPS;                           // Amount of steps for 90 degree turn in stepperMove loop (for actual value need length of 45* of circle with radius = distance between wheels / 2, then just number of steps for that distance
    // FUNCTIONS
        void stepperMove(uint8_t, uint8_t, const uint16_t);     // Take an int value for left and right motor to determine direction. 4 is the centerpoint so a 4 for each motor will stop them. 1-3 will set motor in reverse, while 5-7 will go forwards
        void stepperMoveSheet(uint8_t[], int);                  // Method to control state changes of pins connected to stepper. Each case is a new step configuration

    public:
    // VARIABLES
        const uint8_t MOVE_REVERSE;                             // Int value to start driving motor forward
        const uint8_t MOVE_FORWARD;                             // Int value to drive motors in reverse
        const uint8_t MOVE_STOP;                                // Int value which will stop motor movement
    // FUNCTIONS
        /*
         * Constructor will initialise the pins and handle any initial setup required for this class
         */
        Stepper_Movement_Module();                             
        /*
         * Called to setup this module
         */
        void stepperSetup();                                    
        /*
         * This method can be used to pass in the packet values from the BLE module to control the movement. I set a predetermined value for the total 
         * number of steps to avoid having to constantly check the BLE module, but hopefully to still be responsive enough.
         * @param 1 - Takes the integer value from the first byte of BLE data packet, corresponding to the LEFT motor direction
         * @param 2 - Takes the integer value from the second byte of BLE data packet, corresponding to the RIGHT motor direction
         */
        void stepperBLEMove(uint8_t, uint8_t);      
        /*         
         * This method used for Forward or Reverse for the automatic AI control          
         * @param - Use one of the static class members (MOVE_FORWARD or MOVE_REVERSE) to determine the linear direction
         */
        void stepperAutoLinear(uint8_t);      
        /*                 
         * Turn the robot to the left by 90 degrees using the stepper motors (for automatic AI control)                 
         */
        void stepperAutoLeft90();
        /*
         * Turn the robot to the right by 90 degrees using the stepper motors (for automatic AI control)
         */
        void stepperAutoRight90();
 };
 #endif
