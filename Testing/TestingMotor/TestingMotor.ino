/*************************************************************
Motor Shield 1-Channel DC Motor Demo
by Randy Sarafan

For more information see:
https://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/

*************************************************************/

void setup() {
  
  //Setup Motor Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel B pin
  pinMode(8, OUTPUT); //Initiates Brake Channel B pin
  
}

void loop(){
  
  //forward @ full speed
  digitalWrite(13, HIGH); //Establishes forward direction of Channel B
  digitalWrite(8, LOW);   //Disengage the Brake for Channel B
  analogWrite(11, 255);   //Spins the motor on Channel B at full speed
  
  delay(3000);
  
  digitalWrite(8, HIGH); //Eengage the Brake for Channel B

  delay(1000);
  
  //backward @ half speed
  digitalWrite(13, LOW); //Establishes backward direction of Channel B
  digitalWrite(8, LOW);   //Disengage the Brake for Channel B
  analogWrite(11, 123);   //Spins the motor on Channel B at half speed
  
  delay(3000);
  
  digitalWrite(8, HIGH); //Eengage the Brake for Channel B
  
  delay(1000);
  
}
