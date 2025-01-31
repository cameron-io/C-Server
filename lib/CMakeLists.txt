include_directories(${CServer_SOURCE_DIR}/lib/include)

add_library(TcpLib SHARED
    src/RequestParser.cpp
    src/RequestHandler.cpp
    src/ResponseData.cpp
)
set_target_properties(TcpLib PROPERTIES LINKER_LANGUAGE CXX)
