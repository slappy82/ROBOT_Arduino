/*
 *      Campbell Maxwell
 *      February 2020 - Present
 *      
 *      Calculation for Angle = (PWM Width - 500) / x
 *      
 *     PWM Width   Exact Angle    Value for X     θ using Average      θ Differential      θ Smooth Average    Smooth Differential
 *      500us        = 0          = 0              = 0                      0
 *      675us        = 16         = 10.938         = 15.4                  -0.6
 *      850us        = 34         = 10.294         = 30.8                  -3.2
 *      1025us       = 48         = 10.938         = 46.2                  -1.8
 *      1200us       = 65         = 10.769         = 61.6                  -3.4
 *      ***SMOOTH ZONE BELOW**********************************************************************************************************************
 *      +++ADJUSTED ZONE BELOW++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *      1375us       = 75         = 11.667         = 77.0                  +2.0              = 74.7              -0.3
 *      1550us       = 90         = 11.667         = 92.4                  +2.4              = 89.6              -0.4
 *      1725us       = 104        = 11.779         = 107.8                 +3.8              = 104.5             +0.5
 *      1900us       = 120        = 11.667         = 123.2                 +3.2              = 119.5             -0.5  (Lowest value)
 *      2075us       = 133        = 11.842         = 138.6                 +5.6              = 134.4             +1.4  (Highest value)
 *      2250us       = 148        = 11.824         = 154.0                 +6.0              = 149.3             +1.3
 *      +++ADJUSTED ZONE ABOVE++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *      2425us       = 166        = 11.596         = 169.4                 +3.4              = 164.2             -1.8
 *      ***SMOOTH ZONE ABOVE**********************************************************************************************************************
 *      2600us       = 185        = 11.351         = 184.8                 -0.2              
 *                        Average = 11.361
 *                 Smooth Average = 11.72
 *               Adjusted Average = 11.76   (both highest and lowest value are within 1* of actual angle)
 *                                     
 *      Adjusted smooth range 175us = 15* +/- <1*      
 *      Using 1300us - 2300us as my active range for front sonar, 15* intervals to check range using the calculation θ = (PWM Width - 500) / 11.76
 *      Will be able to refactor this when I have it mounted and know the exact height, desired starting angle, and any offsets.
 *      Also considering a non-linear approach to angular intervals, as approaching 90* each degree will cover an exponentially increasing horizontal 
 *      range, so increased accuracy will be more important approaching 90*.
 *      
 *      1980 = 90*
 *      
 *      900 = 0*
 */

//using namespace std;

const float ADJUSTED_DIVISOR = 11.76f;          // Adjusted average value found from the above table
const uint8_t PWM_PIN = 5;                      // PWM output pin to servo
const uint8_t TOTAL_LOOPS = 5;                  // Ideal number of loops seems to be 5 to guarantee full stator rotation
const uint16_t PWM_PERIOD = 20000;              // Period for the PWM functionality
uint16_t pwmWidth;                              // Max is 2700, min is 300  going to use safe range as 500 - 2550 for exactly 180* 
uint8_t loops;
int val;

void setup() {
    pinMode(PWM_PIN, OUTPUT);
    loops = 0;
    val = 0;
    pwmWidth = 1500;
    Serial.begin(9600);
    Serial.println("Program Starting...");
}

void loop() {
    while (loops < TOTAL_LOOPS) {
        digitalWrite(PWM_PIN, HIGH);
        delayMicroseconds(pwmWidth);
        digitalWrite(PWM_PIN, LOW);
        delayMicroseconds(PWM_PERIOD-pwmWidth);
        loops++;
    }
    loops = 0;
    uint8_t len;
    while (len = Serial.available(), len > 0) {
        byte b = Serial.read();
        if (b > 47 && b < 58) {
            val *= 10;
            val += (int)b-48;           // ASCII offset (each byte is a char from console rather than a full int or string so 0-9 is 48-57)
        }
        else if (b == 10) {
            // Going to use range 1300us - 2300us eventually, for now 500 = ~0* -> 2600 = ~180*
            float angle = (val-500)/ ADJUSTED_DIVISOR;
            String fullMessage = "Moving to position " + (String)angle + " degrees using PWM Width value of " + val + "us";
            Serial.println(fullMessage);
            pwmWidth = val;
            val = 0;
        }
    }
}
