#include <elapsedMillis.h>
elapsedMillis timeElapsed;

const int fsrPin1 = 2;                        //Analoog
const int fsrPin2 = 3;                        //Analoog
const int resetPin = 4;                       // select the input pin for the interrupter
const int encoderPinA = 5;                      //Digitaal
const int encoderPinB = 4;                      //Digitaal
const float vermogenMotor = 0.5;
const float motorConstante = 0.43;             //Kw of Km
const float mechanischeConstante = 3;        //Ke
const float Jfiets = 0.001;
const float Jmotor = 0.0001433;        //Jmotor
float gewensteA = 0;

int countRes, countSteps = 0;
int DrukSensor1, DrukSensor2;
float Voltage1, Voltage2;
float tijd, nettoTijd, vorigeTijd, totaalTijd = 0;
float f, w, vorigeW, w2, a = 0; //f=freq,w=hoeksnelheid,a=hoekversnelling


float encoderPos = -0.5;
int encoderPinALast = LOW;
int n = LOW;
int motorSnelheid = 0;
int resetWaarde;
int deler = 0;
float arm = 0.10;
float stap = 0.08333;
float fsrResistance, fsrConductance;
float N1 = 0;
float N2 = 0;
float W, Wt = 0;

float klemspanning;           //Uk
float motorKoppel;
float nettoKoppel;
const int ankerweerstand = 89;         //Ra


void setup() {
  //Setup Motor Channel B
  pinMode(12, OUTPUT); //Initiates Motor Channel B pin
  pinMode(9, OUTPUT); //Initiates Brake Channel B pin

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
  if (resetWaarde < 50 || encoderPos > 1.49) {
    encoderPos = -0.5;
    countSteps = 0;
    totaalTijd = 0;
    w2 = 0;
  }
  if ((encoderPinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoderPinB) == HIGH) {
      encoderPos = encoderPos + stap;
      countSteps++;
      Serial.println("------------------------------------------");
      hoeksnelheid();                     //Bereken hoeksnelheid en hoekversnelling
      arbeid();                           //Arbeid op trappers berekenen
      //berekenKlemspanning();
      //Serial.println("------------------------------------------");
    } else {
      //encoderPos = encoderPos - stap;
    }
  }
  encoderPinALast = n;

}

void arbeid() {
  DrukSensor1 = analogRead(fsrPin1);  //Uitlezen van Druksensors
  DrukSensor2 = analogRead(fsrPin2);  //
  Voltage1 = map(DrukSensor1, 0, 1023, 0, 5000);      //Sensorwaardes converteren naar milliVolts
  Voltage2 = map(DrukSensor2, 0, 1023, 0, 5000);
  fsrResistance = 5000 - Voltage1;
  fsrResistance *= 10000;
  fsrResistance /= Voltage1;
  fsrConductance = 1000000;
  fsrConductance /= fsrResistance;
  if (fsrConductance <= 1000) {
    N1 = fsrConductance / 80;
  } else {
    N1 = fsrConductance - 1000;
    N1 /= 30;
  }

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

  //Serial.print("Counter:");
  //Serial.println(countSteps);


  W = N1 * arm * cos(encoderPos * PI) + N2 * arm * cos((1 + encoderPos) * PI);
  Wt = Wt + w;
  deler++;
  if (countSteps == 1) {
    Wt = Wt / deler;
    nettoKracht();
    //Serial.print("Arbeid:");
    //Serial.print(Wt);
    //Serial.println("Nm");
    Wt = 0;
    deler = 0;
  }

}


void hoeksnelheid()
{
  if (countSteps == 6 || countSteps == 12 || countSteps == 18 || countSteps == 24 ) {
    tijd = (float)timeElapsed / 1000;     //Delen door duizend: ms => s
    nettoTijd = (tijd - vorigeTijd);
    vorigeTijd = tijd;

    f = 1 / (4 * (nettoTijd));
    w = 2 * PI * f;
    a = (w - vorigeW) / nettoTijd;
    vorigeW = w;

    //Serial.print("Hoeksnelheid:");
    //Serial.print(w);
    //Serial.println(" rad/s");
    //Serial.print("Hoekversnelling:");
    //Serial.print(a);
    //Serial.println(" rad/s^2");
  }
}


void nettoKracht() //T = Iα
{

  nettoKoppel = W - vermogenMotor / w; //Tn = Tlast - Tstatisch
  gewensteA = nettoKoppel / Jfiets;
  berekenKlemspanning();
  //Serial.print("Massatraagheid:");
  //Serial.print(massatraagheid);
  //Serial.println(" ??\t");
  //Serial.print(" MotorKoppel:");
  //Serial.print(motorKoppel);
  //Serial.println(" Nm\t");
  Serial.print(" NettoKoppel:");
  Serial.print(nettoKoppel);
  Serial.println(" Nm");
}

void berekenKlemspanning() {
  //Uk = 1000 * (Pm * (Ra / (Km * w) + Ke * w));       //Klemspanning in milliVolts dus * 1000
  if (a > -0.1 || a < 0.1)
    klemspanning =  (vermogenMotor * ((ankerweerstand) / (motorConstante * w)) + (mechanischeConstante * w));
  else
    klemspanning =  ((ankerweerstand / motorConstante) * (gewensteA * Jmotor + vermogenMotor / w) + mechanischeConstante * w);

  if (w < 0.6)
    klemspanning = 0;
  Serial.print("Klemspanning:");
  Serial.print(klemspanning);
  Serial.println(" mV");

  if (klemspanning < 0)
  {
    klemspanning = klemspanning * -1;
    motorSnelheid = map(klemspanning, 0, 120, 0, 90);
    Serial.print("MotorSnelheid 0-255:");
    Serial.println(motorSnelheid);
    if (motorSnelheid > 40)
      motorSnelheid = 40;
    digitalWrite(12, HIGH); //Establishes backward direction of Channel B
    digitalWrite(9, LOW);   //Disengage the Brake for Channel B
    analogWrite(3, motorSnelheid);   //Spins the motor on Channel B at given speed
    //digitalWrite(, HIGH);   //Engage the Brake for Channel B
  }
  else
  {
    motorSnelheid = map(klemspanning, 0, 120, 0, 90);
    Serial.print("MotorSnelheid 0-255:");
    Serial.println(motorSnelheid);
    if (motorSnelheid > 40)
      motorSnelheid = 40;
    digitalWrite(12, LOW);           //Establishes forward direction of Channel B
    digitalWrite(9, LOW);             //Disengage the Brake for Channel B
    analogWrite(3, motorSnelheid);   //Spins the motor on Channel B at given speed
    //digitalWrite(8, HIGH);   //Engage the Brake for Channel B
  }
}
