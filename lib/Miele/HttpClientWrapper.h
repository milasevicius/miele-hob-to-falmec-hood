#pragma once

#include <HTTPClient.h>

class HttpClientWrapper : public HTTPClient {
public:
  ~HttpClientWrapper();
  void getStream(void (*callback)(void* instance, const String& type, const String& data), void* instance);
};
