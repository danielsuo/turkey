set(CMAKE_BUILD_TYPE Debug)

add_executable(
  main

  main.cpp
  Server.cpp
)

add_executable(
  main_client

  main_client.cpp
  Client.cpp
)
target_link_libraries (main ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (main_client ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (main rt)
target_link_libraries (main_client rt)
