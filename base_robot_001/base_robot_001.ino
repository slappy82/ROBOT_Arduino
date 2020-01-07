// Campbell Maxwell
// September 2019 - Present

  /* Untidy notes: need specsheet to get 1 step = x distance (we can get angle per step and we know radius of wheel (35mm) so opp/35mm = tan(step angle)
   * so opp (distance per step) = 35mm * tan(5.6/64 = 0.08789) = 35*tan(0.08789) = 35*0.00153398 = 0.05369 = 0.054mm per step
   * 1/0.054 = 18.5 steps = 1mm so going to use base movement as 2mm = 37 steps
   */

  // SONAR
  // Pin setup for TRIG
  uint8_t trig = 3;   
  // Pin setup for ECHO 
  uint8_t echo = 2;  
  // STEPPER MOTORS
  // Pin setup for LEFT motor
  uint8_t leftMotor[4] = {22, 23, 24, 25};
  // Pin setup for RIGHT motor
  uint8_t rightMotor[4] = {26, 27, 28, 29};
  // HM-10 BLE MODULE
  uint8_t state = 20;                   // Return value to determine state of HM-10 eg: if there are bytes to be read
  // Global variables used for motor
  uint8_t stepCurrent = 0;              // Starting step for stepper motor position
  uint8_t stepTotal = 0;                // Tracks the number of steps used so far in a routine

  // Constant variables
  const uint8_t STEP_CYCLE_2_6 = 4;     // Amount of steps in each stepperMove loop using 2 and 6 as forward and reverse (eg every second step)
  const uint8_t STEP_CYCLE_3_5 = 8;     // Amount of steps using 3 and 5 (eg every 3rd step - needs longer delay)
  const uint8_t BASE_MOVEMENT = 37;     // 2mm worth of steps to use as a base movement unit
  const uint8_t REVERSE_VAL = 6;        // Int value to start driving motor forward
  const uint8_t FORWARD_VAL = 2;        // Int value to drive motors in reverse
  const uint8_t SONAR_PROXIMITY = 50;   // Sonar detection range in mm
  const uint8_t MS_DELAY = 2;           // Amount of delay (in milliseconds) between motor steps
  const uint16_t US_DELAY = 2000;       // Amount of delay (in microseconds)
  const uint16_t TURN_90_STEPS = 500;   /* Amount of steps for 90 degree turn in stepperMove loop (for actual value need length of 45* of circle with 
                                           radius = distance between wheels / 2, then just number of steps for that distance*/

  #define console Serial     // Used to communicate with the arduino console
  #define btDevice Serial1   // Used to communicate with the HM-10 device

/////////////////////////////////////////////////////////
// SETUP METHODS
/////////////////////////////////////////////////////////
/*
 * Setup of SONAR module
 */
void robotSetup(void) {
  for (int i = 0; i < 4; i++) {
    pinMode(leftMotor[i], OUTPUT);
    pinMode(rightMotor[i], OUTPUT);
  }
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(state, INPUT);  // Only needed if I wish to use an interrupt
}
/*
 * Setup the HM-10 module to peripheral state to begin advertisement
 */
void btSetup() {
    // Initialize and set the baud rate for data transfer via UART
    btDevice.begin(9600);
    console.begin(9600);
    
    // Sends code to test bluetooth (also disconnect it from a device), if it works it should reply 'OK' 
    btDevice.print("AT");  
    console.println(btDevice.readString());   // receiving and printing the return code to console
    delay(2);
    // Return the MAC address of the adapter
    btDevice.print("AT+ADDR?"); 
    console.println(btDevice.readString());   
    delay(2); 
    // Reset the module to defaults
    btDevice.print("AT+NOTI1"); 
    console.println(btDevice.readString());   
    delay(2); 
    // Set the role of the module - peripheral
    btDevice.print("AT+ROLE0"); 
    console.println(btDevice.readString());   
    delay(2);  
    // Set the mode of the module - restrict commands until it is connected ONLY NEEDED FOR CENTRAL ROLE
    btDevice.print("AT+IMME0"); 
    console.println(btDevice.readString());   
    delay(2); 
    // Reset module to allow for changes to occur
    btDevice.print("AT+RESET"); 
    console.println(btDevice.readString());   
    delay(20);  
}  
/////////////////////////////////////////////////////////
// ROUTINE METHODS
/////////////////////////////////////////////////////////
/*
 * Base AI routine to loop
 */
void baseAI() {
  while (!readSonar() && !checkBLEConnected()){
    while (stepTotal < BASE_MOVEMENT){
      stepperForward(FORWARD_VAL);   // speed, amount of steps (used for forwards or reverse)
      stepTotal++;
    }
    stepTotal = 0;
  }
  stepperLeft90();
  while (readSonar()){
    stepperRight90(); 
  }
}
/*
 * Take an int value for left and right motor to determine direction
 */
void stepperMove(uint8_t left, uint8_t right, const uint16_t STEP_TOTAL){
  uint8_t lMotorStep = 0; uint8_t rMotorStep = 0; int stepCount = 0;
  while (stepCount < STEP_TOTAL){                                                
    stepperMoveSheet(leftMotor, lMotorStep);   
    stepperMoveSheet(rightMotor, rMotorStep);    
    lMotorStep = (lMotorStep + left) % 8;
    rMotorStep = (rMotorStep + right) % 8;
    delay(MS_DELAY);
    stepCount++;
  }
}
/*
 * This method can be used for FORWARD OR REVERSE 
 */
void stepperForward(uint8_t stepDirection){
  stepperMoveSheet(leftMotor, stepCurrent);   
  stepperMoveSheet(rightMotor, stepCurrent);    
  stepCurrent = (stepCurrent + stepDirection) % 8;
  delay(MS_DELAY);
}
/*
 * Turn the robot to the left by 90 degrees using the stepper motors
 */
void stepperLeft90(){
  stepperMove(REVERSE_VAL, FORWARD_VAL, TURN_90_STEPS);
}
/*
 * Turn the robot to the right by 90 degrees using the stepper motors
 */
void stepperRight90(){
  stepperMove(FORWARD_VAL, REVERSE_VAL, TURN_90_STEPS);
}
/*
 * Checks for new input to HM-10 then if it is a new connection and hands over to remote method if it is
 */
bool checkBLEConnected() {
   uint8_t msgLength = btDevice.available();
   if (msgLength > 2) {
      String btStatus = btDevice.readString();
      if (btStatus.equals("OK+CONN")) {
          console.println(btStatus);
          while(btRemoteControl());  // looping until remote toggle or hard disconnect    
      }
   } 
   else if (msgLength == 2) {
      byte b[msgLength];
      btDevice.readBytes(b, msgLength);
      if (b[0] == 1 && b[1] == 1) {      
          while(btRemoteControl());  // looping until remote toggle or hard disconnect            
      }
   }
   return false;
}
/*
 * Listens for input from android app then handles it accordingly, ideally making a movement call or returning to AI method
 */
bool btRemoteControl() {  
    // Listen and obtain new input via HM-10 
    uint8_t messageLength; 
    while(delayMicroseconds(10), messageLength = btDevice.available(), messageLength < 2);
    byte newBytes[messageLength];
    btDevice.readBytes(newBytes, messageLength);
    
    // Check input for necessary flags then either send to stepperMove or return to baseAI
    if (messageLength == 2) {
        if (newBytes[0] == 0 && newBytes[1] == 0) {       // Switch toggled to enable AI mode
            return false;                                 // Return to baseAI
        }
        else if (newBytes[0] == 4 && newBytes[1] == 4) {  // Code returned indicating keypress ending to stop movement loop
            return true;                                  // Return to checkBLEConnected loop for recall for next movement
        }
        else {                                            // Pass values to stepperMove for movement
            messageLength = 0;
            while (messageLength < 2) {
                 stepperMove(newBytes[0], newBytes[1], STEP_CYCLE_2_6);  
                 messageLength = btDevice.available();
            }
        }
    }
    else {
        String AT = btDevice.readString();
        console.println(AT);                // Handle String need check for connection, maybe just for "OK+LOST"
        if (AT.equals("OK+LOST")) {         // Any cleaner way to check connection???
            return false; 
        }  
    }
    return true;   // Return to checkBLEConnected loop for recall for next movement
}
/* 
 *  Read from SONAR and return TRUE if there is something in proximity to robot (eg: 50mm)
 */
boolean readSonar(void){
    digitalWrite(trig, LOW);             // Clears the trig pin
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);            // Setting trig pin high for 10us sends the ultrasonic signal which is received to determine distance
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    int distance, duration;
    duration = pulseIn(echo, HIGH);      // Receive the duration between sending and receiving the ultrasonic signal
    distance = (duration * 0.34 / 2);    // Distance = time(s) x speed of sound (m/s) / 2 (round trip is both ways)
    bool turn = (distance < SONAR_PROXIMITY) ? true : false;
    return turn;
}
/*
 * Method to control state changes of pins connected to stepper. Each case is a new step configuration
 */
void stepperMoveSheet(uint8_t in[], int inStep){
  switch(inStep){
    case 0:
      digitalWrite(in[3], HIGH);  // 4
      digitalWrite(in[2], LOW);   // 3
      digitalWrite(in[1], LOW);   // 2
      digitalWrite(in[0], LOW);   // 1
      break;
    case 1:
      digitalWrite(in[3], HIGH);
      digitalWrite(in[2], HIGH);
      digitalWrite(in[1], LOW);
      digitalWrite(in[0], LOW);
      break;
    case 2:
      digitalWrite(in[3], LOW);
      digitalWrite(in[2], HIGH);
      digitalWrite(in[1], LOW);
      digitalWrite(in[0], LOW);
      break;
    case 3:
      digitalWrite(in[3], LOW);
      digitalWrite(in[2], HIGH);
      digitalWrite(in[1], HIGH);
      digitalWrite(in[0], LOW);
      break;
    case 4:
      digitalWrite(in[3], LOW);
      digitalWrite(in[2], LOW);
      digitalWrite(in[1], HIGH);
      digitalWrite(in[0], LOW);
      break;
    case 5:
      digitalWrite(in[3], LOW);
      digitalWrite(in[2], LOW);
      digitalWrite(in[1], HIGH);
      digitalWrite(in[0], HIGH);
      break;
    case 6:
      digitalWrite(in[3], LOW);
      digitalWrite(in[2], LOW);
      digitalWrite(in[1], LOW);
      digitalWrite(in[0], HIGH);
      break;
    case 7:
      digitalWrite(in[3], HIGH);
      digitalWrite(in[2], LOW);
      digitalWrite(in[1], LOW);
      digitalWrite(in[0], HIGH);
      break;
    default:
      digitalWrite(in[3], LOW);
      digitalWrite(in[2], LOW);
      digitalWrite(in[1], LOW);
      digitalWrite(in[0], LOW);
      break;
  }
}
/////////////////////////////////////////////////////////
// SETUP
/////////////////////////////////////////////////////////
void setup() {
  robotSetup();
  btSetup();
}
/////////////////////////////////////////////////////////
// ROUTINE
/////////////////////////////////////////////////////////
void loop() {
  baseAI();
}
