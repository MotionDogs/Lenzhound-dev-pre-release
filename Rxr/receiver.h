#ifndef rxr_receiver_h
#define rxr_receiver_h

struct Packet {
  Packet() 
  : position(0), velocity(0), mode(0) {}
  long position;
  char velocity;
  char mode;
};

class Receiver {
public:
  Receiver();
  void ReloadSettings();
  long Position();
<<<<<<< HEAD
  int Velocity();
  char Mode();
=======
>>>>>>> origin/master
private:
  Packet packet_;
  void LoadSettings();
};

#endif //rxr_receiver_h
