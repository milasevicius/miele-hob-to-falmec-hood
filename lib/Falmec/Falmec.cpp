#include "Falmec.h"

Falmec::Falmec(int pin) {
  this->tx = rfsend_builder(
    RfSendEncoding::TRIBIT_INVERTED,
    pin,
    RFSEND_DEFAULT_CONVENTION, // Do we want to invert 0 and 1 bits? No.
    10,                        // Number of sendings
    nullptr,                   // No callback to keep/stop sending
    5940,                      // initseq
    0,                         // lo_prefix
    0,                         // hi_prefix
    312,                       // first_lo_ign
    312,                       // lo_short
    622,                       // lo_long
    0,                         // hi_short
    0,                         // hi_long
    308,                       // lo_last
    13688,                     // sep
    16                         // nb_bits
  );
  this->speed = 0;
  this->light = false;

  xTaskCreateUniversal(
    [](void* instance) {
      auto* falmec = static_cast<Falmec*>(instance);
      for (;;) {
        falmec->sendCommandTask();
      }
    },
    "sendCommandTask",
    2048,
    this,
    2,
    &sendCommandTaskHandle,
    1
  );
}

const byte* Falmec::getCommand(int speed, bool light) {
  static const byte commands[5][2][2] = {
    {{0x4b, 0xf9}, {0x4b, 0xb9}}, // Speed 0 light off, Speed 0 light on
    {{0x4b, 0x79}, {0x4b, 0x39}}, // Speed 1 light off, Speed 1 light on
    {{0x4a, 0xf9}, {0x4a, 0xb9}}, // Speed 2 light off, Speed 2 light on
    {{0x4a, 0x79}, {0x4a, 0x39}}, // Speed 3 light off, Speed 3 light on
    {{0x49, 0xf9}, {0x49, 0xb9}}  // Speed 4 light off, Speed 4 light on
  };

  return commands[speed][light];
}

void Falmec::sendCommandTask() {
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  this->tx->send(2, getCommand(this->speed, this->light));
}

void Falmec::set(int speed, bool light) {
  this->speed = speed;
  this->light = light;
  xTaskNotifyGive(this->sendCommandTaskHandle);
}

void Falmec::setSpeed(int speed) {
  this->speed = speed;
  xTaskNotifyGive(this->sendCommandTaskHandle);
}

void Falmec::setLight(bool light) {
  this->light = light;
  xTaskNotifyGive(this->sendCommandTaskHandle);
}
