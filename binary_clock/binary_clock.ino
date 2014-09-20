#include <Time.h>
#include "definitions.h"

void resetTimeData(struct time_data timeData)
{
  timeData.hours = hour();
  timeData.minutes = minute();
  timeData.seconds = second();
}

bool clockStateChanged()
{
  int pinValue = digitalRead(stateChangeBtnPin);
  if (pinValue == HIGH)
  {
    switch (clockState)
    {
      case SHOW_TIME:
        clockState = SET_TIME;
        break;
      case SET_TIME:
        clockState = SET_ALARM;
        resetTimeData(changeClockTimeData);
        break;
      case SET_ALARM:
        clockState = SHOW_TIME;
        resetTimeData(changeAlarmTimeData);
        break;
    }
    return true;
  }

  return false;
}

bool alarmStateChanged()
{
  int pinValue = digitalRead(alarmBtnPin);
  if (pinValue == HIGH)
  {
    switch (alarmState)
    {
      case ALARM_SET:
        alarmState = ALARM_OFF;
        break;
      case ALARM_OFF:
        alarmState = ALARM_SET;
        break;
      case ALARM_RUNNING:
        alarmState = ALARM_OFF;
        noTone(buzzerPin);
        break;
    }
    return true;
  }
  return false;
}

void printTime()
{
  int h = hour();
  int m = minute();
  int s = second();

  Serial.print(h);
  Serial.print(":");
  Serial.print(m);
  Serial.print(":");
  Serial.print(s);
  Serial.print("\n");
}

void setup()
{
  Serial.begin(9600);

  resetTimeData(changeClockTimeData);
  resetTimeData(changeAlarmTimeData);

  pinMode(stateChangeBtnPin, INPUT);
  pinMode(hourBtnPin, INPUT);
  pinMode(minBtnPin, INPUT);
  pinMode(alarmBtnPin, INPUT);

  pinMode(buzzerPin, OUTPUT);
  pinMode(alarmLEDPin, OUTPUT);
}

void changeTimeData(struct time_data &timeData)
{
  if (digitalRead(hourBtnPin) == HIGH)
  {
    timeData.hours++;
    timeData.hours %= 24;
  }
  else if (digitalRead(minBtnPin) == HIGH)
  {
    timeData.minutes++;
    timeData.minutes %= 60;
  }
}

void setClockTime()
{
  changeTimeData(changeClockTimeData);
  setTime(changeClockTimeData.hours,
          changeClockTimeData.minutes,
          changeClockTimeData.seconds,
          day(),month(),year());
}

void setAlarmTime()
{
  changeTimeData(changeAlarmTimeData);
  alarmTime = changeAlarmTimeData;
}

void processClockState()
{
  switch(clockState)
  {
    case SHOW_TIME:
      break;
    case SET_TIME:
      setClockTime();
      break;
    case SET_ALARM:
      setAlarmTime();
      break;
  }
}

bool checkRunAlarm()
{
  return (alarmTime.hours == hour() &&
          alarmTime.minutes == minute() &&
          alarmTime.seconds == second());
}

void processAlarmState()
{
  switch(alarmState)
  {
    case (ALARM_SET):
      digitalWrite(alarmLEDPin, HIGH);
      if (checkRunAlarm())
      {
        alarmState = ALARM_RUNNING;
      }
      break;
    case (ALARM_RUNNING):
      digitalWrite(alarmLEDPin, HIGH);
      tone(buzzerPin, 500);
      break;
    case (ALARM_OFF):
      digitalWrite(alarmLEDPin, LOW);
      break;
  }
}

void loop()
{
  //check if the state of the clock has changed
  //& process the current state
  clockStateChanged();
  processClockState();

  //check if the state of the alarm has changed
  //& process the current state
  alarmStateChanged();
  processAlarmState();

  if (clockState != SET_ALARM)
  {
    printTime();
  }
  else
  {
    Serial.print(alarmTime.hours);
    Serial.print(":");
    Serial.print(alarmTime.minutes);
    Serial.print(":");
    Serial.print(alarmTime.seconds);
    Serial.print("\n");
  }
  // pause for a short amount of time.
  // this avoid changing the state multiple times.
  delay(150);
}
