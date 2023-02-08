/*
 * A0, D4..D9 used for LED+buttons
 * A1 used for Pressure sensor (Chrysler 4.7 5.7 Oil pressure sensor)
 * D11 used for control the pump relay to provide desired pressure in given range.
 *
 * it was a sketch_feb12a.ino
 */
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // D

#define btnNONE   0
#define btnRIGHT  1
#define btnUP     2
#define btnDOWN   3
#define btnLEFT   4
#define btnSELECT 5

#define MIN_VALUE 100           // min acceptable pressure to setup 1.00
#define MAX_VALUE 400           // max acceptable pressure to setup 4.00
#define DEFAULT_MIN_VALUE 200   // min after swith on
#define DEFAULT_MAX_VALUE 300   // max after switch on
#define PRESS_LOW 1             // left position
#define PRESS_HIGH 2            // right position
#define DELTA 10                // 0.1 delta

#define PORT_PRESSURE_SENSOR A1 // A
#define PORT_RELAY_PUMP 11      // D swichON=LOW, OFF=HIGH
#define PORT_BARREL_IS_EMPTY 12 // D closed - OK, open - EMPTY

#define EMPTY_LINE      "                "
#define BARREL_IS_EMPTY "BARREL IS EMPTY!"

#include "liveness.h"

int pressure_low;               // 100 - 400
int pressure_high;              // 100 - 400
int pressure_low_volt;          // 0 - 1023
int pressure_high_volt;         // 0 - 1023

int pressure_current_volt;

int current_position;           // PRESS_LOW (LEFT) / PRESS_HIGH (RIGHT)
bool isRelayOn;
bool isEmpty;                   // current state of "empty"

// used to 'mute' the sensor, if low=high we don't use the sensor's value for relay control.
bool useSensor;

/////////////////////////////////////////////////////////
//          MAPPING TABLES AND FUNCTIONS
/////////////////////////////////////////////////////////

int voltage_val[] = { 110, 230, 337, 454, 570, 690, 810, 920 };
int pressure_val[] = {  0, 100, 200, 300, 400, 500, 600, 700 };
int array_length = sizeof(voltage_val) / sizeof(int);

int map(int val, int from[], int to[]) {
  if (val < from[0]) {
    return to[0];
  }
  
  for (int i = 1; i < array_length; i++) {
    if (val < from[i]) {
      int deltaFrom = from[i] - from[i - 1];
      int deltaTo = to[i] - to[i - 1];

      int shiftFrom = val - from[i - 1];
      float k = (float) shiftFrom / deltaFrom;
      return to[i-1] + deltaTo * k;
    }
  }

  return to[array_length - 1];
}

int volt_to_press(int volt) {
  return map(volt, voltage_val, pressure_val);
}

int press_to_volt(int pres) {
  return map(pres, pressure_val, voltage_val);
}

/////////////////////////////////////////////////////////
//          SETUP
/////////////////////////////////////////////////////////

void setup() {
  digitalWrite(PORT_RELAY_PUMP, HIGH);
  pinMode(PORT_RELAY_PUMP, OUTPUT);
  pinMode(PORT_BARREL_IS_EMPTY, INPUT);
  // swith pull up resistor to avoid unexpected behavior without sensor
  digitalWrite(PORT_PRESSURE_SENSOR, HIGH);
  lcd.begin(16, 2);
  pressure_low = DEFAULT_MIN_VALUE;
  pressure_high = DEFAULT_MAX_VALUE;
  current_position = PRESS_HIGH;
  useSensor = true;
  isEmpty = true;   // by default barrel is empty
  switchRelayOff();
  update_pressures_volt();
}

/////////////////////////////////////////////////////////
//          SENSORS READING
/////////////////////////////////////////////////////////

int read_button() {
  int in = analogRead(0); // 0 - port, which used to read buttons
  if (in < 50)  return btnRIGHT;  // 0
  if (in < 150) return btnUP;     // 97
  if (in < 300) return btnDOWN;   // 253
  if (in < 450) return btnLEFT;   // 405
  if (in < 700) return btnSELECT; // 638
  return btnNONE;
}

int read_pressure() {
  return analogRead(PORT_PRESSURE_SENSOR);
}

// HIGH = we are in open  position and pulled-up to +5V
// LOW  = we are in close position and short-circuited to the GROUND
bool read_is_empty() {
  return digitalRead(PORT_BARREL_IS_EMPTY) == HIGH;
}

/////////////////////////////////////////////////////////
//          KEYBOARD HANDLING
/////////////////////////////////////////////////////////

void inc_low() {
  if (pressure_low < pressure_high) {
    pressure_low += DELTA;
  }
}

void dec_low() {
  if (pressure_low > MIN_VALUE) {
    pressure_low -= DELTA;
  }
}

void inc_high() {
  if (pressure_high < MAX_VALUE) {
    pressure_high += DELTA;
  }
}

void dec_high() {
  if (pressure_high > pressure_low) {
    pressure_high -= DELTA;
  }
}

void selectLow() {
  current_position = PRESS_LOW;
}

void selectHigh() {
  current_position = PRESS_HIGH;
}

// convert PSI to Volts for futher comparision
void update_pressures_volt() {
  pressure_low_volt  = press_to_volt(pressure_low);
  pressure_high_volt = press_to_volt(pressure_high);
}

void update_useSensor() {
  useSensor = (pressure_low != pressure_high);
}

void buttonUp() {
  if (current_position == PRESS_LOW) {
    inc_low();
  } else {
    inc_high();
  }
  update_pressures_volt();
  update_useSensor();
}

void buttonDown() {
  if (current_position == PRESS_LOW) {
    dec_low();
  } else {
    dec_high();
  }
  update_pressures_volt();
  update_useSensor();
}

/////////////////////////////////////////////////////////
//          PUMP OPERATIONS
/////////////////////////////////////////////////////////

void switchPumpManually() {
  if (isRelayOn) {
    switchRelayOff();
  } else {
    switchRelayOn();
  }
}

void switchRelayOn() {
  digitalWrite(PORT_RELAY_PUMP, LOW);
  isRelayOn = true;
}

void switchRelayOff() {
  digitalWrite(PORT_RELAY_PUMP, HIGH);
  isRelayOn = false;
}

void controlPressure() {
  pressure_current_volt = read_pressure();
  isEmpty = read_is_empty();

  if (isEmpty) {
    switchRelayOff();
    return;
  }

  if (!useSensor) return;
  
  if (pressure_current_volt >= pressure_high_volt) {
    switchRelayOff();
  }
  
  if (pressure_current_volt <= pressure_low_volt) {
    switchRelayOn();
  }
}

/////////////////////////////////////////////////////////
//          DISPLAY FUNCTIONS
/////////////////////////////////////////////////////////

String intToReadable(int value) {
  int digit1 = value / 100;
  int digit2 = (value % 100) / 10;
  int digit3 = value % 10;
  return String(digit1) + "." + String(digit2) + String(digit3);
}

String getLine1Contents() {
  return
    intToReadable(pressure_low) +
    "/" + intToReadable(pressure_high) +
    " C:" + intToReadable(volt_to_press(pressure_current_volt)) + " ";
}

String getLine2Contents() {
  return
    String(useSensor ? "A" : "M") +
    "V:" + String(pressure_current_volt) + " ";
}

void displayLine1() {
  lcd.setCursor(0,0);
  lcd.print(getLine1Contents());
}

String represent(bool val) {
  return val ? "*" : " ";
}

void displayLine2() {
  lcd.setCursor(0,1);

  if (isEmpty) {
    lcd.print(BARREL_IS_EMPTY);
    return;
  }

  lcd.print(EMPTY_LINE);
  lcd.setCursor(0,1);
  lcd.print(liveness_char());
  lcd.setCursor(2,1);
  lcd.print(represent(current_position == PRESS_LOW));
  lcd.setCursor(7,1);
  lcd.print(represent(current_position == PRESS_HIGH));
  lcd.setCursor(9,1);
  lcd.print(getLine2Contents());
}

void display() {
  displayLine1();
  displayLine2();
}

/////////////////////////////////////////////////////////
//          MAIN LOOP
/////////////////////////////////////////////////////////

void loop() {
  display();
  
  switch (read_button()) {
    case btnRIGHT: {
       selectHigh();
       break;
    }
    case btnLEFT: {
       selectLow();
       break;
    }
    case btnUP: {
       buttonUp();
       break;
    }
    case btnDOWN: {
       buttonDown();
       break;
    }
    case btnSELECT: {
       switchPumpManually();
       break;
    }
    case btnNONE: controlPressure();
 }
  delay(200);
}
