include_directories(${CServer_SOURCE_DIR}/lib/include)

add_library(HttpServerLib SHARED
    event_manager.cc
    http_server.cc
    request_handler.cc
    response_handler.cc
)
set_target_properties(HttpServerLib PROPERTIES LINKER_LANGUAGE CXX)
