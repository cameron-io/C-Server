include_directories(${CServer_SOURCE_DIR}/lib/include)
link_directories(${CServer_SOURCE_DIR}/lib/src)

FetchContent_Declare(
  libuv
  GIT_REPOSITORY https://github.com/libuv/libuv.git
  GIT_TAG v1.50.0
)
FetchContent_MakeAvailable(libuv)

add_executable(LibUvServer libuv/main.cc)
target_link_libraries(LibUvServer
    libuv::libuv
    HttpServerLib)
target_include_directories(LibUvServer 
    PRIVATE
        ${CServer_SOURCE_DIR}/app/src/libuv/include)

add_executable(HttpServer multi_platform/main.cc)
target_link_libraries(HttpServer HttpServerLib)
