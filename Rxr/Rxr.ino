#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <nRF24L01.h>
#include <EEPROM.h>
#include <CmdMessenger.h>
#include "NewTimerOne.h"
#include "Settings.h"
#include "Console.h"
#include "util.h"
#include "constants.h"
#include "macros.h"
#include "motor.h"
#include "receiver.h"

Motor motor;
Console console;
Settings settings;
Receiver receiver;

void TimerISR() {
  motor.Run();
}
 
void setup() {
  Serial.begin(kSerialBaud);

  SET_MODE(SLEEP_PIN, OUT);
  SET_MODE(ENABLE_PIN, OUT);
  SET_MODE(MS1_PIN, OUT);
  SET_MODE(MS2_PIN, OUT);
  SET_MODE(STEP_PIN, OUT);
  SET_MODE(DIR_PIN, OUT);
  SET_MODE(ANT_CTRL1, OUT);
  SET_MODE(ANT_CTRL2, OUT);
 
  long accel = settings.GetAcceleration();
  long max_velocity = settings.GetMaxVelocity();
  char microsteps = settings.GetMicrosteps();

  motor.Configure(accel, max_velocity, microsteps);

  Timer1.initialize();
  Timer1.attachInterrupt(TimerISR, kPeriod);

  SLEEP_PIN(SET);
  ENABLE_PIN(CLR);
  ANT_CTRL1(SET);
  ANT_CTRL1(CLR);

  console.Init();
}
 
void loop() {
  motor.set_observed_position(receiver.Position(motor));
  console.Run();
}
