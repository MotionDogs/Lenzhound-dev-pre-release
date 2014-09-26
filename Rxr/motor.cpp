#include "motor.h"
#include "macros.h"
#include "constants.h"
#include "util.h"

Motor::Motor() :
direction_(1),
calculated_position_(0),
observed_position_(0),
motor_position_(0),
velocity_(0) {
}

long Motor::observed_position(){
  return observed_position_;
}

void Motor::set_observed_position(long position) {
  observed_position_ = position << microsteps_;
}

void Motor::Configure(
  long accel, long max_velocity, char microsteps) {
  microsteps = 3;
  microsteps_ = microsteps;
  accel_ = accel << microsteps_;
  max_velocity_ = max_velocity << microsteps_;
  decel_ = accel_;
  decel_denominator_ = util::FixedMultiply(decel_, util::MakeFixed(2L));
  switch (microsteps_) {
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
  }
}

void Motor::Pulse() {
  STEP_PIN(SET);
  STEP_PIN(CLR);
}

void Motor::SetDirForward() {
  DIR_PIN(SET);
  direction_ = 1;
}

void Motor::SetDirBackward() {
  DIR_PIN(CLR);
  direction_ = 0;
}

long Motor::GetDecelerationThreshold() {
  return util::FixedDivide(
    util::FixedMultiply(velocity_,velocity_),decel_denominator_);
}

void Motor::Run() {
  long steps_to_go = util::Abs(observed_position_ - calculated_position_);
  if(direction_) {
    if((calculated_position_ > observed_position_) ||
      (steps_to_go <= GetDecelerationThreshold())) {
      velocity_ -= decel_;
    } else if (calculated_position_ < observed_position_) {
      velocity_ = util::Min(velocity_+accel_, max_velocity_);
    }
    calculated_position_ += velocity_;
    if((motor_position_ < calculated_position_) &&
      (motor_position_ != observed_position_)) {
      motor_position_ += util::kFixedOne;
      Pulse();
    }
    if(velocity_ < 0)
      SetDirBackward();
  } else {
    if((calculated_position_ < observed_position_) ||
      (steps_to_go <= GetDecelerationThreshold())){
      velocity_ += decel_;
    } else if (calculated_position_ > observed_position_) {
      velocity_ = util::Max(velocity_-accel_, -max_velocity_);
    }
    calculated_position_ += velocity_;
    if(motor_position_ > calculated_position_ &&
      motor_position_ != observed_position_) {
      motor_position_ -= util::kFixedOne;
      Pulse();
    }
    if(velocity_ > 0)
      SetDirForward();
  }
}