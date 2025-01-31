include_directories(${CServer_SOURCE_DIR}/lib/include)

add_library(TcpLib SHARED
    src/req_parser.cc
    src/req_handler.cc
    src/res_data.cc
)
set_target_properties(TcpLib PROPERTIES LINKER_LANGUAGE CXX)
