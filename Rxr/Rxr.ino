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

#define USE_SERIAL_INPUT false

struct Packet {
  long position;
  int velocity;
  int acceleration;
  char mode;
};

Motor motor;
Console console;
Settings settings;

inline void SetMicrosteps(MicrostepInterval microsteps) {
  switch (microsteps) {
    case FULL_STEPS: {
      MS1_PIN(CLR); 
      MS2_PIN(CLR);
      break;
    }
    case HALF_STEPS: {
      MS1_PIN(SET); 
      MS2_PIN(CLR);
      break;
    }
    case QUARTER_STEPS: {
      MS1_PIN(CLR); 
      MS2_PIN(SET);
      break;
    }
    case EIGHTH_STEPS: {
      MS1_PIN(SET); 
      MS2_PIN(SET);
      break;
    }
    default: {
      Serial.println("Bad microstep value.");
    }
  }
}

inline void ReadPosition() {
  Packet packet;
  long observed_position = motor.observed_position();
  packet.position = observed_position >> kMicrosteps;
  if (USE_SERIAL_INPUT) {
    if (Serial.available() > 0) {
      char test = Serial.read();
      if (test != 'a'){   
        packet.position = util::MakeFixed((long(test) - 96) * 40L); 
      }
      Serial.println(motor.run_count);
    } 
  } else {
    if(Mirf.dataReady()){ // Got packet
      Mirf.getData((byte *) &packet);
      packet.position = util::MakeFixed(packet.position);
    }    
  }
  motor.set_observed_position(packet.position << kMicrosteps);
}

void TimerISR() {
  motor.Run();
}
 
void setup() {
  Serial.begin(kSerialBaud);
  if (USE_SERIAL_INPUT) {
    while(!Serial){}
  }
  Serial.println("entering setup");

  SET_MODE(SLEEP_PIN, OUT);
  SET_MODE(ENABLE_PIN, OUT);
  SET_MODE(MS1_PIN, OUT);
  SET_MODE(MS2_PIN, OUT);
  SET_MODE(STEP_PIN, OUT);
  SET_MODE(DIR_PIN, OUT);
  SET_MODE(ANT_CTRL1, OUT);
  SET_MODE(ANT_CTRL2, OUT);
 
  long accel = settings.GetAcceleration() << kMicrosteps;
  long decel = settings.GetDeceleration() << kMicrosteps;
  long max_velocity = settings.GetMaxVelocity() << kMicrosteps;
  motor.Configure(accel, decel, max_velocity);

  Timer1.initialize();
  Timer1.attachInterrupt(TimerISR, kPeriod);
 
  Mirf.spi = &MirfHardwareSpi; 
  Mirf.init(); // Setup pins / SPI
  Mirf.setRADDR((byte *)"serv1"); // Configure recieving address
  Mirf.payload = sizeof(Packet); // Payload length
  Mirf.config(); // Power up reciver
 
  SetMicrosteps(kMicrosteps); 
  SLEEP_PIN(SET);
  ENABLE_PIN(CLR);
  ANT_CTRL1(SET);
  ANT_CTRL1(CLR);
  Serial.println(accel);
  Serial.println(decel);
  Serial.println(max_velocity);

  console.Init();
  Serial.println("exiting setup");
}
 
void loop() {
  ReadPosition();
  if (!USE_SERIAL_INPUT) {
    console.Run();
  }
}
