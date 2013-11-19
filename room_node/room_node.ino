
#include <Servo.h>

int incomingByte;      // a variable to read incoming serial data into

// Servo Objects
Servo airControl;

// Airflow through register in percentage
const int AIR_MIN  = 10;
const int AIR_LOW  = 25;
const int AIR_NORM = 50;
const int AIR_HIGH = 75; 
const int AIR_MAX  = 90;

// Controller Commands
const int CMD_AIR_MIN   = '1';
const int CMD_AIR_LOW   = '2';
const int CMD_AIR_NORM  = '3';
const int CMD_AIR_HIGH  = '4';
const int CMD_AIR_MAX   = '5';
const int CMD_MODE_HEAT = '6';
const int CMD_MODE_COOL = '7';
const int CMD_MODE_OFF  = '8';
const int CMD_TEMP_UP   = 'A';
const int CMD_TEMP_DOWN = 'B';
const int CMD_HEARTBEAT = 'H';

// HVAC Modes
const int OFF  = 0;
const int HEAT = 1;
const int COOL = 2;

// LED 
const int activityLed = 7;
const int coolLed  = 4;
const int heatLed  = 5;

const int servoPin = 12;

int air;
int mode;
int heartbeats = 0;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(heatLed, OUTPUT);
  pinMode(coolLed, OUTPUT);
  
  // initialize the servo
  airControl.attach(9);
}

void loop() {
  // see if there's incoming serial data:
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();

    switch (incomingByte) {

      // Adjust Air Flow
      case CMD_AIR_MIN:
        airControl.write(AIR_MIN);
        air = AIR_MIN;
      break;

      case CMD_AIR_LOW:
        airControl.write(AIR_LOW);
        air = AIR_LOW;
      break;

      case CMD_AIR_NORM:
        airControl.write(AIR_NORM);
        air = AIR_NORM;
      break;

      case CMD_AIR_HIGH:
        airControl.write(AIR_HIGH);
        air = AIR_HIGH;
      break;

      case CMD_AIR_MAX:
        airControl.write(AIR_MAX);
        air = AIR_MAX;
      break;

      case CMD_MODE_HEAT:
        mode = HEAT;
        digitalWrite(coolLed, LOW);
        digitalWrite(heatLed, HIGH);
      break;
      
      case CMD_MODE_COOL:
        mode = COOL;
        digitalWrite(coolLed, HIGH);
        digitalWrite(heatLed, LOW);
      break;
      
      case CMD_MODE_OFF:
        mode = OFF;
        digitalWrite(coolLed, LOW);
        digitalWrite(heatLed, LOW);
      break;
      
      case CMD_HEARTBEAT:
        heartbeats++;
        digitalWrite(activityLed, HIGH);
        delay(100);
        digitalWrite(activityLed, LOW);
      break;
      
      default:
      break;
    }

  }

  if (heartbeats > 200)
    heartbeats = 0;
  else if (heartbeats > 125)
    Serial.write(CMD_TEMP_DOWN);
  else if (heartbeats > 50)
    Serial.write(CMD_TEMP_UP);
  
    
 
}


