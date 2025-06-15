#pragma once

#include <RF433send.h>

class Falmec {
  private:
    RfSend *tx;
    int speed;
    bool light;
    TaskHandle_t sendCommandTaskHandle;
    const byte* getCommand(int speed, bool light);
    void sendCommandTask();

  public:
    Falmec(int pin);
    void set(int speed, bool light);
    void setSpeed(int speed);
    void setLight(bool light);
};
