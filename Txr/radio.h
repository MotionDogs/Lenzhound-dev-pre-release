#ifndef Radio_h
#define Radio_h


class Radio {
public:
  Radio(int packetSize);
  void ReloadSettings(int constantWave);
  void SendPacket(byte* message);
  void SendConstantWave();
private:
  void LoadSettings(int constantWave);
};

#endif //Radio_h