// Campbell Maxwell
// 2019

  //  need specsheet to get 1 step = x distance (we can get angle per step and we know radius of wheel (35mm) so opp/35mm = tan(step angle)
  // so opp (distance per step) = 35mm * tan(5.6/64 = 0.08789) = 35*tan(0.08789) = 35*0.00153398 = 0.05369 = 0.054mm per step
  // 1/0.054 = 18.5 steps = 1mm so going to use base movement as 2mm = 37 steps

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
  uint8_t msDelay = 2;                  // Amount of delay (in milliseconds) between motor steps
  uint16_t usDelay = 3000;              // Amount of delay (in microseconds)
  
  const uint8_t BASE_MOVEMENT = 37;     // 2mm worth of steps to use as a base movement unit
  const uint8_t REVERSE_VAL = 6;        // Int value to start driving motor forward
  const uint8_t FORWARD_VAL = 2;        // Int value to drive motors in reverse
  const uint8_t SONAR_PROXIMITY = 50;   // Sonar detection range in mm

  #define console Serial     // Used to communicate with the arduino console
  #define btDevice Serial1   // Used to communicate with the HM-10 device

/////////////////////////////////////////////////////////
// SETUP METHODS
/////////////////////////////////////////////////////////
void robotSetup(void) {
  for (int i = 0; i < 4; i++) {
    pinMode(leftMotor[i], OUTPUT);
    pinMode(rightMotor[i], OUTPUT);
  }
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(state, INPUT);
}
// Setup the HM-10 module to peripheral state to begin advertisement
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
    // Set the role of the module (slave or master)
    btDevice.print("AT+ROLE0"); 
    console.println(btDevice.readString());   
    delay(2);  
    // Set the mode of the module - restrict commands until it is connected
    btDevice.print("AT+IMME0"); 
    console.println(btDevice.readString());   
    delay(2); 
    // SReset module to allow for changes to occur
    btDevice.print("AT+RESET"); 
    console.println(btDevice.readString());   
    delay(20);  
}  
/////////////////////////////////////////////////////////
// SETUP
/////////////////////////////////////////////////////////
void setup() {
  robotSetup();
  btSetup();
}
/////////////////////////////////////////////////////////
// CODE
/////////////////////////////////////////////////////////
void loop() {
  baseAI();
}
/////////////////////////////////////////////////////////
// CODE METHODS
/////////////////////////////////////////////////////////
  // Base AI routine to loop
void baseAI() {
  while (!readSonar() && !checkBLEConnected()){
    while (stepTotal < BASE_MOVEMENT){
      stepperForward(msDelay, FORWARD_VAL);   // speed, amount of steps (used for forwards or reverse)
      stepTotal++;
    }
    stepTotal = 0;
  }
  stepperLeft90();
  while (readSonar()){
    stepperRight90(); 
  }
}

  // Take an int value for left and right motor to determine direction
void stepperMove(uint8_t left, uint8_t right, uint16_t STEP_TOTAL){
  uint8_t lMotorStep = 0; uint8_t rMotorStep = 0; int stepCount = 0;
  while (stepCount < STEP_TOTAL){                                                // BREAK THIS OUT TO TURN METHODS
    stepperMoveSheet(leftMotor, lMotorStep);   
    stepperMoveSheet(rightMotor, rMotorStep);    
    lMotorStep = (lMotorStep + left) % 8;
    rMotorStep = (rMotorStep + right) % 8;
    delay(msDelay);
    stepCount++;
  }
}
  // This method can be used for FORWARD OR REVERSE - 3 or 5 maybe sub for 2 or 6 for smoother + reduced delay???
void stepperForward(uint8_t stepDelay, uint8_t stepDirection){
  stepperMoveSheet(leftMotor, stepCurrent);   
  stepperMoveSheet(rightMotor, stepCurrent);    
  stepCurrent = (stepCurrent + stepDirection) % 8;
  delay(stepDelay);
}
  // Turn the robot to the left by 90 degrees using the stepper motors
void stepperLeft90(){
  stepperMove(REVERSE_VAL, FORWARD_VAL, 500);
}
  // Turn the robot to the right by 90 degrees using the stepper motors
void stepperRight90(){
  stepperMove(FORWARD_VAL, REVERSE_VAL, 500);
}
bool checkBLEConnected() {
   uint8_t len = btDevice.available();
   if (len > 2) {
      String btStatus = btDevice.readString();
      if (btStatus.equals("OK+CONN")) {
        btRemoteControl();
      }
   } 
   else if (len == 2) {
      byte b[2];
      btDevice.readBytes(b, 2);
      if (b[0] == 1 && b[1] == 1) {        
        btRemoteControl();
      }
   }
  return false;
}

// TODO: send bytes to stepperMove and determine a loop size to feel natural vs the button press duration
void btRemoteControl() {  
    // Listen for new input  
    uint8_t messageLength = btDevice.available();
    while(messageLength < 2) {   // maybe break out to new method and use pointer for return?
      delayMicroseconds(10);
      messageLength = btDevice.available();
    }
    byte newBytes[messageLength];
    btDevice.readBytes(newBytes, messageLength);
    // Check input for necessary flags then either send to stepperMove or back to baseAI, finally recursive call this method
    if (messageLength == 2) {
        if (newBytes[0] == 0 && newBytes[1] == 0) {  // Switch toggled to enable AI mode
            baseAI();   // maybe try return??? (return to checkBLEConn method to return to baseAI if possible) ++++++++++++++++++++++
        }
        else if (newBytes[0] == 4 && newBytes[1] == 4) {
            btRemoteControl();
        }
        else {                                      // Pass values to stepperMove for movement
            messageLength = 0;
            while (messageLength < 2) {
                 stepperMove(newBytes[0], newBytes[1], 4);  // CHECK THIS - stepperMove needs to be fixed - 4 cycles until back to original (8 for 3,5)
                 messageLength = btDevice.available();
            }
        }
    }
    else {
        // Handle String / either recall this method or back to baseAI if conn lost NEED CHECK FOR AT COMMAND +++++++++++++++++++++
        baseAI();  // maybe try return??? (return to checkBLEConn method to return to baseAI if possible) +++++++++++++++++++++
    }
    btRemoteControl();
}
  // Read from SONAR and return TRUE if there is something close to robot (eg: 50mm)
boolean readSonar(void){
  boolean turn = false;
     // Clears the trigPin
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  int distance, duration;
  duration = pulseIn(echo, HIGH);
    // distance = time(s) x speed of sound (m/s) / 2 (round trip is both ways)
  distance = (duration * 0.34 / 2);
  if (distance < SONAR_PROXIMITY){
    turn = true;
  }
  return turn;
}

void stepperMoveSheet(uint8_t in[], int _step){
  switch(_step){
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
