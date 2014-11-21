#include "gtest/gtest.h"
#include "motorcontroller.h"
#include "util.h"

class MotorMock: public lh::Motor {
public:
  MotorMock() : position_(0), direction_(1) {
  }

  void set_boundary(long value) {
    boundary_ = value;
  }
  long position() {
    return position_;
  }
  virtual void Pulse() {
    position_ += direction_;
    ASSERT_LE(position_, boundary_);
  }
  virtual void SetDirForward() {
    direction_ = 1;
  }
  virtual void SetDirBackward() {
    direction_ = -1;
  }
  virtual void Sleep() {
  }
  virtual void WakeUp() {
  }
private:
  long boundary_;
  long position_;
  long direction_;
};

TEST(MotorController, HitsItsTarget) {
  MotorMock mock;
  lh::MotorController controller = lh::MotorController(&mock);
  long target = 5000;

  controller.Configure(50, 6000, 50, 6000);
  controller.set_max_velocity(100, 0);
  controller.set_observed_position(util::MakeFixed(target));

  // make sure we don't go over target
  mock.set_boundary(target);
  for (int i = 0; i < 35000; ++i) {
    controller.Run();
  }
  EXPECT_EQ(mock.position(),target);
}

TEST(MotorController, HandlesSlowSpeeds) {
  MotorMock mock;
  lh::MotorController controller = lh::MotorController(&mock);
  long target = 300;

  controller.Configure(50, 1, 50, 1);
  controller.set_max_velocity(100, 0);
  controller.set_observed_position(util::MakeFixed(target));

  // make sure we don't go over target
  mock.set_boundary(target);
  for (int i = 0; i < 10000000; ++i) {
    controller.Run();
  }
  EXPECT_EQ(target, mock.position());
}