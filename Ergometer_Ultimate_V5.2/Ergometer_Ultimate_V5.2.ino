/*-----Ergometer Ultimate V5.2-----*/

#define interruptPin 2
#define opstarttijd 10000
#define aantal 6          //aantal metingen per omwenteling
#define POT 5
#define PmMinimal 1
#define PmMaximal 30
#define TimeMin 2500
#define TimeMax 3500
#define Motor 13
#define MotorBreak 8
#define MotorSpeed 11

/*-----Constanten-----*/
double pi = 3.14;
double Pm;
double Km = 0.564;
double Ke = 0.9;
float Jnew = 0.00005;     //0.00005/0.000014
float Jold = 0.001;       //0.001/46.164
double Ra = 12.7;

/*-----Variabelen-----*/
boolean x;
float multiply;
unsigned long t1, t2, t3, t = 0;
double TIME;
double ExcelVoltage = 0;
double Omega = 0;
double OmegaOld = 0;
float E = 0;
double Uold = 8.8;
double Unew = 8.8;
double Tm = 0;
double Tl = 0;
float I = 0;
float Iold = 0;
double Alfa = 0;
//double t=2;
int ArduinoVoltage = 150 ;
int PmSwitch;
int TIMEVALUE = 0;
byte meting = 0;
int z;

/*-----LEDS-----*/
int led1 = 0;
int led2 = 0;
int led3 = 0;
int led4 = 0;
int led5 = 0
void setup() {
  Serial.begin(9600);
  pinMode(Motor, OUTPUT);         // motor direction-pin (never changes)
  pinMode(MotorBreak, OUTPUT);    // motor break-pin (never changes)
  pinMode(MotorSpeed, OUTPUT);    // motor speed-pin
  pinMode(3, OUTPUT);             //Led1
  pinMode(4, OUTPUT);             //Led2
  pinMode(5, OUTPUT);             //Led3
  pinMode(6, OUTPUT);             //Led4
  pinMode(7, OUTPUT);             //Led5

  digitalWrite(Motor, HIGH);
  digitalWrite(MotorBreak, LOW);
  analogWrite(MotorSpeed, 200);

  //delay(6000);


  attachInterrupt(digitalPinToInterrupt(interruptPin), timecheck, RISING);

}

void timecheck() {
  t1 = millis();
  meting = 1;
  OmegaOld = Omega;
}

void loop() {
  if (meting == 1) {
    t = t1 - t2;
    t2 = t1;
    meting = 0;
  }

  if (millis() > t2 + 700) {
    t = 0;
    Unew = 0;
  }

  /*-----Aansturing motor-----*/
  if (t > 0) {
    digitalWrite(Motor, HIGH);
    digitalWrite(MotorBreak, LOW);
    analogWrite(MotorSpeed, ArduinoVoltage);
  }
  else {
    digitalWrite(Motor, LOW);
    digitalWrite(MotorBreak, LOW);
    analogWrite(MotorSpeed, 0);
  }

  /*-----POTMETER measures-----*/
  PmSwitch = analogRead(POT);
  Pm = map(PmSwitch, 10, 670, PmMaximal, PmMinimal);
  multiply = 1.5;

  /*-----Calculations-----*/
  if (t > 0) {                                       // will only calculate all the variables when t>0
    Omega = (double)(2000 * pi) / (t * aantal);     // calculate the omega with the time of one round       (2*pi*f)
    Alfa = (double)(Omega - OmegaOld) / (t) * 1000; // calculate Alfa       (alfa=(delta omega)/T)

    E = Ke * Omega;                                 // calculate E          (E=ke*omega)
    if (I >= 0 || I < 0) {                          // checks to see if I is a value. If I is not a value--> I=Iold
      I = (Unew - E) / Ra;                          // calculate I          (I=(Unew-E)/Ra)
      Iold = I;
    }
    else {
      I = Iold;
    }
    Tm = Km * I;                                    // calculate Tm         (Tm=km*I)
    Tl = Tm - (Jold * Alfa);                        // calculate Tl         (Tl=Tm-Tr)
    Uold = (Pm - (I * I * Ra)) / I;
    if (I >= 0 || I < 0) {                          // checks to see if I is a value. If not skip calculation
      Unew = (((Ra / Km) * (Alfa * Jnew * (Pm / Omega)) + (Ke * Omega)) / 10);
      Serial.println(Unew);
    }
    if (Unew > 1) {
      Unew = 1;
    }
  }


  /*-----Dispay using LEDs-----*/
  if (Unew >= 0 || Unew < 0) {
    ExcelVoltage = (double) 1.041218638 + (0.001792114695 * sqrt(49999561 - (5580000 * Unew)));     // calculates the voltage to a number which we use to drive the motor *code

    if (Unew > 0 || Unew > 0) {
      ArduinoVoltage = (double) (163 - (Pm * 1.2) - ((ExcelVoltage - 1)) * 10) * multiply;                 // calculates the voltage to a number which we use to drive the motor *code

      //155-Pm
    }


    else {
      ArduinoVoltage = 0;
    }
  }

  if (Unew < .050) {
    led1 = 1; led2 = 0; led3 = 0; led4 = 0; led5 = 0;
  }
  else if (.150 < Unew && Unew < .250) {
    led1 = 1; led2 = 1; led3 = 0; led4 = 0; led5 = 0;
  }
  else if (.250 < Unew && Unew < .450) {
    led1 = 0; led2 = 1; led3 = 1; led4 = 0; led5 = 0;
  }
  else if (.450 < Unew && Unew < .550) {
    led1 = 0; led2 = 0; led3 = 1; led4 = 0; led5 = 0;
  }
  else if (.550 < Unew && Unew < .650) {
    led1 = 0; led2 = 0; led3 = 1; led4 = 1; led5 = 0;
  }
  else if (.650 < Unew && Unew < .800) {
    led1 = 0; led2 = 0; led3 = 0; led4 = 1; led5 = 1;
  }
  else if (.800 < Unew) {
    led1 = 0; led2 = 0; led3 = 0; led4 = 0; led5 = 1;
  }
  else {
    led1 = 1; led2 = 1; led3 = 1; led4 = 1; led5 = 1;



  }

  digitalWrite(3, led1);
  digitalWrite(4, led2);
  digitalWrite(5, led3);
  digitalWrite(6, led4);
  digitalWrite(7, led5);


}

