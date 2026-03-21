#include <WiFi.h>
#include "HttpClientWrapper.h"
#include "Miele.h"

Miele::Miele(String clientId, String clientSecret, String refreshToken) {
  this->clientId = clientId;
  this->clientSecret = clientSecret;
  this->refreshToken = refreshToken;
  this->filter["*"] = true;
  this->callback = nullptr;
}

void Miele::begin() {
  preferences.begin("MIELE", false);

  if (preferences.isKey("REFRESH_TOKEN")) {
    this->refreshToken = preferences.getString("REFRESH_TOKEN");
  }
}

void Miele::setFilter(std::function<void(JsonDocument&)> builder) {
  this->filter.clear();
  builder(this->filter);
}

void Miele::setCallback(std::function<void(JsonDocument)> callback) {
  this->callback = callback;
}

bool Miele::refreshAccessToken() {
  HttpClientWrapper http;
  String url = "https://auth.domestic.miele-iot.com/partner/realms/mcs/protocol/openid-connect/token";

  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Accept-Language", "en");

  String body = "";
  body.concat("client_id=" + this->clientId);
  body.concat("&client_secret=" + this->clientSecret);
  body.concat("&refresh_token=" + this->refreshToken);
  body.concat("&grant_type=refresh_token");

  int httpCode = http.POST(body);

  if (httpCode != HTTP_CODE_OK) {
    return false;
  }

  JsonDocument filter;
  filter["access_token"] = true;
  filter["refresh_token"] = true;
  JsonDocument doc;
  DeserializationError error = deserializeJson(
    doc,
    http.getString(),
    DeserializationOption::Filter(filter)
  );

  if (error) {
    return false;
  }

  this->accessToken = doc["access_token"].as<String>();
  this->refreshToken = doc["refresh_token"].as<String>();

  preferences.putString("REFRESH_TOKEN", this->refreshToken);
  return true;
}

bool Miele::poll() {
  HttpClientWrapper http;
  String url = "https://api.mcs3.miele.com/v1/devices/all/events";

  http.begin(url);
  http.addHeader("Accept", "text/event-stream");
  http.addHeader("Accept-Language", "en");
  http.addHeader("Authorization", "Bearer " + this->accessToken);

  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    return false;
  }

  http.getStream([](void* instance, const String& type, const String& data) {
    Miele* miele = static_cast<Miele*>(instance);

    if (type != "devices" || data.length() == 0) {
      return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(
      doc,
      data,
      DeserializationOption::Filter(miele->filter)
    );

    if (error) {
      return;
    }

    if (miele->callback) {
      miele->callback(doc);
    }
  }, this);

  return true;
}

void Miele::pollTask() {
  if (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    return;
  }

  if (!refreshAccessToken()) {
    delay(30000);
    return;
  }

  if (!poll()) {
    delay(30000);
    return;
  }
}

void Miele::autoPoll(uint32_t stackSize, uint32_t priority, uint32_t core) {
  xTaskCreateUniversal(
    [](void *instance) {
      Miele* miele = static_cast<Miele*>(instance);
      for(;;) miele->pollTask();
    },
    "pollTask",
    stackSize,
    this,
    priority,
    NULL,
    core
  );
}
