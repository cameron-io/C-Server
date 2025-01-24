include_directories(${CServer_SOURCE_DIR}/lib/include)
link_directories(${CServer_SOURCE_DIR}/lib/src)

add_executable(HttpServer multi_platform/main.cc)
target_link_libraries(HttpServer HttpServerLib)

if(WIN32)
    add_executable(WsaPollServer windows/wsa_poll.cpp)
    target_link_libraries(WsaPollServer HttpServerLib)
endif()