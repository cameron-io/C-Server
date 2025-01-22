include_directories(${CServer_SOURCE_DIR}/lib/include)
link_directories(${CServer_SOURCE_DIR}/lib/src)

add_executable(HttpServer main.cc)
target_link_libraries(HttpServer HttpServerLib)

add_executable(WsaPollServer WsaPollServer.cc)