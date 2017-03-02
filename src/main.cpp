#include "docker.h"
#include "json.hpp"

using json = nlohmann::json;

// http://stackoverflow.com/questions/29173193/shared-memory-with-docker-containers-docker-version-1-4-1
// http://stackoverflow.com/questions/36998845/inter-process-communication-between-docker-container-and-its-host
// http://freecontent.manning.com/wp-content/uploads/docker-in-action-shared-memory.pdf
int main() {

  fprintf(stderr, "curl_version: %s\n", curl_version());

  DOCKER *docker = docker_init("v1.25");

  if (docker) {
    CURLcode response = docker_post(docker, "http://v1.25/containers/create",
                                    "{\"Image\": \"alpine\", \"Cmd\": [\"echo\", \"hello world\"]}");
    if (response == CURLE_OK) {
      fprintf(stderr, "%s\n", docker_buffer(docker));
    }

    response = docker_get(docker, "http://v1.25/images/json");

    if (response == CURLE_OK) {
      fprintf(stderr, "%s\n", docker_buffer(docker));
    }

    docker_destroy(docker);
  } else {
    fprintf(stderr, "ERROR: Failed to get get a docker client!\n");
  }

  json j2 = {
    {"pi", 3.141},
    {"happy", true},
    {"name", "Niels"},
    {"nothing", nullptr},
    {"answer", {
              {"everything", 42}
           }},
    {"list", {1, 0, 2}},
    {"object", {
              {"currency", "USD"},
                 {"value", 42.99}
           }}
  };

  return 0;
}