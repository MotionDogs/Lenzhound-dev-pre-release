#include "NewTimerOne.h"
#include "RollingAverager.h"
#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <nRF24L01.h>
#include "Settings.h"
#include <EEPROM.h>
#include <CmdMessenger.h>
#include "Console.h"

 
// pin macros
#define CLR(x,y)            ( PORT ## x&=(~(1<<y)) )
#define SET(x,y)            ( PORT ## x|=(1<<y) )
#define _BV(bit)            ( 1 << (bit) )
#define IN(x,y)             ( DDR ## x&=(~(1<<y)) )
#define OUT(x,y)            ( DDR ## x|=(1<<y) )
#define SET_MODE(pin,mode)  ( pin(mode) )

// antena pins
#define ANT_CTRL1(verb)     ( verb(F,0) )
#define ANT_CTRL2(verb)     ( verb(F,1) )
 
// easydriver pin macros
#define MS1_PIN(verb)       ( verb(D,2) )
#define MS2_PIN(verb)       ( verb(B,5) )
#define STEP_PIN(verb)      ( verb(D,6) )
#define DIR_PIN(verb)       ( verb(C,7) )
#define SLEEP_PIN(verb)     ( verb(D,3) )
#define ENABLE_PIN(verb)    ( verb(D,7) )
 
#define EIGHTH_STEPS    3
#define QUARTER_STEPS   2
#define HALF_STEPS      1
#define FULL_STEPS      0
 
// 22.10 fixed point macros
#define BITSHIFT        15
#define FIXED           long
#define MAKE_FIXED(a)   ((a)<<BITSHIFT)
#define FIXED_MULT(a,b) (((a) * (b)) >> BITSHIFT)
#define FIXED_DIV(a,b)  (((a) << BITSHIFT) / b)
#define FIXED_ONE       (MAKE_FIXED(1L))

// Serial constants
#define SERIAL_BITS_PER_SECOND 9600

#define USE_SERIAL_INPUT false

const char kMicrosteps          = EIGHTH_STEPS;
const int kLimiterArraySize     = (1 << 4);
const FIXED kMaxAccel           = 32L << kMicrosteps;
const FIXED kMinAccel           = 32L << kMicrosteps;
const FIXED kMaxVelocity        = 5000L << kMicrosteps;
const FIXED kAccelIndexShift    = 14;

// ISR constants
const long kSecondsInMicroseconds = 1000000L;
const long kIsrFrequency          = 6000L;
const long kPeriod                = kSecondsInMicroseconds/kIsrFrequency;

rollingaveragernamespace::RollingAverager time_averager;
rollingaveragernamespace::RollingAverager delta_averager;
FIXED isr_count = 0;
FIXED estimated_receiver_interval = MAKE_FIXED(10L);
FIXED decel = kMaxAccel / 2;
FIXED decel_denominator = FIXED_MULT(decel, MAKE_FIXED(2L));
FIXED velocity = 0L;
FIXED calculated_position = 0L;
bool direction = 1;
FIXED motor_position = 0L;
FIXED observed_position = 0L;
FIXED current_delta = 0L;
FIXED previous_target = 0L;

FIXED piecewise_accel [kLimiterArraySize] = { 0 };
FIXED velocity_limiter [kLimiterArraySize] = { 0 };

inline FIXED Max(FIXED a, FIXED b) {
  return (a > b) ? a : b;
}

inline FIXED Min(FIXED a, FIXED b) {
  return (a < b) ? a : b;
}

inline void SetMicrosteps(char microsteps) {
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

inline bool WeArePastDecelerationThreshold(FIXED steps_to_go) {
  return steps_to_go <= FIXED_DIV(
    FIXED_MULT(velocity,velocity),decel_denominator);
}

inline FIXED GetAccel(FIXED current_delta) {
  return piecewise_accel[Min(current_delta >> kAccelIndexShift, 
    kLimiterArraySize - 1)];
}

inline FIXED GetSpeedCap() {
  return current_delta / estimated_receiver_interval;
}

void InitializeLimiterArrays() {
  FIXED accel_range = kMaxAccel - kMinAccel;
  for (int i = 0; i < kLimiterArraySize; ++i) {
    piecewise_accel[i] = kMinAccel +
      accel_range * (double(i) / double(kLimiterArraySize));
  }
}

inline void ReadPosition() {
  long position = observed_position >> kMicrosteps;
  if (USE_SERIAL_INPUT) {
    if (Serial.available() > 0) {
      char test = Serial.read();
      if (test != 'a'){   
        position = MAKE_FIXED((long(test) - 96) * 1L); 
      }
    } 
  } else {
    if(Mirf.dataReady()){ // Got packet
      Mirf.getData((byte *) &position);
      position = MAKE_FIXED(position);
    }    
  }
  estimated_receiver_interval = time_averager.Roll(isr_count);
  previous_target = observed_position;
  observed_position = position << kMicrosteps;
  current_delta = delta_averager.Roll(Abs(observed_position-previous_target));
  isr_count = 0;
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
 
// Using an inline function for this because the arduino implementation
// is a macro (we don't want to recompute a)
inline FIXED Abs(FIXED a) {
  return a < 0 ? -a : a;
}
 
void Run() {
  isr_count++;
  //DetermineSleepState();
  FIXED steps_to_go = Abs(observed_position - calculated_position);
  if(direction){
    if(calculated_position > observed_position || 
      WeArePastDecelerationThreshold(steps_to_go) || 
      (steps_to_go < delta_averager.get_sum() && velocity > GetSpeedCap())) {
      velocity-=decel;
    } else if (calculated_position < observed_position) {
      velocity=Min(velocity+GetAccel(current_delta), kMaxVelocity);
    }
    calculated_position+=velocity;
    if(motor_position<calculated_position && 
      motor_position!=observed_position) {
      motor_position+=FIXED_ONE;
      Pulse();
    }
    if(velocity < 0)
      SetDirBackward();
  } else {
    if(calculated_position < observed_position || 
      WeArePastDecelerationThreshold(steps_to_go) || 
      (steps_to_go < delta_averager.get_sum() && (-velocity) > GetSpeedCap())){
      velocity+=decel;
    } else if (calculated_position > observed_position) {
      velocity=Max(velocity-GetAccel(current_delta), -kMaxVelocity);
    }
    calculated_position+=velocity;
    if(motor_position>calculated_position && 
      motor_position!=observed_position) {
      motor_position-=FIXED_ONE;
      Pulse();
    }
    if(velocity > 0)
      SetDirForward();
  }
}
 
inline void DetermineSleepState(){
  if (motor_position == observed_position && abs(velocity) <= decel) {
    SLEEP_PIN(CLR); // this sleeps
  } else {
    SLEEP_PIN(SET); // this wakes
  }
}

Console console;
 
void setup(){
  Serial.begin(SERIAL_BITS_PER_SECOND);
  if (USE_SERIAL_INPUT) {
    while(!Serial) {}
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
 
  Timer1.initialize();
  Timer1.attachInterrupt(Run, kPeriod);
 
  Mirf.spi = &MirfHardwareSpi; 
  Mirf.init(); // Setup pins / SPI
  Mirf.setRADDR((byte *)"serv1"); // Configure recieving address
  Mirf.payload = sizeof(observed_position); // Payload length
  Mirf.config(); // Power up reciver
 
  InitializeLimiterArrays();
 
  SetMicrosteps(kMicrosteps); 
  SLEEP_PIN(SET);
  ENABLE_PIN(CLR);
  ANT_CTRL1(SET);
  ANT_CTRL1(CLR);
 
  console.Init();
  Serial.println("exiting setup");
}
 
void loop() {
  ReadPosition();
}

