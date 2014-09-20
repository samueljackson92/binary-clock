/*
    Definitions.h
    
    Definitions of various data structures and typdefs used in the code.
*/

// constant definitions for the input digital pins used.
const int stateChangeBtnPin = 2;
const int hourBtnPin = 3;
const int minBtnPin = 4;
const int alarmBtnPin = 5;

//constant definitions for the output digital pin used.
const int buzzerPin = 6;
const int alarmLEDPin = 7;

//enum definition to hold the current state of the alarm
typedef enum {ALARM_OFF, ALARM_SET, ALARM_RUNNING} alarm_state_t;
alarm_state_t alarmState = ALARM_OFF;

//enum definition to hold the current mode of the clock
typedef enum {SHOW_TIME, SET_TIME, SET_ALARM} clock_state_t;
clock_state_t clockState = SHOW_TIME;

//convenience structure to hold hours, minutes and seconds
typedef struct time_data
{
  int hours;
  int minutes;
  int seconds;
} time_data_t;

//struct for changing the current time of the clock
time_data_t changeClockTimeData;
//struct for changing the time of the alarm
time_data_t changeAlarmTimeData;
//struct to hold the current alarm time
time_data_t alarmTime;
