include_directories(${CServer_SOURCE_DIR}/lib/include)

add_library(http_server_lib SHARED
    http_server.cc
    http_client.cc
    request_handler.cc
)
set_target_properties(http_server_lib PROPERTIES LINKER_LANGUAGE CXX)
