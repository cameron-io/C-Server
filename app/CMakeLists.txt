include_directories(${CServer_SOURCE_DIR}/lib/include)
link_directories(${CServer_SOURCE_DIR}/lib/src)

FetchContent_Declare(
  libuv
  GIT_REPOSITORY https://github.com/libuv/libuv.git
  GIT_TAG v1.50.0
)
FetchContent_MakeAvailable(libuv)

add_executable(HttpServer src/main.c)
target_link_libraries(HttpServer
    libuv::libuv
    TcpLib)
target_include_directories(HttpServer 
    PRIVATE
        ${CServer_SOURCE_DIR}/app/include)
