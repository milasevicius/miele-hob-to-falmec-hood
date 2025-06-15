#include "HomeKit.h"

HoodSpanAccessory::HoodSpanAccessory() {
  new Service::AccessoryInformation();
  new Characteristic::Identify();
}

HoodFanService::HoodFanService(std::function<void(int)> callback) : Service::Fan(), callback(callback) {
  new Characteristic::ConfiguredName("Fan");
  this->active = new Characteristic::Active();
  this->speed = (new Characteristic::RotationSpeed())->setRange(0, 100, 25);
}

boolean HoodFanService::update() {
  callback(this->speed->getNewVal() / 25);
  return true;
}

void HoodFanService::setSpeed(int speed) {
  this->active->setVal(speed > 0);
  this->speed->setVal(speed * 25);
}

HoodLightBulbService::HoodLightBulbService(std::function<void(boolean)> callback) : Service::LightBulb(), callback(callback) {
  new Characteristic::ConfiguredName("Light");
  this->on = new Characteristic::On();
}

boolean HoodLightBulbService::update() {
  callback(this->on->getNewVal());
  return true;
}

HoodManualModeSwitchService::HoodManualModeSwitchService(std::function<void(boolean)> callback) : Service::Switch(), callback(callback) {
  new Characteristic::ConfiguredName("Manual Mode");
  this->on = new Characteristic::On(false);
}

boolean HoodManualModeSwitchService::update() {
  callback(this->on->getNewVal());
  return true;
}
