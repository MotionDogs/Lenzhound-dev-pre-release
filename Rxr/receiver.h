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
  long Position();
private:
  Packet packet_;
};

#endif //rxr_receiver_h