const int TEMP_UP   = 1;
const int TEMP_DOWN = 0;
int ramp = TEMP_UP;

const int MAX_TEMP = 100;
const int MIN_TEMP = 50;
int temp = MIN_TEMP;

const int HEAT = 1;
const int COOL = 2;
int hvacMode = HEAT;
const int heatLed  = 4;
const int coolLed  = 5;

int airflow;
// Servo airflow opening size (90 degress/4)
const int AIR_10_PCT  = 5;
const int AIR_25_PCT  = 22;
const int AIR_50_PCT  = 45;
const int AIR_75_PCT  = 67; 
const int AIR_100_PCT = 90;


const int activityLed = 13;
int activityToggle = 0;

int high;
int low;

void setup()
{
  // initialize serial output for Zigbee
  Serial.begin(9600);
  pinMode(activityLed, OUTPUT);
  
  pinMode(heatLed, OUTPUT);
  pinMode(coolLed, OUTPUT);
}

int getTemp ()
{
  if (temp < MAX_TEMP) {
    temp++;
  } else  {
    temp = MIN_TEMP;
  }
  return temp;
}

int getTemp2 ()
{
  // Temperature simulator
  if (temp < MAX_TEMP && ramp == TEMP_UP) {
    temp++;
  } else if (temp == MAX_TEMP) {
    ramp = TEMP_DOWN;
  } else if (temp > MIN_TEMP && ramp == TEMP_DOWN) {
    temp--;
  } else if (temp == MIN_TEMP) {
    ramp = TEMP_UP;
  } else {
    temp = MIN_TEMP;
    ramp = TEMP_UP;
  }

  return temp;  
}

int getTargetTemp () 
{
  return 72; 
}

void setHvacMode (int mode) 
{
  hvacMode = mode;

  switch (mode) {
    case HEAT:
      digitalWrite(heatLed, HIGH);
      digitalWrite(coolLed, LOW);    
    break;
    
    case COOL:
      digitalWrite(heatLed, LOW);
      digitalWrite(coolLed, HIGH);
    break;
    
    default:
      digitalWrite(heatLed, HIGH);
      digitalWrite(coolLed, HIGH);
    break;
    
  }
}

int getAirFlow(int curTemp, int trgTemp)
{
  int tempDiff = curTemp - trgTemp;
  int flowPct = AIR_10_PCT;

  // convert to positive number
  if (tempDiff < 0) 
    tempDiff *= -1;   
    
  // determine gradient size
  if (tempDiff >= 20)
    flowPct = AIR_100_PCT;
  else if (tempDiff >= 15)
    flowPct = AIR_75_PCT;
  else if (tempDiff >= 10)
    flowPct = AIR_50_PCT;
  else if (tempDiff >= 5)
    flowPct = AIR_25_PCT;
  else
    flowPct = AIR_10_PCT;
    
  return flowPct;  
  
}

void loop()
{

  // Temperature data
  int currentTemp = getTemp(); 
  int targetTemp = getTargetTemp();
  
  // HVAC mode
  if (currentTemp <= targetTemp) {
    setHvacMode(HEAT);
  } else {
    setHvacMode(COOL);
  }

  // Airflow
  int newAirflow = getAirFlow(currentTemp, targetTemp);
  if (airflow != newAirflow) {

    // Update  
    airflow = newAirflow;

    // Send airflow setting
    switch (airflow) {
      case AIR_10_PCT:  Serial.write('a'); break;
      case AIR_25_PCT:  Serial.write('b'); break;
      case AIR_50_PCT:  Serial.write('c'); break;
      case AIR_75_PCT:  Serial.write('d'); break;
      case AIR_100_PCT: Serial.write('e'); break;
      default:          Serial.write('a'); break;
    }

    digitalWrite(activityLed, HIGH);
    delay(100);
    digitalWrite(activityLed, LOW);
  }
  
  // Send temperature readings
  high = targetTemp/10 + 0x30;
  low  = targetTemp%10 + 0x30;
  Serial.write('x');
  Serial.write(targetTemp);

  delay(250);

  high = currentTemp/10 + 0x30;
  low  = currentTemp%10 + 0x30;
  Serial.write('y');
  Serial.write(currentTemp);


  delay(250);


}
