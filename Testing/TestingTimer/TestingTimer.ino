#include <elapsedMillis.h>

elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs

// Pin 13 has an LED connected on most Arduino boards.
int led = 13;

// delay in milliseconds between blinks of the LED
const float interval = 1000;

// state of the LED = LOW is off, HIGH is on
boolean ledState = LOW;

void setup() 
{
  Serial.begin(9600);
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);     
}

void loop()
{
   // Serial.println((float)timeElapsed % (interval/10));
  if (timeElapsed > interval) 
  {
    timeElapsed = 0;
    berekenMe();
    ledState = !ledState;    // toggle the state from HIGH to LOW to HIGH to LOW ... 
    digitalWrite(led, ledState);
           // reset the counter to 0 so the counting starts over...
    Serial.println("---TimeElapsed --- ");
    Serial.println(timeElapsed);
  }
}

void berekenMe()
{
  int sommetje = 1;
    for (int i = 0; i < 35; i++) 
    {
      sommetje = sommetje * i + 1;
      Serial.println(sommetje);
      delay(5);
    }
    Serial.println("---TimeElapsed NA LOOP --- ");
    Serial.println(timeElapsed);
}
