include_directories(${CServer_SOURCE_DIR}/lib/include)

add_library(HttpServerLib SHARED
    event_manager.cc
    http_server.cc
    req_handler.cc
    res_handler.cc
    res_content.cc
)
set_target_properties(HttpServerLib PROPERTIES LINKER_LANGUAGE CXX)
