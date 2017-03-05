#include "Docker.h"

namespace docker {

static size_t write_function(void *data, size_t size, size_t nmemb, void *buffer);

Buffer::Buffer() {
  size = 0;
}

Buffer::~Buffer() {
  free(data);
  fprintf(stderr, "Destructing Buffer.\n");

}

Docker::Docker() {
  fprintf(stderr, "Constructing Docker.\n");

  curl_global_init(CURL_GLOBAL_ALL);

  if ((_curl = curl_easy_init()) == NULL) {
    perror("ERROR: failed to initialize curl");
    exit(1);
  }

  initCurl();
}

Docker::~Docker() {
  fprintf(stderr, "Destructing Docker.\n");
  curl_easy_cleanup(_curl);
  curl_global_cleanup();
}

void Docker::initBuffer() {
  _buffer.data = (char *)malloc(1);

  if (_buffer.data == NULL) {
    perror("ERROR: failed to initialize buffer.");
    exit(1);
  }

  _buffer.size = 0;
}

void Docker::initCurl() {
  curl_easy_setopt(_curl, CURLOPT_UNIX_SOCKET_PATH, "/var/run/docker.sock");
  curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_function);
  curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_buffer);
}

void Docker::performCurl(std::string endpoint) {
  initBuffer();

  std::string url = "http://" + DOCKER_API_VERSION + "/" + endpoint;

  std::cerr << url << std::endl;

  curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
  CURLcode curl_code = curl_easy_perform(_curl);

  if (curl_code != CURLE_OK) {
    perror(curl_easy_strerror(curl_code));
  }

  curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &code);
  curl_easy_reset(_curl);
}

Response Docker::getResponse() {
  Response response;
  response.code = code;

  std::string data(_buffer.data);
  response.data = data;

  free(_buffer.data);
  _buffer.size = 0;

  return response;
}

Response Docker::POST(std::string endpoint, std::string data) {
  GetInstance().initCurl();

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, (void *)data.c_str());
  performCurl(endpoint);
  curl_slist_free_all(headers);

  return getResponse();
}

Response Docker::GET(std::string endpoint) {
  GetInstance().initCurl();
  performCurl(endpoint);

  return getResponse();
}

Response Docker::DELETE(std::string endpoint) {
  GetInstance().initCurl();

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");
  performCurl(endpoint);
  curl_slist_free_all(headers);

  return getResponse();
}

static size_t write_function(void *data, size_t size, size_t nmemb, void *buffer) {
  size_t realsize = size * nmemb;
  Buffer *mem = (Buffer *)buffer;

  mem->data = (char *)realloc(mem->data, mem->size + realsize + 1);
  if(mem->data == NULL) {
    perror("ERROR: failed to reallocate memory for curl response.");
    exit(1);
  }

  memcpy(&(mem->data[mem->size]), data, realsize);
  mem->size += realsize;
  mem->data[mem->size] = 0;

  return realsize;
}

}
