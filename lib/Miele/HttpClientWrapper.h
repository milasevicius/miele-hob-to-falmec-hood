#pragma once

#include <functional>
#include <HTTPClient.h>

class HttpClientWrapper : public HTTPClient {
public:
  ~HttpClientWrapper();
  void getStream(std::function<void(void* instance, const String& type, const String& data)> callback, void* instance);
};
