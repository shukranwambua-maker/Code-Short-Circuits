#include <ESP32Servo.h>
#include "BluetoothSerial.h"



BluetoothSerial SerialBT;
Servo my_servo;

int servo_pin = 5;

int motorPin1 = 22 ;
int motorPin2= 23 ;
int enable_pin = 15;//
int freq = 5000;
int res = 8;


char receivedChar;    // Variable to receive data from the app

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Motor_Control26");
  Serial.println("Device is ready for pairing");
  Serial.println("Waiting for commands...");

  //PWM control
  ledcAttach(enable_pin, freq, res);
  
  // Set motor pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  //servo pin
  
  // Set motor pins as outputs
  my_servo.attach(servo_pin);
  my_servo.write(45);




}

void loop() {
  // put your main code here, to run repeatedly:
   // Forward data from Serial to Bluetooth
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  // Process Bluetooth commands
  if (SerialBT.available()) {
    receivedChar = SerialBT.read();  // Receive data from phone
    Serial.print("Received command: ");
    Serial.println(receivedChar);
    
    // Process the command
    processCommand(receivedChar);
  }

}
void processCommand(char command) {
  switch(command) {
    case 'W':  // Motor Forward
      Serial.println("ACTION: Motor Forward");
      ledcWrite(enable_pin, 255);
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      break;

    case 'S':  // Motor stop
      Serial.println("ACTION: Motor stop");
      //ledcWrite(enable_pin, 255);
      digitalWrite(motorPin1, LOW);
      //digitalWrite(motorPin2, LOW);
      break;
      
      /*
        case 'B':  // Motor Backward
      Serial.println("ACTION: Motor Backward");
      ledcWrite(enable_pin, 240);
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      break;
      */
  
      
    case 'L':  // Motor Stop
      Serial.println("tURN left!!!");
      my_servo.write(90);

      break;
      
    case 'R':  // Report status
    Serial.println("tURN rIGHT!!!");
     my_servo.write(0);

      break;
      
    default:
      Serial.print("UNKNOWN COMMAND: ");
      Serial.println(command);
      break;
  }
}

