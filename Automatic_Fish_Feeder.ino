#include <Arduino.h>

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200

// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS 1

#define DIR 8
#define STEP 9
#define ENABLE 13 // optional (just delete ENABLE from everywhere if not used)

#include "DRV8825.h"
#define MODE0 10
#define MODE1 11
#define MODE2 12
DRV8825 stepper(MOTOR_STEPS, DIR, STEP, ENABLE, MODE0, MODE1, MODE2);

// #include "BasicStepperDriver.h" // generic
// BasicStepperDriver stepper(DIR, STEP);

//======================== adjustable variables ===============================
double RPM = 10; //The RPM the stepper rotates at
double feedinterval = 5; //feed every 5 hours twice a day
double feedsubinterval = 15; //feed again after 15 minutes for smaller fish
double feedrate = 360; //180 degree turn of the auger bit
int feednumber = 2; //number of feedings per day

//======================== timing variables ============================
unsigned long elapsedMillis;
unsigned long currMillis;
unsigned long innerMillis;
long dayMilli = 86400000; // 86400000 milliseconds in a day
long hourMilli = 3600000; // 3600000 milliseconds in an hour
long minuteMilli = 60000; // 60000 milliseconds in a minute
long secondMilli =  1000; // 1000 milliseconds in a second
long feedtime1 = feedinterval*hourMilli; //Feed in intervals of 5 hours
long feedsubtime = feedsubinterval*minuteMilli; //Feed again after 15 minutes
long feedtime2 = feedtime1 + feedsubtime;
long delaytime = dayMilli - 2*feedtime2; //delay for the rest of the day after the two feeding cycles
extern volatile unsigned long timer0_millis;

//======================= button variables =================================
int buttonPin = 2; //pin number used for tactile button
int ledPin = 7; //pin number used for LED to show tactile button is pushed
int buttonState = 0; //initial button state is 0 for not pushed

void setup() {
    Serial.begin(9600);
    stepper.begin(RPM, MICROSTEPS);
    stepper.enable();
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT);
}

void loop() {
    elapsedMillis = millis(); //Get the current time stamp
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH)
    {
      digitalWrite(ledPin, HIGH); //LED goes on to show button is functioning
      stepper.rotate(360); //for every button push stepper rotates 360
    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    for (int feedcounter = 0; feedcounter < feednumber; feedcounter++)
    {
      elapsedMillis = 0;
      currMillis = millis();
      while(elapsedMillis < feedtime1)
      {
        innerMillis = millis(); //used to offset elapsedmillis
        elapsedMillis = innerMillis - currMillis; //counts elapsed millis in while loops
        buttonState = digitalRead(buttonPin);
        if (buttonState == HIGH)
        {
          digitalWrite(ledPin, HIGH); //LED goes on to show button is functioning
          stepper.rotate(360); //for every button push stepper rotates 360
        }
        else
        {
          digitalWrite(ledPin, LOW);
        }
//        Serial.println("while loop 1");
//        Serial.println(elapsedMillis);
      }
      stepper.rotate(feedrate); //first feeding
      while(elapsedMillis < feedtime2)
      {
        innerMillis = millis(); //used to offset elapsedmillis
        elapsedMillis = innerMillis - currMillis; //counts elapsed millis in while loops
        buttonState = digitalRead(buttonPin);
        if (buttonState == HIGH)
        {
          digitalWrite(ledPin, HIGH); //LED goes on to show button is functioning
          stepper.rotate(360); //for every button push stepper rotates 360
        }
        else
        {
         digitalWrite(ledPin, LOW);
        }
//        Serial.println("while loop 2");
//        Serial.println(elapsedMillis);
      }
      stepper.rotate(feedrate); //second feeding
    }
    elapsedMillis = millis();
    while(elapsedMillis < delaytime)
    {
      elapsedMillis = millis(); //Get the current time stamp
      buttonState = digitalRead(buttonPin);
      if (buttonState == HIGH)
      {
        digitalWrite(ledPin, HIGH); //LED goes on to show button is functioning
        stepper.rotate(360); //for every button push stepper rotates 360
      }
      else
      {
        digitalWrite(ledPin, LOW);
      }
//      Serial.println("while loop 3");
//      Serial.println(elapsedMillis);
    }
    noInterrupts (); //resets millis to zero
    timer0_millis = 0;
    interrupts ();
}
