/*
 TimerAlarms.h - Arduino Timer Alarm library

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

*/

#ifndef TimerAlarms_h
#define TimerAlarms_h

#include <inttypes.h>
#include <wiring_private.h> // for boolean

#include "DateTime.h" 

typedef enum {
    dtMillisecond, dtSecond, dtMinute, dtHour, dtDay //,clkMonth,clkYear
} dtUnits_t;

typedef struct  {
     uint8_t isEnabled              :1 ; // alarm will only fire if true
     uint8_t isOneShot              :1 ; // alarm will fire once only if true
     uint8_t isAlarm                :1 ; // alarm based on absolute time, not used this version
  }
    AlarmMode_t   ;

typedef uint8_t AlarmID_t;
#define INVALID_ALARM_ID 255
#define NBR_ALARMS 6 // this sets the number of alarms, each alarm uses 12 bytes of RAM

class AlarmClass;  // forward reference
typedef void (*OnTick_t)(AlarmID_t);  // alarm callback function typedef 
extern void onAlarm(AlarmID_t);  // User supplied callback for all alarms

// class defining an alarm instance, only used by dtAlarmsClass
class AlarmClass
{      
private:
     friend class dtAlarmsClass;
     void updateNextTrigger();
     time_t value;
     time_t nextTrigger;
     AlarmID_t ID;           // unique instance id 
     AlarmMode_t Mode;
     AlarmClass();
public:
};

// class containing the collection of alarms
class dtAlarmsClass
{
private:
  AlarmClass Alarm[NBR_ALARMS];
  boolean isServicing;
  AlarmID_t nextID;
  AlarmID_t create( time_t value, boolean isOneShot, boolean isEnabled );
  void serviceAlarms();

public:
     dtAlarmsClass();
     void delay(unsigned long ms);
     uint8_t getDigitsNow( dtUnits_t Units);  // returns the current digit value for the given time unit
     void waitForDigits( uint8_t Digits, dtUnits_t Units);
     void waitForRollover(dtUnits_t Units);
   AlarmID_t createOneshot( time_t value, boolean isEnabled = true);
   AlarmID_t createRepeating( time_t value, boolean isEnabled = true);
     void setValue(AlarmID_t ID, time_t value);
     time_t getValue(AlarmID_t ID);
     void enable(AlarmID_t ID);
     void disable(AlarmID_t ID);

};

extern dtAlarmsClass dtAlarms;  // make an instance for the user

/*==============================================================================
* MACROS
*============================================================================*/

/* public */
#define waitUntilThisSecond(_val_) waitForDigits( _val_, dtSecond)
#define waitUntilThisMinute(_val_) waitForDigits( _val_, dtMinute)
#define waitUntilThisHour(_val_)   waitForDigits( _val_, dtHour)
#define waitUntilThisDay(_val_)    waitForDigits( _val_, dtDay)
#define waitMinuteRollover() waitForRollover(dtSecond)
#define waitHourRollover()   waitForRollover(dtMinute)
#define waitDayRollover()    waitForRollover(dtHour)

#define AlarmHMS(_hr_, _min_, _sec_) (_hr_ * SECS_PER_HOUR + _min_ * SECS_PER_MIN + _sec_)


#endif 