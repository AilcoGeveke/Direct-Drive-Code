#include <elapsedMillis.h>

elapsedMillis timeElapsed;
int fsrPin1 = 0;
int fsrPin2 = 1;
int DrukSensor1;
int DrukSensor2;
float Voltage1;
float Voltage2;
float fsrResistance;
float fsrConductance;
float nettoTijd, vorigeTijd = 0;
float f, w, vorigeW, a = 0; //f=freq,w=hoeksnelheid,a=hoekversnelling

int encoderPinA = 3;
int encoderPinB = 4;
float encoderPos = -0.08333;
int encoderPinALast = LOW;
int n = LOW;
float arm = 0.10;
float stap = 0.08333;
float N1 = 0;
float N2 = 0;
float W = 0;

void setup() {
  pinMode (encoderPinA, INPUT_PULLUP);
  pinMode (encoderPinB, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  traphoek();
}


void traphoek() {
  n = digitalRead(encoderPinA);
  if ((encoderPinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoderPinB) == HIGH) {
      encoderPos = encoderPos + stap;
    } else {
      encoderPos = encoderPos + stap;
    }
    hoeksnelheid();                     //Bereken hoeksnelheid en hoekversnelling
    DrukSensor1 = analogRead(fsrPin1);  //Uitlezen van Druksensors
    Serial.print("DrukSensor1:");
    Serial.println(DrukSensor1);
    DrukSensor2 = analogRead(fsrPin2);  //
    arbeid();                           //Arbeid op trappers berekenen
    if (encoderPos > 1.99) {
      encoderPos = 0;
    }
  }
  delay(10);
  encoderPinALast = n;
}

void arbeid() {
  Voltage1 = map(DrukSensor1, 0, 1023, 0, 5000);      //Sensorwaardes converteren naar milliVolts
  Serial.print("Voltage1:");
  Serial.println(Voltage1);
  Voltage2 = map(DrukSensor2, 0, 1023, 0, 5000);      //

  //Formule y = 34331050 + (2.047047 - 34331050)/(1 + (x/162446.1)^3.412111)
  fsrResistance = 5000 - Voltage1;                    
  fsrResistance *= 10000;
  fsrResistance /= Voltage1;
  fsrConductance = 1000000;
  fsrConductance /= fsrResistance;

//If-Else loop om kracht op eerste trapper te berekenen
  if (fsrConductance <= 1000) {
    N1 = fsrConductance / 80;
  } else {
    N1 = fsrConductance - 1000;
    N1 /= 30;
  }
  Serial.print("N1:");
  Serial.println(N1);
  fsrResistance = 5000 - Voltage2;
  fsrResistance *= 10000;
  fsrResistance /= Voltage2;
  fsrConductance = 1000000;
  fsrConductance /= fsrResistance;

  //If-Else loop om kracht op tweede trapper te berekenen
  if (fsrConductance <= 1000) {
    N2 = fsrConductance / 80;
  } else {
    N2 = fsrConductance - 1000;
    N2 /= 30;
  }
  
  W = N1 * arm * cos(encoderPos * PI) + N2 * arm * cos((1 + encoderPos) * PI);
  Serial.print(W);
  Serial.println("Nm");
  //Serial.print("N1: ");
  //Serial.println(N1);
  //Serial.print("N2: ");
  //Serial.println(N2);
}

void hoeksnelheid()
{
  //--- Formules ---
  //w = 2 pi f
  //f = 1 / T
  //----------------

  nettoTijd = (float)timeElapsed - vorigeTijd;
  vorigeTijd = (float)timeElapsed;

  f = 1 / (12 * (nettoTijd));
  w = 2 * PI * f;

  a = w - vorigeW / nettoTijd;
  vorigeW = w;

  
}
