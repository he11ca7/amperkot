#include <QuadDisplay2.h>
#include <TroykaRTC.h>
#include <TroykaDHT11.h>
#include <TroykaButton.h>
#include "melodies.h"

#define PIN_QD         10
#define PIN_BUZZER     8
#define PIN_DHT        0
#define PIN_BUTTON_S   A5
#define PIN_BUTTON_L   A4
#define PIN_BUTTON_V   2
#define PIN_BUTTON_G   7

#define TIMEUP_CLOCK       3000
#define TIMEUP_DHT         14000
#define TIMEUP_FLASH       3000

unsigned long
timeClock = 0,
timeDHT = 0,
timeFlash = 0;

int8_t setupMinutes = 0;

QuadDisplay qd(PIN_QD);
DHT11 dht(PIN_DHT);
RTC clk;
TroykaButton button(PIN_BUTTON_S, 3000);

enum states
{
  STATE_TIME,
  STATE_TEMPERATURE,
  STATE_HUMIDITY,
  STATE_SETUP
};
states state = STATE_TIME;

void setup()
{ 
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BUTTON_S, INPUT);
  pinMode(PIN_BUTTON_L, OUTPUT);
  pinMode(PIN_BUTTON_V, OUTPUT);
  pinMode(PIN_BUTTON_G, OUTPUT);
  digitalWrite(PIN_BUTTON_V, HIGH);
  digitalWrite(PIN_BUTTON_G, LOW);

  qd.begin();
  qd.displayClear();
  
  dht.begin();
  dht.read();
  
  clk.begin();
  // clk.set(10,25,45,27,07,2005,THURSDAY); // Вручную
  // clk.set(__TIMESTAMP__); // При компиляции
  clk.read();

  // melodyKeyboardCat(PIN_BUZZER, PIN_BUTTON_S, PIN_BUTTON_L);
  // melodyNyan(PIN_BUZZER, PIN_BUTTON_S, PIN_BUTTON_L);

  button.begin();
}

bool timedOut(
    unsigned long &timeStamp,
    const unsigned long timeOut)
{
  const unsigned long ms = millis();
  if(ms - timeStamp >= timeOut)
  {
    timeStamp = ms;
    return true;
  }
  else return false;
}

void loop()
{
  if(timedOut(timeClock, TIMEUP_CLOCK))
    clk.read();

  if(timedOut(timeDHT, TIMEUP_DHT))
    dht.read();

  button.read();
  if(button.justPressed())
  {
    if(state == STATE_SETUP)
    {
      ++setupMinutes;
      setupMinutes = setupMinutes % 60;
      tone(PIN_BUZZER, NOTE_A4, 100);
    }
    else
    {
      switch (state)
      {
        case STATE_TIME: state = STATE_TEMPERATURE; break;
        case STATE_TEMPERATURE: state = STATE_HUMIDITY; break;
        case STATE_HUMIDITY: state = STATE_TIME; break;
      }
      tone(PIN_BUZZER, NOTE_A4, 100);
    }
  }
  else if(button.isHold())
  {
    state = (state == STATE_SETUP ? STATE_TIME : STATE_SETUP);
    if(state == STATE_TIME)
    {
      tone(PIN_BUZZER, NOTE_A4, 100);
      delay(88);
      tone(PIN_BUZZER, NOTE_G4, 100);

      clk.setMinute(setupMinutes);
    }
    else if(state == STATE_SETUP)
    {
      tone(PIN_BUZZER, NOTE_G4, 100);
      delay(88);
      tone(PIN_BUZZER, NOTE_A4, 100);

      clk.read();
      setupMinutes = clk.getMinute();
    }
  }

  switch (state)
  {
    case STATE_TIME:
    {
      qd.displayInt(clk.getHour() * 100 + clk.getMinute(), true);
      break;
    }
    case STATE_TEMPERATURE:
    {
      qd.displayTemperatureC(dht.getTemperatureC());
      break;
    }
    case STATE_HUMIDITY:
    {
      qd.displayHumidity(dht.getHumidity());
      break;
    }
    case STATE_SETUP:
    {
      qd.displayInt(setupMinutes);
      break;
    }
  }
}
