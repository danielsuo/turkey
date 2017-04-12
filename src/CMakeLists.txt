set(CMAKE_BUILD_TYPE Debug)

add_executable(main main.cpp
                    Server.cpp)
target_link_libraries (main ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (main rt)
