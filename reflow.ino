
// include the library code:
//#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include "Adafruit_RGBLCDShield.h"

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
// #define RED 0x1
#define BACKLIGHT_ON 0x1

int buzzer = D4;    // I have a simple 5v buzzer connected to this ( works fine at 3.3v )
int temp_in = A1;   // This is input from Adafruit AD8495 K-Type Thermocoupler Amplifier
int relay = D7;     // base output to transistor that enables 5v to solid state relay for heating element control

int tempC = 0;
int tempF = 0;
int targetC = 0;

#define MODE_READY 1
#define MODE_RUNNING 2
#define MODE_COOLING 3

#define STATE_HEAT_ON 1
#define STATE_HEAT_OFF 0

int state = STATE_HEAT_OFF;
int mode = MODE_READY;

long start = 0;
long runtime = 0;


/*
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
*/

#define ROOM_TEMP 15

// profile and target code taken from http://reprap.org/wiki/Toaster_Oven_Reflow_Technique

/*
 - heat up to 150oC - this is the flux activation temperature
 - heat up more slowly to 183oC - this is the solder's melting point
 - heat up faster to 215oC - this is the reflow temperature
 - cool
*/

// The temperature/time profile as {secs, temp}
// This profile is linearly interpolated to get the required temperature at any time.
// PLEN is the number of entries
#define PLEN 7
long profile[PLEN][2] = { {0, 15}, {140, 150}, {210, 183}, {280, 215}, {290, 215}, {320, 183}, {350, 0} };

// Linearly interpolate the profile for the current time in secs, t
int target(long t)
{
  if(t <= profile[0][0])
   return profile[0][1];
  if(t >= profile[PLEN-1][0])
  {
   // We are off the end of the time curve
   return profile[PLEN-1][1];
  }
  for(int i = 1; i < PLEN-1; i++)
  {
     if(t <= profile[i][0])
       return (int)(profile[i-1][1] + ((t - profile[i-1][0])*(profile[i][1] - profile[i-1][1]))/
         (profile[i][0] - profile[i-1][0]));
  }
  return 0;
}

void beep(long on) {
  digitalWrite(buzzer, HIGH);
  delay(on);
  digitalWrite(buzzer, LOW);
}

void updateTemp() {
  double raw = analogRead(temp_in);
  double millivolts = ( raw * 3.3 ) / 4096;
  double ttempC = ( millivolts - 1.25 ) / 0.005;
  tempC = (int) ttempC;
  tempF = (int) (( ttempC * 1.8 ) + 32);
}

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(temp_in, AN_INPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(buzzer, LOW);
  digitalWrite(relay, LOW);
  
  Particle.variable("tempC", &tempC, INT);
  Particle.variable("tempF", &tempF, INT);
  Particle.variable("targetC", &targetC, INT);
  
  // Debugging output
  Serial.begin(9600);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);

  // Print a message to the LCD. We track how long it takes since
  // this library has been optimized a bit and we're proud of it :)
  lcd.clear();
  lcd.setBacklight(BACKLIGHT_ON);
  mode = MODE_READY;
  start = millis();
  targetC = 0;
}

void loop() {
  updateTemp();

  runtime = ( (millis() - start) / 1000 );

  if (mode == MODE_RUNNING) {
      targetC = target(runtime);
      if (targetC == 0) {
        mode = MODE_COOLING;
        beep(200);
        delay(100);
        beep(200);
      }
  }

  lcd.setBacklight(BACKLIGHT_ON);

  lcd.setCursor(0, 0);
  if (mode == MODE_READY) { lcd.print("Ready"); }
  if (mode == MODE_RUNNING) { lcd.print("Running"); }
  if (mode == MODE_COOLING) { lcd.print("Cooling"); }

  if (mode != MODE_READY) {
    lcd.print(" time:");
    lcd.print(runtime);
  }

  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(tempC);
  lcd.print("C -> ");
  lcd.print(targetC);
  lcd.print("C  ");

  if (mode == MODE_RUNNING) {
    if (tempC < targetC) {
      digitalWrite(relay, HIGH);
    } else {
      digitalWrite(relay, LOW);
    }
  } else {
    digitalWrite(relay, LOW);
  }

  uint8_t buttons = lcd.readButtons();

  if (buttons) {
    lcd.clear();
    if (buttons & BUTTON_LEFT) {
        beep(20);
        lcd.begin(16,2);
    }
    if (buttons & BUTTON_SELECT) {
      beep(20);
      delay(30);
      beep(20);
      if (mode == MODE_READY) {
        mode = MODE_RUNNING;
        start = millis();
        runtime = 0;
      } else if (mode == MODE_RUNNING) {
        mode = MODE_COOLING;
        beep(100);
      } else if (mode == MODE_COOLING) {
        if (tempC <= 20) {
          mode = MODE_READY;
          start = millis();
          runtime = 0;
        }
      }
    }
  }
  delay(50);
}
