include_directories(${CServer_SOURCE_DIR}/lib/include)

add_library(web_server_lib SHARED
    content.c
    handle.c
    manager.c
    responses.c
    socket.c
)
set_target_properties(web_server_lib PROPERTIES LINKER_LANGUAGE C)
