include_directories(${CServer_SOURCE_DIR}/lib/include)

add_library(TcpLib SHARED
    src/req_parser.c
    src/req_router.c
    src/res_content.c
)
set_target_properties(TcpLib PROPERTIES LINKER_LANGUAGE CXX)
