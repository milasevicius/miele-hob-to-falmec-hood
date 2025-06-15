#pragma once

#include <functional>
#include <ArduinoJson.h>
#include <Preferences.h>

class Miele {
  private:
    String clientId;
    String clientSecret;
    String refreshToken;
    String accessToken;
    Preferences preferences;
    JsonDocument filter;
    std::function<void(JsonDocument)> callback;
    void refreshAccessToken();
    void poll();
    void pollTask();

  public:
    Miele(String clientId, String clientSecret, String refreshToken);
    void begin();
    void setFilter(std::function<void(JsonDocument&)> builder);
    void setCallback(std::function<void(JsonDocument)> callback);
    void autoPoll(uint32_t stackSize = 8192, uint32_t priority = 1, uint32_t core = 0);
};
