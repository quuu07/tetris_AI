cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 选项：是否构建为共享库，共享库设为 ON；静态库设为 OFF
option(BUILD_SHARED_LIBS "Build libraries as shared" OFF)

# ========= 本地SFML 3.0.0 配置（解决Ogg缺失报错，完整保留音频网络） =========
set(SFML_STATIC_LIBRARIES TRUE)
# 你的SFML解压根路径，不用修改，和你报错里的路径一致
set(SFML_ROOT "D:/sfmlgame/SFML-3.0.0-windows-gcc-14.2.0-mingw-64-bit/SFML-3.0.0")
# 让CMake读取extlibs里的ogg/vorbis音频依赖库
list(APPEND CMAKE_PREFIX_PATH "${SFML_ROOT}/extlibs")
# 指定SFML的cmake配置目录
set(SFML_DIR "${SFML_ROOT}/lib/cmake/SFML")
# 全部组件保留：Graphics Window System Audio Network 一个不删
find_package(SFML 3.0 COMPONENTS Graphics Window System Audio Network REQUIRED)
# ===========================================================================

# 下面这段自动下载SFML代码已经全部注释失效，不用管
# include(FetchContent)
# FetchContent_Declare(SFML
#     GIT_REPOSITORY https://github.com/SFML/SFML.git
#     GIT_TAG 3.0.1
#     GIT_SHALLOW ON
#     EXCLUDE_FROM_ALL
#     SYSTEM)
# FetchContent_MakeAvailable(SFML)

# 添加一个库目标，包含 Game 和 Tetris 的实现
add_library(GameLib
    src/Game.cpp
    src/Tetris.cpp
    src/GeneticAlgorithm.cpp
)

# 头文件目录
target_include_directories(GameLib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 链接库，音频、网络全部保留
target_link_libraries(GameLib 
    PUBLIC
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
        SFML::Network
)

add_executable(main src/main.cpp)   # 创建可执行文件目标
target_compile_features(main PRIVATE cxx_std_17)

target_link_libraries(main
    PUBLIC
        GameLib
)

# 在Windows系统中（使用 MinGW-w64）构建时
# 静态链接需要将libstdc++、libgcc编译进可执行文件中，避免运行时依赖外部 DLL
if(WIN32 AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")))
  target_link_libraries(main PRIVATE
      -static-libgcc
      -static-libstdc++
    )  
endif()

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data 
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/data 
    COMMENT "Copying data directory to output directory"
)

# 测试 & 打包
enable_testing()
add_test(NAME SmokeTest
         COMMAND main
         WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

install(TARGETS main RUNTIME DESTINATION .)
install(DIRECTORY "data/"
        DESTINATION "data"
        FILES_MATCHING PATTERN "*"
)

set(CPACK_PACKAGE_NAME "Tetris")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "HDU")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CMake-SFML-Tetris")
set(CPACK_PACKAGE_CONTACT "contact@example.com")
include(CPack)