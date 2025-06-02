#include "HttpClientWrapper.h"

HttpClientWrapper::~HttpClientWrapper() {
  end();
}
