include_directories(${CServer_SOURCE_DIR}/lib/include)
link_directories(${CServer_SOURCE_DIR}/lib/src)

add_executable(http_server main.cc)
target_link_libraries(http_server http_server_lib)