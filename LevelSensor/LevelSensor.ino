/*
  This code for a WaterLevel sensor and a WaterLevel Indicator has been developed and produced by Pierre Pennings (December 2018)
  This application can be used for various situations where information about the level of water in a reservoir tank is required
  e.g. in automatic plant watering systems or in caravans or campers where there is no direct visibility on the water reserve in the tank.
  The DIY WaterLevelSensor uses 6 pieces of copper electricity wire connected to a ladder network of 680K Ohm resistors
  The DIY WaterLevel Indicator is made with 5 (Neopixel) SMD5050 LEDs with WS2812B controller chips powered with 5 V
  Every individual LED is adressed from one ARDUINO output pin and the control adress determined by the measured WaterLevel
  The WaterLevel is measured periodically (during only 200 mili seconds, to avoid corrosion due to electrolysis effects)
  The WaterLevel Indicator is set to the measured water level
  The measured WaterLevelValues are not distributed linearly but follow a second grade polynomial
  Reference values for the measurements are stored in an Array called LEVELarray[] consisting of 6 positions
  The actual measured WaterLevelValues are compared with the values in the Array and consquently the Water level is determined
  For this Project, which is part of a bigger plan, an ESP 32 (NodeMCU) is used with 12 Bits ADCs, however an normal ARDUINO UNO (or almost any other model) will do the job
  (of course the settings in the code will need to be adjusted, e.g. due to 10 Bits ADC and different Pin allocations
  The ESP 32 device works at 3.3 Volt levels, while the WaterLevel Indicator runs on 5 V
  The 5 Level LEDs have been built in a separate indicator Display (indicating 1%, 25%, 50%, 75% and 100% levels)
  The ESP 32 is fed with 5 V power (from a 5V adaptor or 5v powerbank), it has an on-board 3.3V voltage regulator
  The 5 indicator LEDs get the 5V supply directly from the 5 volt pin of the ESP 32

  This code is licensed under GPL3+ license.
*/

///////////////////////////////////////////////// initialise the GPIO pins
#define NUM_SENSORS 2
const int SensorOnPins[NUM_SENSORS] = { 8, 4 };                       // pin 27 sends a "0"  or "1" (0 -3.3 V) to the waterlevel measurement circuit
const int LevelSensorPins[NUM_SENSORS] = { A0, A1 };                 // 12 bits ADC pin 34 senses the voltage level of the waterlevel sensor (values 0 - 4095)

int WaterLevelValues[NUM_SENSORS] = { 0, 0 };                        // Variable to store the value of the Waterlevel sensor
int level = 0;                                  // Variable of the WaterLevel

//                      0    1    2    3    4    5
int LEVELarray [6] = {340, 370, 430, 500, 590, 680} ; // Array with the level reference values to determine the waterlevel
// the "0" level is applicable when there is no water in the reservoir
// the "5" level is applicable when the reservoir is full


/////////////////////////////////////////////////// the setup code that follows, will run once after "Power On" or after a RESET
void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(SensorOnPins[i], OUTPUT);                    // Initializes the power output pin (3.3 V) for the WaterLevel Sensor circuit
    digitalWrite(SensorOnPins[i], LOW);                  // Set SensorOnPin to LOW; this will send 0 V to the measuring circuit

    pinMode(LevelSensorPins[i], INPUT);               // Initializes the water level sensorpin
  }

}

/////////////////////////////////////////////////// the loop code that follows, will run repeatedly until "Power Off" or a RESET
void loop() {

  for (int i = 0; i < NUM_SENSORS; i++) {
    getMeasureLevel(i);
  }
  delay(1000);                            // Check for new value every 1 sec;
  //this value is just for demonstration purposes and will in a practical application be far less frequent
}
//////////////////END of LOOP////////////////////////////////////////////////////////////


/////////////////////////////////////////////////// Hereafter follows the Function for measuring the WaterLevel (called from within the loop)

void getMeasureLevel(int num)  {
  if (num < NUM_SENSORS) {
    digitalWrite(SensorOnPins[num], HIGH);           // make pin 27 HIGH
    delay(200);                             // allow the circuit to stabilize
    WaterLevelValues[num] = analogRead(LevelSensorPins[num]);  //Read data from analog pin and store it to WaterLevelvalue variable

    for (int i = 0; i < 6 ; i++)
    {
      if ((WaterLevelValues[num] > (LEVELarray[i] * 0.96)) && (WaterLevelValues[num] < (LEVELarray[i] * 1.04)))              // allow a margin of 4% on the measured values to eliminate jitter and noise
      {
        level = i;
      }
    }
    digitalWrite(SensorOnPins[num], LOW);           // make pin 27 LOW
    Serial.print(" Sensor: "); Serial.print(num+1); Serial.print(" = "); Serial.print(LEVELarray[level]); Serial.print("   WaterLevelValue: "); Serial.print(WaterLevelValues[num]); Serial.print("  Level: "); Serial.println(level);
    // uncomment this code for determining the values to be put in the LEVELarray [] using the serial plotter and/or serial monitor or the ARDUINO IDE
  }
}
