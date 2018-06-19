int fsrPin1 = 0;   
int fsrPin2 = 1;  
int DrukSensor1;
int DrukSensor2;
float Voltage1;
float Voltage2;
float fsrResistance;
float fsrConductance;

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

void traphoek(){
  n = digitalRead(encoderPinA);
  if ((encoderPinALast == LOW) && (n == HIGH)) {
    if (digitalRead(encoderPinB) == HIGH) {
      encoderPos = encoderPos + stap;
    } else {
      encoderPos = encoderPos + stap;
    }
    DrukSensor1 = analogRead(fsrPin1);
    DrukSensor2 = analogRead(fsrPin2);
    arbeid();
    if(encoderPos > 1.99){
      encoderPos = 0;
    }
  }
  delay(10);
  encoderPinALast = n;
}

void arbeid() {
  Voltage1 = map(DrukSensor1, 0, 1023, 0, 5000);
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
  if (fsrConductance <= 1000) {
    N2 = fsrConductance / 80;
  } else {
    N2 = fsrConductance - 1000;
    N2 /= 30;
  }
  W = N1*arm*cos(encoderPos*PI) + N2*arm*cos((1 + encoderPos)*PI); 
  Serial.print(W);
  Serial.println("Nm");
  //Serial.print("N1: ");
  //Serial.println(N1);
  //Serial.print("N2: ");
  //Serial.println(N2);
}
