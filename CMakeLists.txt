cmake_minimum_required(VERSION 3.20)
project(GameEngine)

set(CMAKE_CXX_STANDARD 17)

# 设置 SFML 路径
set(SFML_ROOT "./lib/SFML-2.6.1-gcc")
set(WIN_STATIC_LINK ON)

if(WIN_STATIC_LINK)
    add_definitions(-DSFML_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

# 包含头文件
include_directories(${SFML_ROOT}/include)
include_directories(./lib)
include_directories(src)
include_directories(src/Components)
include_directories(src/Components/Collisions)
include_directories(src/Components/CollisionHandles)
include_directories(src/GameObjects)
include_directories(src/Scene)
include_directories(src/State)
include_directories(src/Manager)

# 定义源文件列表
file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

link_directories(${SFML_ROOT}/lib)
add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
    # Windows 下需区分 Debug/Release 后缀（如 sfml-graphics-d.lib）
    if(WIN_STATIC_LINK)
        # Windows 下静态链接
        # 首先尝试查找 openal32
        if(EXISTS "${SFML_ROOT}/lib/libopenal32.a")
            set(OPENAL_LIB "openal32")
        elseif(EXISTS "${SFML_ROOT}/lib/libopenal.a")
            set(OPENAL_LIB "openal")
        else()
            # 如果没有找到，可能需要从系统路径查找
            set(OPENAL_LIB "openal32")
        endif()

        target_link_libraries(
                ${PROJECT_NAME}
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
                ${OPENAL_LIB}

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
                gdi32
                ws2_32

                # 额外系统库
                kernel32
                user32
                shell32
                ole32
                oleaut32
                uuid
                comdlg32
                advapi32
        )
    else()
        target_link_libraries(
                ${PROJECT_NAME}
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
    endif()
else()
    # Linux/macOS 通常无后缀
    target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system sfml-network)
endif()
