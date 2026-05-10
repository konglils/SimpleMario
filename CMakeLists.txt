cmake_minimum_required(VERSION 3.20)
project(GameEngine LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 设置 SFML 路径
if (WIN32)
    set(SFML_ROOT "./lib/SFML-2.6.1-gcc")
    set(WIN_STATIC_LINK OFF)
else ()
    set(SFML_ROOT "./lib/SFML-2.6.1-linux")
endif ()

if (WIN_STATIC_LINK)
    add_definitions(-DSFML_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${SRC_DIR}
        ${SRC_DIR}/Components
        ${SRC_DIR}/Components/Collisions
        ${SRC_DIR}/Components/CollisionHandles
        ${SRC_DIR}/GameObjects
        ${SRC_DIR}/Scene
        ${SRC_DIR}/State
        ${SRC_DIR}/Manager
        ${SRC_DIR}/Network

        ${LIB_DIR}
        ${SFML_ROOT}/include
)

target_link_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${SFML_ROOT}/lib
)

if (WIN32)
    if (WIN_STATIC_LINK)
        # Windows 下静态链接
        target_link_libraries(
                ${PROJECT_NAME}
                PRIVATE
                debug sfml-audio-s-d
                debug sfml-graphics-s-d
                debug sfml-window-s-d
                debug sfml-system-s-d
                debug sfml-network-s-d

                optimized sfml-audio-s
                optimized sfml-graphics-s
                optimized sfml-window-s
                optimized sfml-system-s
                optimized sfml-network-s

                # OpenAL 音频库
                openal32
                # 音频编解码器库
                flac
                vorbisenc
                vorbisfile
                vorbis
                ogg
                # 其他依赖库
                freetype
                opengl32
                winmm
                ws2_32
        )
    else ()
        target_link_libraries(
                ${PROJECT_NAME}
                PRIVATE
                debug sfml-graphics-d
                debug sfml-window-d
                debug sfml-system-d
                debug sfml-network-d
                debug sfml-audio-d
                optimized sfml-graphics
                optimized sfml-window
                optimized sfml-system
                optimized sfml-network
                optimized sfml-audio
        )
    endif ()
else ()
    target_link_libraries(
            ${PROJECT_NAME}
            PRIVATE
            sfml-graphics
            sfml-window
            sfml-system
            sfml-audio
            sfml-network
    )
endif ()

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SRC_DIR}/Asset/
        $<TARGET_FILE_DIR:GameEngine>/Asset
)