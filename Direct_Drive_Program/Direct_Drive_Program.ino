#include <elapsedMillis.h>
elapsedMillis timeElapsed;

const int fsrPin1 = 2;                        //Analoog
const int fsrPin2 = 3;                        //Analoog
const int resetPin = 4;                       // select the input pin for the interrupter
const int encoderPinA = 5;                      //Digitaal
const int encoderPinB = 4;                      //Digitaal
const int vermogenMotor = 40;
const float motorConstante = 0.5424;             //Kw of Km
const float mechanischeConstante = 0.222;        //Ke

int countSteps = 0;
int DrukSensor1, DrukSensor2;
float Voltage1, Voltage2;
float nettoTijd, vorigeTijd = 0;
float f, w, vorigeW, a = 0; //f=freq,w=hoeksnelheid,a=hoekversnelling


float encoderPos = -0.08333;
int encoderPinALast = LOW;
int n = LOW;
int motorSnelheid = 0;
int resetWaarde;
float arm = 0.10;
float stap = 0.08333;
float N1 = 0;
float N2 = 0;
float W = 0;
float massatraagheid;         //I
float klemspanning;           //Uk
float motorKoppel;
float nettoKoppel;
const int ankerweerstand = 89;         //Ra


void setup() {
  //Setup Motor Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel B pin
  pinMode(8, OUTPUT); //Initiates Brake Channel B pin

  pinMode (encoderPinA, INPUT_PULLUP);
  pinMode (encoderPinB, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  mainProgram();
}


void mainProgram() {
  n = digitalRead(encoderPinA);
  resetWaarde = analogRead(resetPin);
  if (resetWaarde < 50 || encoderPos > 1.99) {
    encoderPos = 0;
    countSteps = 0;
  }
  if ((encoderPinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoderPinB) == HIGH) {
      encoderPos = encoderPos + stap;
      countSteps++;
    } else {
      //encoderPos = encoderPos - stap;
    }
    //Serial.println("------------------------------------------");
    hoeksnelheid();                     //Bereken hoeksnelheid en hoekversnelling
    DrukSensor1 = analogRead(fsrPin1);  //Uitlezen van Druksensors
    //Serial.print("DrukSensor1:");
    //Serial.println(DrukSensor1);
    DrukSensor2 = analogRead(fsrPin2);  //
    //Serial.print("DrukSensor2:");
    //Serial.println(DrukSensor2);
    arbeid();                           //Arbeid op trappers berekenen
    //berekenNettoKracht();
    //berekenKlemspanning();
    //Serial.println("------------------------------------------");
  }
  encoderPinALast = n;

}

void arbeid() {
  Voltage1 = map(DrukSensor1, 0, 1023, 0, 5000);      //Sensorwaardes converteren naar milliVolts
  //Serial.print("Voltage1:");
  //Serial.println(Voltage1);
  //Serial.print("Voltage2:");
  //Serial.println(Voltage2);
  Voltage2 = map(DrukSensor2, 0, 1023, 0, 5000);      //

  //Formule y = 34331050 + (2.047047 - 34331050)/(1 + (x/162446.1)^3.412111)
  if (Voltage1 != 0)
    N1 = 34331050 + (2.047047 - 34331050) / (1 + (pow((Voltage1 / 162446.1), 3.412111)));
  else
    N1 = 0;
  //Serial.print("NewtonForce1:");
  //Serial.print(N1);
  //Serial.println(" N");

  if (Voltage2 != 0)
    N2 = 34331050 + (2.047047 - 34331050) / (1 + (pow((Voltage2 / 162446.1), 3.412111)));
  else
    N2 = 0;
  //Serial.print("NewtonForce2:");
  //Serial.print(N2);
  //Serial.println(" N");

  //Serial.print("Arm:");
  //Serial.print(arm);
  //Serial.println(" m");

  Serial.print("EncoderPositie:");
  Serial.println(encoderPos);
  Serial.print("Counter:");
  Serial.println(countSteps);
  //Serial.println(" rad");
  W = N1 * arm * cos(encoderPos * PI) + N2 * arm * cos((1 + encoderPos) * PI);
  //Serial.print("Arbeid:");
  //Serial.print(W);
  //Serial.println("Nm");
}

void hoeksnelheid()
{
  //--- Formules ---
  //w = 2 pi f
  //f = 1 / T
  //----------------

  nettoTijd = ((float)timeElapsed - vorigeTijd) / 1000;       //Delen door duizend: ms => s
  vorigeTijd = (float)timeElapsed;

  f = 1 / (12 * (nettoTijd));
  w = 2 * PI * f;

  a = w - vorigeW / nettoTijd;
  vorigeW = w;
}


void berekenNettoKracht() //T = Iα
{
  if (a == 0)
    massatraagheid = 0;
  else
    massatraagheid = W / a;                               //I = Tlast / α

  motorKoppel = motorConstante * massatraagheid;        //Tm = Kw * I
  nettoKoppel = motorKoppel - W;                        //Tnetto = Tm - Tlast

  Serial.print("Massatraagheid:");
  Serial.print(massatraagheid);
  Serial.println(" ??\t");
  Serial.print(" MotorKoppel:");
  Serial.print(motorKoppel);
  Serial.println(" Nm\t");
  Serial.print(" NettoKoppel:");
  Serial.print(nettoKoppel);
  Serial.println(" Nm");
}

void berekenKlemspanning() {
  //Uk = 1000 * (Pm * (Ra / (Km * w) + Ke * w));       //Klemspanning in milliVolts dus * 1000
  if (massatraagheid == 0)
    klemspanning = 1000 * (vermogenMotor * (ankerweerstand / (motorConstante * w) + mechanischeConstante * w));
  else
    klemspanning = 1000 * ((ankerweerstand / motorConstante) * (nettoKoppel + (vermogenMotor)) + mechanischeConstante * w);

  Serial.print("Klemspanning:");
  Serial.print(klemspanning);
  Serial.println(" mV");

  if (klemspanning < 0)
  {
    klemspanning = klemspanning * -1;
    motorSnelheid = map(klemspanning, 0, 12000, 0, 255);
    digitalWrite(13, LOW); //Establishes backward direction of Channel B
    digitalWrite(8, LOW);   //Disengage the Brake for Channel B
    analogWrite(11, motorSnelheid);   //Spins the motor on Channel B at given speed
    delay(10);                  //-----------------------DELAY?????????????-----------------
    digitalWrite(8, HIGH);   //Engage the Brake for Channel B
  }
  else
  {
    motorSnelheid = map(klemspanning, 0, 12000, 0, 255);
    digitalWrite(13, HIGH);           //Establishes forward direction of Channel B
    digitalWrite(8, LOW);             //Disengage the Brake for Channel B
    analogWrite(11, motorSnelheid);   //Spins the motor on Channel B at given speed
    delay(10);                        //-----------------------DELAY?????????????-----------------
    digitalWrite(8, HIGH);   //Engage the Brake for Channel B
  }
}
