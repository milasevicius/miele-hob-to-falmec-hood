#include <HomeSpan.h>
#include <ArduinoJson.h>
#include <Miele.h>
#include <Falmec.h>
#include "main.h"

Miele miele(MIELE_CLIENT_ID, MIELE_CLIENT_SECRET, MIELE_REFRESH_TOKEN);
Falmec falmec(RF_TX_PIN);

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

  int speed = mapHobPowerToHoodSpeed(0);

  for (JsonObject item : plateStep) {
    int mappedSpeed = mapHobPowerToHoodSpeed(item["value_raw"]);
    if (mappedSpeed > speed) {
      speed = mappedSpeed;
    }
  }

  falmec.set(speed, speed > 0);
}

void setup() {
  Serial.begin(115200);
  Serial.flush();

  homeSpan.enableOTA();
  homeSpan.enableWebLog(100, "pool.ntp.org", "UTC", "log");
  homeSpan.setSketchVersion(HOMESPAN_SKETCH_VERSION);
  homeSpan.setControlPin(HOMESPAN_BUTTON_PIN);
  homeSpan.setStatusPixel(HOMESPAN_PIXEL_PIN, 240, 100, 100);
  homeSpan.begin(Category::Other, "HomeSpan Miele Hob");
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
