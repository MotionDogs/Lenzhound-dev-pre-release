#ifndef rxr_motor_h
#define rxr_motor_h

class Motor {
public:
  long run_count;
  Motor();
  void Configure(long accel, long decel, long max_velocity);
  void Run();
  long observed_position() {
    return observed_position_;
  }
  void set_observed_position(long position) {
    observed_position_ = position;
  }
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
  }
private:
  bool direction_;
  long max_velocity_;
  long accel_;
  long decel_;
  long decel_denominator_;
  long velocity_;
  long calculated_position_;
  long motor_position_;
  long observed_position_;

  void Pulse();
  void SetDirForward();
  void SetDirBackward();
  long GetDecelerationThreshold();
};

#endif //rxr_motor_h
