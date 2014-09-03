#include "NewTimerOne.h"
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
 
// higher order stepping helpers
#define EIGHTH_STEPS()  do { MS1_PIN(SET); MS2_PIN(SET); } while (0)
#define QUARTER_STEPS() do { MS1_PIN(CLR); MS2_PIN(SET); } while (0)
#define HALF_STEPS()    do { MS1_PIN(SET); MS2_PIN(CLR); } while (0)
#define FULL_STEPS()    do { MS1_PIN(CLR); MS2_PIN(CLR); } while (0)
 
// 22.10 fixed point macros
#define BITSHIFT  10
#define FIXED long
#define MAKE_FIXED(a) ((a)<<BITSHIFT)
#define FIXED_MULT(a,b) (((a) * (b)) >> BITSHIFT)
#define FIXED_DIV(a,b) (((a) << BITSHIFT) / b)
#define FIXED_ONE (MAKE_FIXED(1L))
 
// ISR constants
#define ISR_FREQUENCY           20000
#define SECOND_IN_MICROSECONDS  1000000
#define PERIOD                  SECOND_IN_MICROSECONDS/ISR_FREQUENCY
#define PERIOD_IN_MILLISECONDS  PERIOD/1000
 
// Serial constants
#define SERIAL_BITS_PER_SECOND 9600
 
#define LIMITER_LENGTH 500
 
FIXED accel = 1L;
FIXED decel = 3L;
FIXED decel_denominator = FIXED_MULT(decel, MAKE_FIXED(2));
FIXED velocity = 0L;
FIXED calculated_position = 0L;
bool direction = 1;
FIXED motor_position = 0L;
FIXED observed_position = 0L;
 
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
 
FIXED pos = 0;
inline FIXED getPosition() {
  return pos;
}
 
void tryStep() {
  observed_position = getPosition();
  FIXED stepsToGo = Abs(observed_position - calculated_position);
  if(direction){
    if(calculated_position<observed_position){
      if(stepsToGo>
        FIXED_DIV(FIXED_MULT(velocity,velocity),decel_denominator))
        velocity=min(velocity+accel,FIXED_ONE);
      else
        velocity-=decel;
    } else if(calculated_position!=observed_position) {
      velocity=velocity-decel;   
    }
    calculated_position+=velocity;
    if(motor_position<calculated_position && motor_position!=observed_position){
      motor_position+=FIXED_ONE;
      Pulse();
    }
    if(velocity < 0)
      SetDirBackward();
  } else {
    if(calculated_position>observed_position){
      if(stepsToGo>
        FIXED_DIV(FIXED_MULT(velocity,velocity),decel_denominator))
        velocity=max(velocity-accel, -FIXED_ONE);
      else
        velocity+=decel;
    } else if(calculated_position!=observed_position) {
      velocity=velocity+decel;
    }
    calculated_position+=velocity;
    if(motor_position>calculated_position && motor_position!=observed_position){
      motor_position-=FIXED_ONE;
      Pulse();
    }
    if(velocity > 0)
      SetDirForward();
  }
}
 
inline void trySleep(){
  static int counter = 0;
  if (motor_position == observed_position){
    if (counter == 500){
      SLEEP_PIN(SET);
    } else {
      counter++;
    }
  }
  else{
    counter = 0;
  }
}

Console console;
 
void setup(){
  SET_MODE(SLEEP_PIN, OUT);
  SET_MODE(ENABLE_PIN, OUT);
  SET_MODE(MS1_PIN, OUT);
  SET_MODE(MS2_PIN, OUT);
  SET_MODE(STEP_PIN, OUT);
  SET_MODE(DIR_PIN, OUT);
  SET_MODE(ANT_CTRL1, OUT);
  SET_MODE(ANT_CTRL2, OUT);
 
  Timer1.initialize();
  Timer1.attachInterrupt(tryStep, PERIOD);
 
  Serial.begin(SERIAL_BITS_PER_SECOND);
 
  Mirf.spi = &MirfHardwareSpi; 
  Mirf.init(); // Setup pins / SPI
  Mirf.setRADDR((byte *)"serv1"); // Configure recieving address
  Mirf.payload = sizeof(pos); // Payload length
  Mirf.config(); // Power up reciver
 
 
  EIGHTH_STEPS(); 
  SLEEP_PIN(SET);
  ENABLE_PIN(CLR);
  ANT_CTRL1(SET);
  ANT_CTRL1(CLR);
 
  console.Init();
  Serial.println("exiting setup");
}
 
void loop(){
   //if (Serial.available() > 0) {
   //  //SLEEP_PIN(CLR);
   //  char test = Serial.read();
   //  if (test != 'a'){   
   //    pos = MAKE_FIXED((long(test) - 96) * 500L); 
   //  }
   //  Serial.println(velocity);
   //}
   //trySleep(); 
   
  if(Mirf.dataReady()){ // Got packet
    long position1 = 0;
    Mirf.getData((byte *) &position1);
    pos = MAKE_FIXED(position1);
  }
  
  console.Run();
}
