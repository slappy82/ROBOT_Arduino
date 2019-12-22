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
  uint8_t go = 0;             // starting step for stepper motor position
  uint8_t steps;              // tracks the number of steps in a routine
  
  const uint8_t MM_STEP = 20; // One mm worth of steps
  const uint8_t REVERSE_VAL = 5;  // Int value to start driving motor forward
  const uint8_t FORWARD_VAL = 3;  // Int value to drive motors in reverse
  
  boolean test = true;

void robotSetup(void) {
  for (int i = 0; i < 4; i++) {
  pinMode(leftMotor[i], OUTPUT);
  pinMode(rightMotor[i], OUTPUT);
  }
  steps = 0;
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
}

void setup() {
  robotSetup();
}

void loop() {
  //  need specsheet to get 1 step = x distance (we can get angle per step and we know radius of wheel (35mm) so opp/35mm = tan(step angle)
  // so opp (distance per step) = 35mm * tan(step)  -- should be ~20 steps = 1mm if data correct - 5.6/64
  while (!readSonar()){
    while (steps < MM_STEP){
      stepperSyncMove(3, FORWARD_VAL);   // speed, amount of steps (used for forwards or reverse)
      steps++;
    }
    steps = 0;
  }
  stepperLeft90();
  while (readSonar()){
    stepperRight90(); 
  }
}
  // This method can be used for forward and reverse - steplength 3 for forward, steplength 5 for reverse
void stepperSyncMove(uint8_t _delay, uint8_t steplength){
  stepperMoveSheet(leftMotor, go);   
  stepperMoveSheet(rightMotor, go);    
  go = (go + steplength) % 8;
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
