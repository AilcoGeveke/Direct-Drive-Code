  68#include <DateTime.h>
#include <TimerAlarms.h>

AlarmID_t AlarmA, AlarmB, AlarmC, AlarmD;

void onAlarm(AlarmID_t Sender){
// alarm callback function
Serial.print("cume time ");  
Serial.print(DateTime.now());  // print elapsed seconds since sketch started runnin 

Serial.print(": ");
 if( Sender == AlarmA) {
     Serial.println("15 seconds Alarm");         
 }
 else  if( Sender == AlarmB){
     Serial.println("90 second Alarm");    
 }
 else  if( Sender == AlarmC){
     Serial.print("One Shot alarm, elapsed period was ");    
     time_t alarmValue  = dtAlarms.getValue(Sender);
     Serial.println(alarmValue,DEC); 
 }
 else  if( Sender == AlarmD){
     Serial.print("re-trigged One Shot alarm, elapsed period was ");    
     time_t alarmValue  = dtAlarms.getValue(Sender);
     Serial.println(alarmValue,DEC); 
     dtAlarms.setValue(Sender, alarmValue + 2); //re-enable with a new value two seconds longer
   }
}

void setup(){
  pinMode(13,OUTPUT);
  Serial.begin(19200);

  AlarmA = dtAlarms.createRepeating( 15 );  // alarm every 15 seconds
  AlarmB = dtAlarms.createRepeating( AlarmHMS(0,1,30) );  // alarm every 1 minute 30 seconds
  AlarmC = dtAlarms.createOneshot( 10 );  // one shot alarm in 10 seconds
  AlarmD = dtAlarms.createOneshot( 12 );  // one shot alarm that will be manually retriggered

  Serial.println("started");
}

void loop(){
 dtAlarms.waitUntilThisSecond(0); //  this code blocks waiting for start of the next minute, background alarms are still serviced 
 Serial.println("turning LED on");
 digitalWrite(13, HIGH);
 dtAlarms.delay(2000);  // note we call the alarm delay to service the background alarms 
 Serial.println("turning LED off");
 digitalWrite(13,LOW);  

}
