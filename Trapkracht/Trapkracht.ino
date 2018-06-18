/* Read Quadrature Encoder
   Connect Encoder to Pins encoder0PinA, encoder0PinB, and +5V.

   Sketch by max wolf / www.meso.net
   v. 0.1 - very basic functions - mw 20061220

*/

int fsrPin = 0;     
int fsrReading;
int val;
int encoder0PinA = 3;
int encoder0PinB = 4;
float encoder0Pos = -0.08333;
int encoder0PinALast = LOW;
int n = LOW;
float arm = 0.10;
float stap = 0.08333;
float W = 0;

void setup() {
  pinMode (encoder0PinA, INPUT_PULLUP);
  pinMode (encoder0PinB, INPUT_PULLUP);
  Serial.begin (9600);
}

void loop() {

  traphoek();
  arbeid();
  
  
}

void traphoek(){

  n = digitalRead(encoder0PinA);
  if ((encoder0PinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoder0PinB) == HIGH) {
      encoder0Pos = encoder0Pos + stap;
    } else {
      encoder0Pos = encoder0Pos + stap;
    }
    fsrReading = analogRead(fsrPin);
    if(encoder0Pos > 1.99){
      encoder0Pos = 0;
    }
    /*Serial.print (encoder0Pos);
    Serial.println ("pi");*/
  }
  delay(10);
  encoder0PinALast = n;
}

void arbeid() {
   
  W = fsrReading*arm*cos(encoder0Pos); 
  Serial.print(W);
  Serial.println("Nm");
}
