////////////////////////////////////////////////////////////////////////////
/// 12 VDC JGA25-370-35.5K Encoded DC Motor                             ///
/// Encoder on motor shaft | 11 PPR | Gear Ratio 1:35.5                 ///
/// Speed control via 10K potentiometer | L298N driver                  ///
/// Date: 25/04/2026                                                     ///
////////////////////////////////////////////////////////////////////////////

// ---------------------Encoder------------------------------------
#define ENC_COUNT_REV  11      // Pulses per rev (motor shaft)
#define GEAR_RATIO     35.5    // JGA25-370-35.5K gearbox ratio

//----------------------Pin Definitions----------------------------
#define ENC_IN   3             // Encoder signal → interrupt-capable pin
#define EN_Pin   10            // L298N ENA (PWM speed)
#define DIR_F    8             // L298N IN3 (forward)
#define DIR_B    9             // L298N IN4 (backward)
#define POT_PIN  A0            // 10K potentiometer

//Timing 
const unsigned long INTERVAL = 1000;   // RPM calculation interval (ms)
unsigned long previousMillis = 0;

//Encoder (volatile — modified inside ISR) 
volatile long encoderValue = 0;

//Variables
int   motorPwm      = 0;
float motorShaftRpm = 0.0;
float outputRpm     = 0.0;


void setup() {
  Serial.begin(9600);

  // Encoder pin
  pinMode(ENC_IN, INPUT_PULLUP);

  // Mega to L298N pins
  pinMode(EN_Pin, OUTPUT);
  pinMode(DIR_F,  OUTPUT);
  pinMode(DIR_B,  OUTPUT);

  // Set direction: forward
  digitalWrite(DIR_F, HIGH);
  digitalWrite(DIR_B, LOW);

  // Attach encoder interrupt on rising edge
  attachInterrupt(digitalPinToInterrupt(ENC_IN), updateEncoder, RISING);

  previousMillis = millis();

  // Header
  Serial.println(F("============================================"));
  Serial.println(F("  JGA25-370-35.5K Motor Speed Controller   "));
  Serial.println(F("  Encoder: 11 PPR | Gear Ratio: 1:35.5     "));
  Serial.println(F("============================================"));
  Serial.println(F("PWM\t\tMOTOR RPM\tOUTPUT RPM"));
  Serial.println(F("--------------------------------------------"));
}


void loop() {

  //Read potentiometer and set motor speed 
  motorPwm = map(analogRead(POT_PIN), 0, 1023, 0, 255);
  analogWrite(EN_Pin, motorPwm);

  //Calculate RPM every second 
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= INTERVAL) {  //determine how many pulses recorded after 1 second (1000 mS)
    previousMillis = currentMillis;

    // Atomically snapshot and reset encoder count
    noInterrupts();
    long pulses = encoderValue;
    encoderValue = 0;
    interrupts();

    //RPM Calculations
    // Motor shaft RPM  = (pulses per second ÷ PPR) × 60
    motorShaftRpm = ((float)pulses / ENC_COUNT_REV) * 60.0;
    // Output shaft RPM = Motor shaft RPM ÷ gear ratio
    outputRpm     = motorShaftRpm / GEAR_RATIO;

    //Serial Output
    if (motorPwm > 0 || outputRpm > 0) {
      Serial.print(F("PWM: "));
      Serial.print(motorPwm);
      Serial.print(F(" ("));
      Serial.print(map(motorPwm, 0, 255, 0, 100));
      Serial.print(F("%)\t"));

      Serial.print(F("Motor: "));
      Serial.print(motorShaftRpm, 1);
      Serial.print(F(" RPM\t"));

      Serial.print(F("Output: "));
      Serial.print(outputRpm, 1);
      Serial.println(F(" RPM"));
    } else {
      Serial.println(F("Motor stopped."));
    }
  }
}


// ISR for Encoder pulses
void updateEncoder() {
  encoderValue++;
}