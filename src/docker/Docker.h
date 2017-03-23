#pragma once

#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <cstring>
#include <curl/curl.h>

// NOTE: https://curl.haxx.se/libcurl/c/threadsafe.html

namespace docker {

const std::string DOCKER_API_VERSION = "v1.26";

struct Response {
  long code;
  std::string data;
};

struct Buffer {
  char *data;
  size_t size;
  bool allocated;

  Buffer();
  ~Buffer();
};

class Docker {
  Docker();
  ~Docker();

  void initBuffer();
  void initCurl();
  void performCurl(std::string endpoint);
  Response getResponse();

  CURL *_curl;
  Buffer _buffer;
  long code;

public:
  Docker(const Docker&) = delete;
  Docker& operator=(const Docker&) = delete;
  Docker(Docker&&) = default;
  Docker& operator=(Docker&&) = default;

  static Docker &GetInstance() {
    static Docker instance;
    return instance;
  }

  Response POST(std::string endpoint, std::string data = "");
  Response GET(std::string endpoint);
  Response DELETE(std::string endpoint);
};

}
