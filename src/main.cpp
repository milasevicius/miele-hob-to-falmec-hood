#include <HomeSpan.h>
#include <ArduinoJson.h>
#include <Miele.h>
#include <Falmec.h>
#include "HomeKit.h"
#include "main.h"

HoodFanService* hoodFanService = nullptr;
HoodLightBulbService* hoodLightBulbService = nullptr;
HoodManualModeSwitchService* hoodManualModeSwitchService = nullptr;

Miele miele(MIELE_CLIENT_ID, MIELE_CLIENT_SECRET, MIELE_REFRESH_TOKEN);
Falmec falmec(RF_TX_PIN);

int lastHoodSpeed = 0;

void handleFanUpdate(int speed) {
  hoodManualModeSwitchService->on->setVal(true);
  falmec.setSpeed(speed);

  WEBLOG("[HomeKit Fan] Manual override - setting hood speed to %d", speed);
}

void handleLightUpdate(bool on) {
  hoodManualModeSwitchService->on->setVal(true);
  falmec.setLight(on);

  WEBLOG("[HomeKit Light] Manual override - setting hood light to %s", on ? "ON" : "OFF");
}

void handleManualModeSwitchUpdate(bool on) {
  if (on) {
    WEBLOG("[HomeKit Switch] Miele automation DISABLED");

    return;
  }

  if (hoodFanService) {
    hoodFanService->setSpeed(lastHoodSpeed);
  }

  if (hoodLightBulbService) {
    hoodLightBulbService->on->setVal(lastHoodSpeed > 0);
  }

  falmec.set(lastHoodSpeed, lastHoodSpeed > 0);

  WEBLOG("[HomeKit Switch] Miele automation ENABLED - applying latest command (speed=%d, light=%s)", lastHoodSpeed, lastHoodSpeed > 0 ? "ON" : "OFF");
}

int mapHobPowerToHoodSpeed(int power) {
  // Keep warm
  // Standard power level range (220)
  // Extended power level range (110)
  if (power == 220 || power == 110) {
    return 0;
  }

  // Boost 1, boost 2
  // Standard power level range (118, 218)
  // Extended power level range (117, 217)
  if (power == 118 || power == 218 || power == 117 || power == 217) {
    return 1;
  }

  // Regular power levels
  // Standard power level range (9 settings, even numbers 2-18)
  // Extended power level range (17 settings, all numbers 1-17)
  return ((power + (power % 2 == 0 ? 4 : 5)) / 6);
}

void handleMieleDeviceEvent(JsonDocument data) {
  JsonArray plateStep = data[MIELE_DEVICE_ID]["state"]["plateStep"];

  if (plateStep.isNull()) {
    return;
  }

  lastHoodSpeed = mapHobPowerToHoodSpeed(0);

  for (JsonObject item : plateStep) {
    lastHoodSpeed = std::max(lastHoodSpeed, mapHobPowerToHoodSpeed(item["value_raw"]));
  }

  if (hoodManualModeSwitchService && hoodManualModeSwitchService->on->getVal()) {
    WEBLOG("[Miele] Automation DISABLED - storing latest command (speed=%d, light=%s)", lastHoodSpeed, lastHoodSpeed > 0 ? "ON" : "OFF");
    return;
  }

  homeSpanPAUSE;
  if (hoodFanService) {
    hoodFanService->setSpeed(lastHoodSpeed);
  }

  if (hoodLightBulbService) {
    hoodLightBulbService->on->setVal(lastHoodSpeed > 0);
  }
  homeSpanRESUME;

  falmec.set(lastHoodSpeed, lastHoodSpeed > 0);

  WEBLOG("[Miele] Automation ENABLED - applying latest command (speed=%d, light=%s)", lastHoodSpeed, lastHoodSpeed > 0 ? "ON" : "OFF");
}

void setup() {
  Serial.begin(115200);
  Serial.flush();

  homeSpan.enableOTA();
  homeSpan.enableWebLog(100, "pool.ntp.org", "UTC", "log");
  homeSpan.setSketchVersion(HOMESPAN_SKETCH_VERSION);
  homeSpan.setControlPin(HOMESPAN_BUTTON_PIN);
  homeSpan.setStatusPixel(HOMESPAN_PIXEL_PIN, 240, 100, 100);
  homeSpan.begin(Category::Fans, "Cooker Hood");

  new HoodSpanAccessory();
  hoodFanService = new HoodFanService(handleFanUpdate);
  hoodLightBulbService = new HoodLightBulbService(handleLightUpdate);
  hoodManualModeSwitchService = new HoodManualModeSwitchService(handleManualModeSwitchUpdate);

  homeSpan.autoPoll(8192, 1, 0);

  miele.begin();
  miele.setFilter([](JsonDocument& filter) {
    filter[MIELE_DEVICE_ID]["state"]["plateStep"] = true;
  });
  miele.setCallback(handleMieleDeviceEvent);
  miele.autoPoll(8192, 1, 1);
}

void loop() {
  delay(1000);
}
