#pragma once

#include <stdio.h>
#include <errno.h>
#include <string>
#include <cstring>
#include <curl/curl.h>

// NOTE: https://curl.haxx.se/libcurl/c/threadsafe.html

namespace docker {

const std::string DOCKER_API_VERSION = "v1.25";

struct Response {
  CURLcode code;
  std::string data;
};

struct Buffer {
  char *data;
  size_t size;

  Buffer();
  ~Buffer();
};

class Docker {
  Docker();
  ~Docker();

  void initBuffer();
  void initCurl();
  void executeCurl(std::string url);
  Response getResponse();

  CURL *_curl;
  Buffer _buffer;
  CURLcode code;

public:
  Docker(const Docker&) = delete;
  Docker& operator=(const Docker&) = delete;
  Docker(Docker&&) = default;
  Docker& operator=(Docker&&) = default;

  static Docker &GetInstance() {
    static Docker instance;
    return instance;
  }

  Response POST(std::string url, std::string data);
  Response GET(std::string url);
};

}
