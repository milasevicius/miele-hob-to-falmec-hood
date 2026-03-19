#include <StreamUtils/Clients/ChunkDecodingClient.hpp>
#include "HttpClientWrapper.h"

HttpClientWrapper::~HttpClientWrapper() {
  end();
}

void HttpClientWrapper::getStream(std::function<void(void* instance, const String& type, const String& data)> callback, void* instance) {
  StreamUtils::ChunkDecodingClient client(*getStreamPtr());
  String eventType = "";
  String eventData = "";

  while (client.connected()) {
    if (!client.available()) {
      delay(10);
      continue;
    }

    String line = client.readStringUntil('\n');

    if (line.endsWith("\r")) {
      line.remove(line.length() - 1);
    }

    if (line.length() == 0) {
      if (eventData.length() > 0 || eventType.length() > 0) {
        callback(instance, eventType, eventData);

        eventType = "";
        eventData = "";
      }

      continue;
    }

    if (line.startsWith("event: ")) {
      line.remove(0, 7);
      eventType = line;
      continue;
    }

    if (line.startsWith("data: ")) {
      if (eventData.length() > 0) {
        eventData += "\n";
      }

      line.remove(0, 6);
      eventData += line;
      continue;
    }
  }
}
