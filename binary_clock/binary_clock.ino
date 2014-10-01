#include <binary.h>
#include <Sprite.h>
#include <Matrix.h>
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
        changeClockTimeData = getCurrentTime();
        break;
      case SET_TIME:
        clockState = SET_ALARM;
        resetTimeData(changeClockTimeData);
        break;
      case SET_ALARM:
        clockState = SHOW_TIME;
        alarmTime = changeAlarmTimeData;
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

void printTimeToSerial(time_data_t timeData)
{
  Serial.print(timeData.hours);
  Serial.print(":");
  Serial.print(timeData.minutes);
  Serial.print(":");
  Serial.print(timeData.seconds);
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

  noTone(buzzerPin);

  leds.setBrightness(5);
  leds.clear();
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
  showTime(changeClockTimeData);
  changeTimeData(changeClockTimeData);
  setTime(changeClockTimeData.hours,
          changeClockTimeData.minutes,
          changeClockTimeData.seconds,
          day(),month(),year());
}

void setAlarmTime()
{
  showTime(changeAlarmTimeData);
  changeTimeData(changeAlarmTimeData);
}

void processClockState()
{
  switch(clockState)
  {
    case SHOW_TIME:
      showCurrentTime();
      break;
    case SET_TIME:
      setClockTime();
      break;
    case SET_ALARM:
      setAlarmTime();
      break;
  }
}

void showCurrentTime()
{
    time_data_t currentTime = getCurrentTime();
    showTime(currentTime);
}

void showTime(time_data_t timeData)
{
    printTimeToSerial(timeData);
    displayTime(timeData);
}

bool checkRunAlarm()
{
  return (alarmTime.hours == hour() &&
          alarmTime.minutes == minute() &&
          alarmTime.seconds == second());
}

time_data_t getCurrentTime()
{
    time_data_t currentTime;
    currentTime.seconds = second();
    currentTime.minutes = minute();
    currentTime.hours = hour();
    return currentTime;
}

void alarmPending()
{
    digitalWrite(alarmLEDPin, HIGH);
    if (checkRunAlarm())
    {
      alarmState = ALARM_RUNNING;
    }
}

void alarmRunning()
{
    digitalWrite(alarmLEDPin, HIGH);
    tone(buzzerPin, buzzerTone);
}

void alarmOff()
{
    digitalWrite(alarmLEDPin, LOW);
}

void processAlarmState()
{
  switch(alarmState)
  {
    case (ALARM_SET):
      alarmPending();
      break;
    case (ALARM_RUNNING):
      alarmRunning();
      break;
    case (ALARM_OFF):
      alarmOff();
      break;
  }
}

/*
 * Show a single binary digit on the LED matrix
 *
 * @param digit :: the digit to convert to binary.
 * @param column :: the column of the LED matrix to display it on
 */
void showBinaryDigit(int digit, int column)
{
    int i = 0;
    while (digit > 0)
    {
        if (digit % 2 == 1)
        {
            leds.write((8-i)%8, column, HIGH);
        }
        ++i;
        digit = digit / 2;
    }
}

/*
 * Display some time data on the LED matrix
 *
 * @param timeData :: the time data to output to the LED matrix
 */
void displayTime(struct time_data timeData)
{
  leds.clear();

  const int numberOfTimeComponents = 3;
  const int timeComponents[3] = {timeData.seconds, timeData.minutes, timeData.hours};

  int column = 0;
  for (int i=0; i<numberOfTimeComponents; ++i)
  {
    int first_digit = timeComponents[i] % 10;
    int second_digit = timeComponents[i] / 10;

    showBinaryDigit(first_digit, column);
    showBinaryDigit(second_digit, column+1);

    column+=2;
  }
}

void loop()
{
  //check if the state of the clock has changed
  //& process the current state
  clockStateChanged();
  processClockState();

  //check if the state of the alarm (on/off) has changed
  //& process the current state
  alarmStateChanged();
  processAlarmState();

  // pause for a short amount of time.
  // this avoid changing the state multiple times.
  delay(120);
}
