/*
  DIY Fog Machine
  parts used:
    3 e-cigarette coils
    2 water level sensors
    1 aquarium pump, to fille liquid from the big containmanet into the small one
    1 relais for switching the professional fog machine off/on 
    1 small containment, for the coils of the fog machine
    1 big containment for the diy fog machine and the professional fog machine
  Based on WaterLevel Sensor Indicator project by Pierre Pennings, 2018
  This code is licensed under GPL3+ license.
*/

//#define _DEBUG
///////////////////////////////////////////////// initialise the GPIO pins
#define NUM_SENSORS 2
const int SensorOnPins[NUM_SENSORS] = { 8, 4 };                       // pin 27 sends a "0"  or "1" (0 -3.3 V) to the waterlevel measurement circuit
const int LevelSensorPins[NUM_SENSORS] = { A0, A1 };                 // 12 bits ADC pin 34 senses the voltage level of the waterlevel sensor (values 0 - 4095)

int WaterLevelValues[NUM_SENSORS] = { 0, 0 };                        // Variable array to store the value of the Waterlevel sensor
int levels[NUM_SENSORS] = { 0, 0 };                                         // Variable array of the WaterLevel

//                      0    1    2    3    4    5
int LEVELarray [6] = {340, 380, 430, 480, 530, 580} ; // Array with the level reference values to determine the waterlevel
// the "0" level is applicable when there is no water in the reservoir
// the "5" level is applicable when the reservoir is full

const int PotiPin = A2;

const int PumpPin = 9;
const int RelaisPin = 10;

#define NUM_HEATINGS 3
const int HeatingPins[NUM_HEATINGS] = { 3, 5, 6 };
int     heatValue = 0;

enum eState {
  S_HEAT_ON = 1,
  S_PUMP_ON = 2,
  S_RELAIS_ON = 4
};

int state = 0;
int oldState = (S_HEAT_ON | S_PUMP_ON | S_RELAIS_ON);

/////////////////////////////////////////////////// the setup code that follows, will run once after "Power On" or after a RESET
void setup() {
  int i;

  Serial.begin(115200);
  Serial.println("DIY fog machine v0.1");

  for (i = 0; i < NUM_SENSORS; i++) {
    pinMode(SensorOnPins[i], OUTPUT);                   // Initializes the power output pin (3.3 V) for the WaterLevel Sensor circuit
    digitalWrite(SensorOnPins[i], LOW);                 // Set SensorOnPin to LOW; this will send 0 V to the measuring circuit

    pinMode(LevelSensorPins[i], INPUT);                 // Initializes the water level sensorpin
  }

  for (i = 0; i < NUM_HEATINGS; i++) {
    pinMode(HeatingPins[i], OUTPUT);                    // Initializes the power output pin (3.3 V) for the heating circuit
    analogWrite(HeatingPins[i], 0);                     // heating circuits off
  }
  pinMode(PumpPin, OUTPUT);                             // Initializes the water pump pin
  digitalWrite(PumpPin, LOW);                           // water pump off
  pinMode(RelaisPin, OUTPUT);                           // Initializes the relais pin
  digitalWrite(RelaisPin, LOW);                         // relais off
  state = 0;

}

/////////////////////////////////////////////////// the loop code that follows, will run repeatedly until "Power Off" or a RESET
void loop() {

  int i;
  
  for (i = 0; i < NUM_SENSORS; i++) {
    getMeasureLevel(i);
  }
#ifdef _DEBUG
  Serial.print(WaterLevelValues[0]);
  Serial.print(",");
  Serial.println(levels[0]);
#endif
  heatValue = map(analogRead(PotiPin), 0, 1024, 0, 255);
  //Serial.print(" Heat: "); Serial.println(heatValue);

  if (levels[1] < 1) {              // all level sensors under minimum
    for (i = 0; i < NUM_HEATINGS; i++) {
      analogWrite(HeatingPins[i], 0);                // heating off
    }
    state &= ~S_HEAT_ON;
    digitalWrite(PumpPin, LOW);                      // water pump off
    state &= ~S_PUMP_ON;
  }
  if (levels[0] >= 2 && levels[1] <= 1) {       // small level sensors under minimum
    digitalWrite(PumpPin, HIGH);                     // water pump on
    state |= S_PUMP_ON;
  }
  else if (levels[1] >= 3) { // small level sensors over maximum
    digitalWrite(PumpPin, LOW);                     // water pump off
    state &= ~S_PUMP_ON;
  }
  if (levels[0] >= 2) { // big containment sensor over minimum
    digitalWrite(RelaisPin, HIGH);                  // relais on, secondary fog machine on
    state |= S_RELAIS_ON;
  }
  else { // big containment sensor under minimum 
    digitalWrite(RelaisPin, LOW);                   // relais off, secondary fog machine off
    state &= ~S_RELAIS_ON;
    digitalWrite(PumpPin, LOW);                     // water pump off
    state &= ~S_PUMP_ON;
  }
  if (levels[1] >= 1) { // both level sensors over minimum
    for (i = 0; i < NUM_HEATINGS; i++) {
      analogWrite(HeatingPins[i], heatValue);       // heating on
    }
    state |= S_HEAT_ON;
  }
  if (state != oldState) {
  #ifndef _DEBUG
    Serial.print("Sensor 1: "); Serial.print(" = "); Serial.print(LEVELarray[levels[0]]); Serial.print("   WaterLevelValue: "); Serial.print(WaterLevelValues[0]); Serial.print("  Level: "); Serial.println(levels[0]);
    Serial.print("Sensor 2: "); Serial.print(" = "); Serial.print(LEVELarray[levels[1]]); Serial.print("   WaterLevelValue: "); Serial.print(WaterLevelValues[1]); Serial.print("  Level: "); Serial.println(levels[1]);
    Serial.print("Heating: "); Serial.println(heatValue);
    if (state & S_HEAT_ON) Serial.println("Heating ON"); else Serial.println("Heating OFF");
    if (state & S_PUMP_ON) Serial.println("Pump ON"); else Serial.println("Pump OFF");
    if (state & S_RELAIS_ON) Serial.println("Relais ON"); else Serial.println("Relais OFF");
  #endif
    oldState = state;
  }
  delay(200);                            // Check for new value every 1 sec;
  //this value is just for demonstration purposes and will in a practical application be far less frequent
}
//////////////////END of LOOP////////////////////////////////////////////////////////////


/////////////////////////////////////////////////// Hereafter follows the Function for measuring the WaterLevel (called from within the loop)

void getMeasureLevel(int num) {
  boolean inRange = false;
  if (num < NUM_SENSORS) {
    digitalWrite(SensorOnPins[num], HIGH);           // make SenorOnPin HIGH
    delay(200);                             // allow the circuit to stabilize
    WaterLevelValues[num] = analogRead(LevelSensorPins[num]);  //Read data from analog pin and store it to WaterLevelvalue variable

    for (int i = 0; i < 6 ; i++)
    {
      if ((WaterLevelValues[num] > (LEVELarray[i] * 0.95)) && (WaterLevelValues[num] < (LEVELarray[i] * 1.05)))              // allow a margin of 4% on the measured values to eliminate jitter and noise
      {
        levels[num] = i;
        inRange = true;
      }
    }
    if (inRange == false) {
        levels[num] = 0;
    }
    
    digitalWrite(SensorOnPins[num], LOW);           // make SenorOnPin LOW
    //Serial.print(" Sensor: "); Serial.print(num + 1); Serial.print(" = "); Serial.print(LEVELarray[levels[num]]); Serial.print("   WaterLevelValue: "); Serial.print(WaterLevelValues[num]); Serial.print("  Level: "); Serial.println(levels[num]);
    // uncomment this code for determining the values to be put in the LEVELarray [] using the serial plotter and/or serial monitor or the ARDUINO IDE
    //Serial.println(levels[num]);
  }
}
