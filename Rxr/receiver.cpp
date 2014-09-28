#include <SPI.h>
#include <Mirf.h>
#include <MirfHardwareSpiDriver.h>
#include <MirfSpiDriver.h>
#include <nRF24L01.h>
#include "receiver.h"
#include "util.h"

Receiver::Receiver() {
  Mirf.spi = &MirfHardwareSpi; 
  Mirf.init(); // Setup pins / SPI
  Mirf.setRADDR((byte *)"serv1"); // Configure recieving address
  Mirf.payload = sizeof(Packet); // Payload length
  Mirf.config(); // Power up reciver
}

long Receiver::Position() {
  if(Mirf.dataReady()){ // Got packet
    Mirf.getData((byte *) &packet_);
    packet_.position = util::MakeFixed(packet_.position);
  }
  return packet_.position;
}