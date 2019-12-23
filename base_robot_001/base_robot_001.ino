// Campbell Maxwell
// 2019

  // SONAR
  // Pin setup for TRIG
  uint8_t trig = 5;   
  // Pin setup for ECHO 
  uint8_t echo = 3;  
  // STEPPER MOTORS
  // Pin setup for LEFT motor
  uint8_t leftMotor[4] = {13, 12, 11, 10};
  // Pin setup for RIGHT motor
  uint8_t rightMotor[4] = {9, 8, 7, 6};
  // Global variables used for motor
  uint8_t stepCurrent = 0;              // Starting step for stepper motor position
  uint8_t stepTotal = 0;                // Tracks the number of steps used so far in a routine
  uint8_t msDelay = 3;                  // Amount of delay (in milliseconds) between motor steps
  uint16_t usDelay = 2500;              // Amount of delay (in microseconds)
  
  const uint8_t BASE_MOVEMENT = 37;     // 2mm worth of steps to use as a base movement unit
  const uint8_t REVERSE_VAL = 5;        // Int value to start driving motor forward
  const uint8_t FORWARD_VAL = 3;        // Int value to drive motors in reverse
  
  boolean test = true;

void robotSetup(void) {
  for (int i = 0; i < 4; i++) {
    pinMode(leftMotor[i], OUTPUT);
    pinMode(rightMotor[i], OUTPUT);
  }
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
}

void setup() {
  robotSetup();
}

void loop() {
  //  need specsheet to get 1 step = x distance (we can get angle per step and we know radius of wheel (35mm) so opp/35mm = tan(step angle)
  // so opp (distance per step) = 35mm * tan(5.6/64 = 0.08789) = 35*tan(0.08789) = 35*0.00153398 = 0.05369 = 0.054mm per step
  // 1/0.054 = 18.5 steps = 1mm so going to use base movement as 2mm = 37 steps
  while (!readSonar()){
    while (stepTotal < BASE_MOVEMENT){
      stepperSyncMove(msDelay, FORWARD_VAL);   // speed, amount of steps (used for forwards or reverse)
      stepTotal++;
    }
    stepTotal = 0;
  }
  stepperLeft90();
  while (readSonar()){
    stepperRight90(); 
  }
}
  // This method can be used for forward and reverse - steplength 3 for forward, steplength 5 for reverse
void stepperSyncMove(uint8_t _delay, uint8_t steplength){
  stepperMoveSheet(leftMotor, stepCurrent);   
  stepperMoveSheet(rightMotor, stepCurrent);    
  stepCurrent = (stepCurrent + steplength) % 8;
  delay(_delay);
}
  // Take an int value for left and right motor to determine direction
  // TODO: Look at refactoring syncmove method here, less code duplication etc
void stepperMove(uint8_t left, uint8_t right){
  uint8_t lMotorStep = 0; uint8_t rMotorStep = 0; int stepCount = 0;
  while (stepCount < 500){
    stepperMoveSheet(leftMotor, lMotorStep);   
    stepperMoveSheet(rightMotor, rMotorStep);    
    lMotorStep = (lMotorStep + left) % 8;
    rMotorStep = (rMotorStep + right) % 8;
    delay(3);
    stepCount++;
  }
}
  // Turn the robot to the left by 90 degrees using the stepper motors
void stepperLeft90(){
  stepperMove(REVERSE_VAL, FORWARD_VAL);
}
  // Turn the robot to the right by 90 degrees using the stepper motors
void stepperRight90(){
  stepperMove(FORWARD_VAL, REVERSE_VAL);
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
  if (distance < 50){
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