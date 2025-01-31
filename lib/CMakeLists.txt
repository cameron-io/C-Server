include_directories(${CServer_SOURCE_DIR}/lib/include)

add_library(TcpLib SHARED
    src/RequestParser.cc
    src/RequestHandler.cc
    src/ResponseData.cc
)
set_target_properties(TcpLib PROPERTIES LINKER_LANGUAGE CXX)
