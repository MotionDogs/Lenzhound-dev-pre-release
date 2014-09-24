#include "macros.h"
#include "constants.h"
#include "util.h"
#include "motor.h"

Motor::Motor() :
direction_(1),
calculated_position_(0),
observed_position_(0),
motor_position_(0),
velocity_(0) {
}

void Motor::Configure(long accel, long decel, long max_velocity) {
  accel_ = accel;
  max_velocity_ = max_velocity;
  decel_ = accel / 2;
  decel_denominator_ = util::FixedMultiply(decel_, util::MakeFixed(2L));
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
