include_directories(${CServer_SOURCE_DIR}/lib/include)
link_directories(${CServer_SOURCE_DIR}/lib/src)

add_executable(web_server main.c)
target_link_libraries(web_server web_server_lib)
