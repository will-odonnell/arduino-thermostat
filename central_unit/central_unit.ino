
// Temperature
const int RAMP_UP   = 1;
const int RAMP_DOWN = 0;
int ramp = RAMP_UP;

const int MAX_TEMP = 95;
const int MIN_TEMP = 50;
const int MIN_TRG_TEMP = 60;
const int MAX_TRG_TEMP = 80;
int temp = MIN_TEMP;

// HVAC Modes
const int OFF  = 0;
const int HEAT = 1;
const int COOL = 2;
int hvacMode = HEAT;

int airflow;
int curAir;    // Holds current airflow setting

// Airflow through register in percentage
const int AIR_MIN  = 10;
const int AIR_LOW  = 25;
const int AIR_NORM = 50;
const int AIR_HIGH = 75; 
const int AIR_MAX  = 90;

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

// LED 
const int activityLed = 3;
const int heatLed  = 4;
const int coolLed  = 5;

void setup()
{
  // initialize serial output for Zigbee
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(100);
  Serial1.write(0x11);   //backlight on
  Serial1.write(0x16);  //no cursor

  pinMode(activityLed, OUTPUT);
  pinMode(heatLed, OUTPUT);
  pinMode(coolLed, OUTPUT);
}

static int currentTemp = MIN_TEMP;
int getTemp ()
{
  if (ramp == RAMP_UP) {
    if (currentTemp >= MAX_TEMP) {
      currentTemp = MAX_TEMP;
      ramp = RAMP_DOWN;
    } else {
      currentTemp += 2;  
    }
  } else {
    if (currentTemp <= MIN_TEMP) {
      currentTemp = MIN_TEMP;
      ramp = RAMP_UP;
    } else {
      currentTemp -= 2;  
    }
  }

  return currentTemp;
}

//---------------------------------------
// Temperature Functions
//---------------------------------------
static int targetTemp = MIN_TRG_TEMP;

int getTargetTemp () 
{
  return targetTemp;
}

int setTargetTemp (int cmd)
{
  int trgTemp = getTargetTemp();

  switch (cmd) {
    case CMD_TEMP_UP:
      if (trgTemp >= MAX_TRG_TEMP) 
        trgTemp = MAX_TRG_TEMP;
      else
        trgTemp += 10;

  digitalWrite(activityLed, HIGH);
  delay(100);
  digitalWrite(activityLed, LOW);

    break;
    
    case CMD_TEMP_DOWN:
      if (trgTemp <= MIN_TRG_TEMP) 
        trgTemp = MIN_TRG_TEMP;
      else
        trgTemp -= 10;

  digitalWrite(activityLed, HIGH);
  delay(100);
  digitalWrite(activityLed, LOW);

    break;

    default:
    break;    
  }

  targetTemp = trgTemp;
  return trgTemp;
}

//---------------------------------------
// HVAC Functions
//---------------------------------------
int setHvacMode (int curTemp, int trgTemp) 
{
  int mode = COOL;

  // determine mode
  if (curTemp <= trgTemp) {
    mode = HEAT;
  } else {
    mode = COOL;
  }

  // activate mode signals and to room node
  switch (mode) {
    case HEAT:
      digitalWrite(heatLed, HIGH);
      digitalWrite(coolLed, LOW);
      Serial.write(CMD_MODE_HEAT);    
    break;
    
    case COOL:
      digitalWrite(heatLed, LOW);
      digitalWrite(coolLed, HIGH);
      Serial.write(CMD_MODE_COOL);
    break;
    
    default:
      digitalWrite(heatLed, LOW);
      digitalWrite(coolLed, LOW);
      Serial.write(CMD_MODE_OFF);
    break;
  }
  
  return mode;
}

//---------------------------------------
// Airflow Functions
//---------------------------------------
static int currentAirFlow;

int getAirFlow()
{
  return currentAirFlow;  
}

int setAirFlow(int curTemp, int trgTemp)
{
  int tempDiff = curTemp - trgTemp;
  int newAir = AIR_NORM;
  int curAir = getAirFlow();

  // convert to positive number
  if (tempDiff < 0) 
    tempDiff *= -1;   
    
  // determine register opening size
  if      (tempDiff >= 20)  newAir = AIR_MAX;
  else if (tempDiff >= 15)  newAir = AIR_HIGH;
  else if (tempDiff >= 10)  newAir = AIR_NORM;
  else if (tempDiff >= 5)   newAir = AIR_LOW;
  else                      newAir = AIR_MIN;
    
  // send new register setting if changed
  
  if (newAir != curAir) {
    switch (newAir) {
      case AIR_MIN:  Serial.write(CMD_AIR_MIN);  break;
      case AIR_LOW:  Serial.write(CMD_AIR_LOW);  break;
      case AIR_NORM: Serial.write(CMD_AIR_NORM); break;
      case AIR_HIGH: Serial.write(CMD_AIR_HIGH); break;
      case AIR_MAX:  Serial.write(CMD_AIR_MAX);  break;
      default:       Serial.write(CMD_AIR_NORM); break;
    }  
    curAir = newAir;
  }
  
  return curAir;  
}

void heartBeat()
{
  // Progress status
  Serial.write(CMD_HEARTBEAT);
//  digitalWrite(activityLed, HIGH);
//  delay(100);
//  digitalWrite(activityLed, LOW);

}

void loop()
{
  heartBeat();

  //--------------------------------------------
  // Read any commands
  //--------------------------------------------
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();  

    switch (incomingByte) {
    
      case CMD_TEMP_UP:
      case CMD_TEMP_DOWN:
        setTargetTemp(incomingByte);
      break;
      
      default:
      break;
      
    }
  }
  
  //--------------------------------------------
  // Collect data
  //--------------------------------------------
  
  // Temperature data
  int curTemp = getTemp(); 
  int trgTemp = getTargetTemp();
  
  // HVAC mode
  int hvacMode = setHvacMode(curTemp, trgTemp);

  // Airflow
  int airFlow = setAirFlow(curTemp, trgTemp);

  //---------------------------------------------
  // Write to the LCD screen
  //---------------------------------------------

  // Write target temperature
  Serial1.write(0x80);    
  Serial1.write("Trg:");
  Serial1.print(trgTemp,DEC);

  // Write current temperature
  Serial1.write(0x94);
  Serial1.write("Cur:");
  Serial1.print(curTemp,DEC);

  // Write airflow percentage
  Serial1.write(0x87);
  Serial1.write("Air:");
  Serial1.print(airFlow,DEC);
  Serial1.write(0x25);

  // Write HVAC mode
  Serial1.write(0x9B);
  Serial1.write("Mode:");
  if (hvacMode == HEAT)
    Serial1.write("heat");
  else if (hvacMode == COOL)
    Serial1.write("cool");
  else
    Serial1.write("n/a");
  
  delay(500);


}
