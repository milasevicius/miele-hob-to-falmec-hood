#include <NetworkClient.h>
#include "HttpClientWrapper.h"

HttpClientWrapper::~HttpClientWrapper() {
  end();
}

void HttpClientWrapper::getStream(void (*callback)(void* instance, const String& type, const String& data), void* instance) {
  NetworkClient *client = getStreamPtr();
  String eventType = "";
  String eventData = "";

  while (client->connected()) {
    if (!client->available()) {
      delay(10);
      continue;
    }

    String line = client->readStringUntil('\n');

    if (line.length() == 0) {
      if (eventData.length() > 0 || eventType.length() > 0) {
        callback(instance, eventType, eventData);

        eventType = "";
        eventData = "";
      }

      continue;
    }

    if (line.startsWith("event: ")) {
      eventType = line.substring(7);
      continue;
    }

    if (line.startsWith("data: ")) {
      if (eventData.length() > 0) {
        eventData += "\n";
      }

      eventData += line.substring(6);
      continue;
    }
  }
}
