#ifndef lenzhound_motor_controller_h
#define lenzhound_motor_controller_h
#include "motor.h"
#include "constants.h"

namespace lh {

class MotorController {
public:
  MotorController(Motor * motor);
  void Configure(long accel, long max_velocity);
  void Run();
  void set_observed_position(long position);
  void set_max_velocity(int velocity);
private:
  Motor * motor_;
  bool direction_;
  long max_velocity_;
  long current_velocity_cap_;
  long accel_;
  long decel_denominator_;
  long velocity_;
  long calculated_position_;
  long motor_position_;
  long observed_position_;
  long run_count_;
  long sleeping_;

  long GetDecelerationThreshold();
  bool TrySleep();
  void Sleep();
  void WakeUp();
};

}

#endif //lenzhound_motor_controller_h