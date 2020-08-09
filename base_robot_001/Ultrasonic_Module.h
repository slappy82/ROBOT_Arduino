/*
 * Campbell Maxwell
 * July 2020 - Present
 * Ultrasonic module for Robotics project. Responsible for proximity detection and trigonometry function for depth detection.
 *     const float SG90_HYP_OFFSETS[10] = {0.707f, 0.5f, 0.423f, 0.342f, 0.259f};  68mm, 96mm, 113mm, 140mm, 185mm
 */

#ifndef ULTRASONIC_MODULE_H
#define ULTRASONIC_MODULE_H
 
#include <Arduino.h>

 class Ultrasonic_Module {
    private:
    // PINS
        const uint8_t TRIG_PIN;                 // SONAR Trig pin
        const uint8_t ECHO_PIN;                 // SONAR Echo pin
        const uint8_t PWM_PIN;                  // SG90 PWM pin
    // FUNCTIONS
        int calculateSteps(int, int);           // Return the total number of steps calculated before a recheck is needed
        void setSG90Position(int);              // Set the position of the servo using the PWM high duration as a parameter
        int getSonarValue();                    // Read the ultrasonic sensor and return the distance value

    public:
    // FUNCTIONS
        /*
         * Constructor will initialise the pins and handle any initial setup required for this class
         */
        Ultrasonic_Module();                    
        /*
         * Called to setup this module
         */
        void ultrasonicSetup();                
        /*
         * Method to return the number of steps required to reach the furthest 'safe' distance based on trig calculations
         * @return - Integer value containing the number of steps
         */
        int SG90TrigModule();                   
        /*
         * Method to determine whether the robot is within a predetermined proximity range of an object
         * @return - Boolean value FALSE if we are within proximity of an object, TRUE if not
         */
        bool noCollisionImminent();             
};

#endif
