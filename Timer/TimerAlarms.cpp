/*
 TimerAlarms.cpp - Arduino Timer Alarm library
 Copyright (c) Michael Margolis.  All right reserved.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*/

extern "C" {
#include <string.h> // for memset
}

#include "DateTime.h"
#include "TimerAlarms.h"

//**************************************************************
//* Alarm Class Constructor

AlarmClass::AlarmClass(){
 Mode.isAlarm =  Mode.isEnabled = Mode.isOneShot = 0;
 value = nextTrigger = 0;  
}

//**************************************************************
//* Private Methods

void AlarmClass::updateNextTrigger(){
 if( (value != 0) && (Mode.isEnabled != 0) ){
   time_t time = DateTime.now();
   nextTrigger = time + value;  // add the value to previous time (this ensures delay always at least Value seconds)    
 }
 else {
   Mode.isEnabled = 0;  // Disable if the value is 0
 }
}

//**************************************************************
//* Date Time Alarms Public Methods

dtAlarmsClass::dtAlarmsClass(){
 nextID = 0;
 isServicing = false;
}

AlarmID_t dtAlarmsClass::createOneshot( time_t value, boolean isEnabled){
 return create( value, true, isEnabled );
}

AlarmID_t dtAlarmsClass::createRepeating( time_t value, boolean isEnabled){
  return create( value, false, isEnabled );
}

void dtAlarmsClass::enable(AlarmID_t ID){
 if(ID < NBR_ALARMS){
   Alarm[ID].Mode.isEnabled = (Alarm[ID].value != 0) ;  // only enable if value is non zero
   Alarm[ID].updateNextTrigger(); // trigger is updated whenever  this is called, even if already enabled       
 }
}

void dtAlarmsClass::disable(AlarmID_t ID)
{
 if(ID < NBR_ALARMS)
   Alarm[ID].Mode.isEnabled = false;
}

void dtAlarmsClass::setValue(AlarmID_t ID, time_t value){
 if(ID < NBR_ALARMS){
   Alarm[ID].value = value;
   enable(ID);
 }
}

time_t dtAlarmsClass::getValue(AlarmID_t ID){
 if(ID < NBR_ALARMS)
   return Alarm[ID].value;
 else 
   return 0;
}

// following functions are not Alarm ID specific.
void dtAlarmsClass::delay(unsigned long ms)
{
 unsigned long endtime = millis() + ms;
 boolean Overflow = endtime < millis();
 if( Overflow){
   while( millis() > endtime)
     serviceAlarms();
 }
 while( millis() < endtime)
   serviceAlarms();
}

void dtAlarmsClass::waitForDigits( uint8_t Digits, dtUnits_t Units){
 while(Digits != getDigitsNow(Units) )  {
   serviceAlarms();
 }
}

void dtAlarmsClass::waitForRollover( dtUnits_t Units){
 while(getDigitsNow(Units) == 0  ) // if its just rolled over than wait for another rollover                                  
   serviceAlarms();
 waitForDigits(0, Units);
}

uint8_t dtAlarmsClass::getDigitsNow( dtUnits_t Units){
 time_t time = DateTime.now();
 if(Units == dtSecond) return numberOfSeconds(time);
 if(Units == dtMinute) return numberOfMinutes(time); 
 if(Units == dtHour) return numberOfHours(time);
 if(Units == dtDay) return dayOfWeek(time);
 return 255;  // This should never happen 
}

//***********************************************************
//* Private Methods

void dtAlarmsClass::serviceAlarms(){
 if(! isServicing)
 {
   isServicing = true;
   for(uint8_t i = 0; i < NBR_ALARMS; i++)
   {
     if( Alarm[i].Mode.isEnabled && (DateTime.now() >= Alarm[i].nextTrigger)  )
     {
             if( Alarm[i].Mode.isOneShot == 1 ){
                Alarm[i].Mode.isEnabled = 0;  // this prevents the oneshot from re-triggering unless its explicitly re-armed
             } 
       Alarm[i].updateNextTrigger();
       if( onAlarm != NULL) {                        
               onAlarm(i);
       }
     }
   }
   isServicing = false;
 }
}

AlarmID_t dtAlarmsClass::create( time_t value, boolean isOneShot, boolean isEnabled ){  // returns true if has been registerd ok
 AlarmID_t id = INVALID_ALARM_ID;
 if( nextID < NBR_ALARMS){
     id = nextID;
   Alarm[id].Mode.isOneShot = isOneShot;
     Alarm[id].value = value;
     isEnabled ?  enable(id) : disable(id);   
   nextID++;   
 }
 return id;
}

// make one instance for the user to use
dtAlarmsClass dtAlarms = dtAlarmsClass() ;