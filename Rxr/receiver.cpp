#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <nRF24L01.h>
#include "receiver.h"
#include "util.h"

#define PLAYBACK_MODE 1
#define MAX_VELOCITY 100

Receiver::Receiver() {
  Mirf.spi = &MirfHardwareSpi; 
  Mirf.init(); // Setup pins / SPI
  Mirf.setRADDR((byte *)"serv1"); // Configure recieving address
  Mirf.payload = sizeof(Packet); // Payload length
  Mirf.config(); // Power up reciver
}
void Receiver::receive_data(){
  if(Mirf.dataReady()){ // Got packet
    Mirf.getData((byte *) &packet_);
    packet_.position = util::MakeFixed(packet_.position);
  }
}

long Receiver::Position() {
  return packet_.position;
}

int Receiver::Velocity() {
  if(packet_.mode==PLAYBACK_MODE)
    return packet_.velocity;
  return MAX_VELOCITY;
}

char Receiver::Mode() {
  return packet_.mode;
}
