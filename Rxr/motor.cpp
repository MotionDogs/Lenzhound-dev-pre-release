#include "motor.h"
#include "macros.h"
#include "constants.h"
#include "util.h"

#define PERCENT_CONVERSION_FACTOR .01

Motor::Motor() :
direction_(1),
calculated_position_(0),
observed_position_(0),
motor_position_(0),
run_count_(0),
sleeping_(false),
velocity_(0) {
}

void Motor::set_observed_position(long position) {
  long new_position = position >> (3 - microsteps_);
  if (new_position != observed_position_) {
    WakeUp();
    run_count_ = 0;
  }
  observed_position_ = new_position;
}

void Motor::set_max_velocity(int velocity) {
  current_velocity_cap_ = max(max_velocity_ * velocity * PERCENT_CONVERSION_FACTOR, 1L);
}

void Motor::Configure(
  long accel, long max_velocity, char microsteps) {
  char microstep_difference = microsteps - microsteps_;
  microsteps_ = microsteps;
  accel_ = accel << microsteps_;
  max_velocity_ = max_velocity << microsteps_;
  decel_ = accel_;
  decel_denominator_ = util::FixedMultiply(decel_, util::MakeFixed(2L));

  // negative shifting is undefined behavior in c++
  if (microstep_difference >= 0) {
    observed_position_ <<= microstep_difference;
    calculated_position_ <<= microstep_difference;
    motor_position_ <<= microstep_difference;
    velocity_ <<= microstep_difference;
  } else {
    observed_position_ >>= -microstep_difference;
    calculated_position_ >>= -microstep_difference;
    motor_position_ >>= -microstep_difference;
    velocity_ >>= -microstep_difference;
  }

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

void Motor::Sleep() {
  sleeping_ = true;
  run_count_ = 0;
  SLEEP_PIN(CLR);
}

void Motor::WakeUp() {
  sleeping_ = false;
  SLEEP_PIN(SET);
}

bool Motor::TrySleep() {
  if (motor_position_ != observed_position_) {
    run_count_ = 0;
    return false;
  }
  if (sleeping_) {
    return true;
  }
  if (run_count_ > kSleepThreshold) {
    Sleep();
    return true;
  }
  run_count_++;
  return false;
}

void Motor::Run() {
  if (TrySleep()) {
    return;
  }
  long steps_to_go = util::Abs(observed_position_ - calculated_position_);
  if(direction_) {
    if((calculated_position_ > observed_position_) ||
      (steps_to_go <= GetDecelerationThreshold())) {
      velocity_ -= decel_;
    } else if (calculated_position_ < observed_position_) {
      velocity_ = util::Min(velocity_+accel_, current_velocity_cap_);
    }
    if (velocity_ < decel_) {
      calculated_position_ = observed_position_;
    } else {
      calculated_position_ += velocity_;
    }
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
      velocity_ = util::Max(velocity_-accel_, -current_velocity_cap_);
    }
    if (-velocity_ < decel_) {
      calculated_position_ = observed_position_;
    } else {
      calculated_position_ += velocity_;
    }
    if(motor_position_ > calculated_position_ &&
      motor_position_ != observed_position_) {
      motor_position_ -= util::kFixedOne;
      Pulse();
    }
    if(velocity_ > 0)
      SetDirForward();
  }
}
