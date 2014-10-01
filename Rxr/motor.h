#ifndef rxr_motor_h
#define rxr_motor_h
#include "Arduino.h"
#include "constants.h"

#define EIGHTH_STEPS  3
#define QUARTER_STEPS 2
#define HALF_STEPS    1
#define FULL_STEPS    0

class Motor {
public:
  Motor();
  void Configure(long accel, long max_velocity, char microsteps);
  void Run();
  long observed_position();
  void set_observed_position(long position);
  void inspect() {
    Serial.print("direction_: ");
    Serial.println(direction_);
    Serial.print("velocity_:");
    Serial.println(velocity_);
    Serial.print("calculated_position_:");
    Serial.println(calculated_position_);
    Serial.print("motor_position_:");
    Serial.println(motor_position_);
    Serial.print("observed_position_:");
    Serial.println(observed_position_);
    Serial.print("accel_:");
    Serial.println(accel_);
    Serial.print("decel_:");
    Serial.println(decel_);
    Serial.print("decel_denominator_:");
    Serial.println(decel_denominator_);
    Serial.print("max_velocity_:");
    Serial.println(max_velocity_);
    Serial.print("microsteps_:");
    Serial.println(microsteps_);
  }
private:
  char microsteps_;
  bool direction_;
  long max_velocity_;
  long accel_;
  long decel_;
  long decel_denominator_;
  long velocity_;
  long calculated_position_;
  long motor_position_;
  long observed_position_;
  long run_count_;
  long sleeping_;

  void Pulse();
  void SetDirForward();
  void SetDirBackward();
  long GetDecelerationThreshold();
  bool TrySleep();
  void Sleep();
  void WakeUp();
};

#endif //rxr_motor_h
