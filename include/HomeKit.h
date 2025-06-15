#pragma once

#include <functional>
#include <HomeSpan.h>

struct HoodSpanAccessory final : SpanAccessory {
  HoodSpanAccessory();
};

struct HoodFanService final : Service::Fan {
  SpanCharacteristic* active;
  SpanCharacteristic* speed;

  HoodFanService(std::function<void(int)> callback);
  boolean update() override;
  void setSpeed(int speed);

private:
  std::function<void(int)> callback;
};

struct HoodLightBulbService final : Service::LightBulb {
  SpanCharacteristic* on;

  HoodLightBulbService(std::function<void(boolean)> callback);
  boolean update() override;

private:
  std::function<void(boolean)> callback;
};

struct HoodManualModeSwitchService final : Service::Switch {
  SpanCharacteristic* on;

  HoodManualModeSwitchService(std::function<void(boolean)> callback);
  boolean update() override;

private:
  std::function<void(boolean)> callback;
};
