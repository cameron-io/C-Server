include_directories(${CServer_SOURCE_DIR}/lib/include)

add_library(web_server_lib SHARED
    tcp.c
    req_content.c
    req_handler.c
    res_handler.c
    conn_manager.c
)
set_target_properties(web_server_lib PROPERTIES LINKER_LANGUAGE C)
