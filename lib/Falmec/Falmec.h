#pragma once

#include <RF433send.h>

class Falmec {
  private:
    RfSend *tx;
    int speed;
    bool light;
    const byte* getCommand(int speed, bool light);
    void sendCommand(const byte* command);

  public:
    Falmec(int pin);
    void set(int speed, bool light);
    void setSpeed(int speed);
    void setLight(bool light);
};
