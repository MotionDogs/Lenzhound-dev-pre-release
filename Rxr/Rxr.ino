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

#define USE_SERIAL_INPUT false

struct Packet {
  long position;
  char velocity;
  char mode;
};

long max_velocity;
long accel;
long decel = accel / 2;
long decel_denominator = util::FixedMultiply(decel, util::MakeFixed(2L));
long velocity = 0L;
long calculated_position = 0L;
bool direction = 1;
long motor_position = 0L;
long observed_position = 0L;
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

inline bool WeArePastDecelerationThreshold(long steps_to_go) {
  return steps_to_go <= util::FixedDivide(
    util::FixedMultiply(velocity,velocity),decel_denominator);
}

inline void ReadPosition() {
  Packet packet;
  packet.position = observed_position >> kMicrosteps;
  if (USE_SERIAL_INPUT) {
    if (Serial.available() > 0) {
      char test = Serial.read();
      if (test != 'a'){   
        packet.position = util::MakeFixed((long(test) - 96) * 1L); 
      }
    } 
  } else {
    if(Mirf.dataReady()){ // Got packet
      Mirf.getData((byte *) &packet);
      packet.position = util::MakeFixed(packet.position);
    }    
  }
  observed_position = packet.position << kMicrosteps;
}
 
inline void Pulse() {
  STEP_PIN(SET); 
  STEP_PIN(CLR);
}
 
inline void SetDirForward() {
  DIR_PIN(SET); 
  direction = 1;
}
 
inline void SetDirBackward() {
  DIR_PIN(CLR); 
  direction = 0;
}
 
void Run() {
  //DetermineSleepState();
  long steps_to_go = util::Abs(observed_position - calculated_position);
  if(direction) {
    if(calculated_position > observed_position || 
      WeArePastDecelerationThreshold(steps_to_go)) {
      velocity -= decel;
    } else if (calculated_position < observed_position) {
      velocity = util::Min(velocity+accel, max_velocity);
    }
    calculated_position += velocity;
    if(motor_position < calculated_position && 
      motor_position != observed_position) {
      motor_position += util::kFixedOne;
      Pulse();
    }
    if(velocity < 0)
      SetDirBackward();
  } else {
    if(calculated_position < observed_position || 
      WeArePastDecelerationThreshold(steps_to_go)){
      velocity += decel;
    } else if (calculated_position > observed_position) {
      velocity = util::Max(velocity-accel, -max_velocity);
    }
    calculated_position += velocity;
    if(motor_position > calculated_position && 
      motor_position != observed_position) {
      motor_position -= util::kFixedOne;
      Pulse();
    }
    if(velocity > 0)
      SetDirForward();
  }
}
 
inline void DetermineSleepState(){
  if (motor_position == observed_position && util::Abs(velocity) <= decel) {
    SLEEP_PIN(CLR); // this sleeps
  } else {
    SLEEP_PIN(SET); // this wakes
  }
}

void LoadSettings() {
  max_velocity = settings.GetMaxVelocity() << kMicrosteps;
  accel = settings.GetAcceleration() << kMicrosteps;
  decel = settings.GetDeceleration() << kMicrosteps;
}
 
void setup() {
  Serial.begin(kSerialBaud);

  LoadSettings();
  
  SET_MODE(SLEEP_PIN, OUT);
  SET_MODE(ENABLE_PIN, OUT);
  SET_MODE(MS1_PIN, OUT);
  SET_MODE(MS2_PIN, OUT);
  SET_MODE(STEP_PIN, OUT);
  SET_MODE(DIR_PIN, OUT);
  SET_MODE(ANT_CTRL1, OUT);
  SET_MODE(ANT_CTRL2, OUT);
 
  Timer1.initialize();
  Timer1.attachInterrupt(Run, kPeriod);
 
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

  console.Init();
}
 
void loop() {
  ReadPosition();
  console.Run();
}
