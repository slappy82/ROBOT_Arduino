/*
 * Campbell Maxwell
 * August 2020 - Present
 */

#include "Ultrasonic_Module.h"

/*
 * Constructor for Ultrasonic_Module class
 */
Ultrasonic_Module::Ultrasonic_Module() : TRIG_PIN(3), 
                                         ECHO_PIN(2), 
                                         PWM_PIN(5) {
}
/*
 * Method to setup pin assignment and anything else needed for this module to run
 */
void Ultrasonic_Module::ultrasonicSetup() {
    pinMode(ECHO_PIN, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(PWM_PIN, OUTPUT);
}
/*
 * Method to return the distance value from the sonar (in mm) to the nearest object
 * @return - An integer value of the distance (in mm)
 */
int Ultrasonic_Module::getSonarValue(){
    digitalWrite(TRIG_PIN, LOW);                    
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);                   
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    int duration = pulseIn(ECHO_PIN, HIGH);         
    return (duration * 0.343 / 2);                  // Distance = time(s) x speed of sound (m/s) / 2 (round trip is both ways and converting to mm)
}
/*
 * Method to move the sonar motor to a determined position using a PWM value (in us)
 * @param pwmWidth - Takes an integer value containing the PWM duration (in us) for pin to be held high
 */
void Ultrasonic_Module::setSG90Position(int pwmWidth) {
    const uint8_t TOTAL_PWM_LOOPS = 5;              // Ideal number of loops seems to be 5 to guarantee full stator rotation
    const uint16_t PWM_PERIOD = 20000;              // Period for the PWM functionality in us

    for (uint8_t currentPWMLoop = 0; currentPWMLoop < TOTAL_PWM_LOOPS; currentPWMLoop++) {
        digitalWrite(PWM_PIN, HIGH);
        delayMicroseconds(pwmWidth);
        digitalWrite(PWM_PIN, LOW);
        delayMicroseconds(PWM_PERIOD-pwmWidth);   
    }
}
/*
 * Method calculate the number of steps to take to reach the furthest 'safe' position using trig functions pre calculated for several angles / distances
 * @param sonar_reading - Integer containing distance (in mm) read from sonar unit
 * @param trig_index - Integer value containing the index of the angle being examined
 * @return - Integer containing the number of steps from current robot position to this position, or 0 if it is 'unsafe'
 */
int Ultrasonic_Module::calculateSteps(int sonar_reading, int trig_index) {
    const uint8_t SG90_TRIG_HEIGHT = 48;                                                        // Height to sensor pin junction in mm
    const int STEPS_TOTAL[5] = {1224, 1728, 2052, 2520, 3330};                                  // Number of steps to reach determined distance
    const uint8_t SG90_HYP_OFFSETS[10] = {59, 77, 88, 105, 105, 120, 130, 150, 175, 195};       // List of offset values - [nI] = low, [(nI)+1] = high

    if (sonar_reading > SG90_HYP_OFFSETS[2*trig_index] && sonar_reading < SG90_HYP_OFFSETS[(2*trig_index)+1])
        return STEPS_TOTAL[trig_index];
    else
        return 0;
}
/*
 * Method to determine whether the robot is within a predetermined proximity range of an object
 * @return - Boolean value FALSE if we are within proximity of an object, TRUE if not
 */
bool Ultrasonic_Module::noCollisionImminent() {
    const uint8_t SONAR_PROXIMITY = 50;                                         // Range in mm set to be max proximity distance for a collision
    return (Ultrasonic_Module::getSonarValue() > SONAR_PROXIMITY);
}
/*
 * Method to return the number of steps required to reach the furthest 'safe' distance based on trig calculations
 * @return - Integer value containing the number of steps
 */
int Ultrasonic_Module::SG90TrigModule() {
    const int SG90_TRIG_PWM_VALUES[6] = {1400, 1600, 1650, 1700, 1750, 1950};   // PWM values(us) as X(degrees) {X : 45, 60, 65, 70, 75, 90} 
    int steps_total = 0;
    for (int i = 0; i < 5; i++) {
        setSG90Position(SG90_TRIG_PWM_VALUES[i]);
        int current_steps_value = Ultrasonic_Module::calculateSteps(Ultrasonic_Module::getSonarValue(), i);
        if (!current_steps_value)
            break;
        steps_total = current_steps_value;
    }    
    setSG90Position(SG90_TRIG_PWM_VALUES[6]);                     // Reset to default servo position
    return steps_total;                                           // Return the number of steps to move before a recalculation is needed 
}
