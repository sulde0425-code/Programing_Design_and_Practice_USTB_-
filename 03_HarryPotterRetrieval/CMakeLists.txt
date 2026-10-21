cmake_minimum_required(VERSION 3.16)
project(HarryPotterRetrieval VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows MSVC: treat source files as UTF-8
if(MSVC)
    add_compile_options(/utf-8)
endif()

# 源文件
set(CORE_SOURCES
    src/core/TextProcessor.cpp
    src/core/InvertedIndex.cpp
    src/core/SearchEngine.cpp
    src/core/Trie.cpp
    src/core/Similarity.cpp
    src/core/StringMatch.cpp
)

set(CORE_HEADERS
    src/core/TextProcessor.h
    src/core/InvertedIndex.h
    src/core/SearchEngine.h
    src/core/Trie.h
    src/core/Similarity.h
    src/core/StringMatch.h
)

# ===== 命令行测试程序（无需 Qt）=====
add_executable(HarryPotterCLI
    src/cli_main.cpp
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

target_include_directories(HarryPotterCLI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ===== Qt GUI 程序（可选）=====
option(BUILD_GUI "Build Qt GUI application" ON)

if(BUILD_GUI)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

    # macOS 特定设置
    if(APPLE)
        set(CMAKE_MACOSX_RPATH ON)
        set(CMAKE_FIND_FRAMEWORK LAST)
    endif()

    # 查找 Qt6 或 Qt5
    find_package(Qt6 COMPONENTS Widgets QUIET)
    if(NOT Qt6_FOUND)
        find_package(Qt5 COMPONENTS Widgets REQUIRED)
        set(QT_VERSION 5)
    else()
        set(QT_VERSION 6)
    endif()

    set(GUI_SOURCES
        src/gui/MainWindow.cpp
        src/gui/CompareWidget.cpp
    )

    set(GUI_HEADERS
        src/gui/MainWindow.h
        src/gui/CompareWidget.h
    )

    # 主程序
    add_executable(${PROJECT_NAME}
        src/main.cpp
        ${CORE_SOURCES}
        ${CORE_HEADERS}
        ${GUI_SOURCES}
        ${GUI_HEADERS}
    )

    # 链接 Qt
    if(QT_VERSION EQUAL 6)
        target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
    endif()

    # 包含目录
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    # macOS rpath 设置
    if(APPLE)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            INSTALL_RPATH "@executable_path/../Frameworks;$ENV{HOME}/Frameworks;$ENV{HOME}/Qt/6.6.1/macos/lib"
            BUILD_RPATH "$ENV{HOME}/Frameworks;$ENV{HOME}/Qt/6.6.1/macos/lib"
        )
    endif()

    # 安装规则
    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
    )
endif()

message(STATUS "Build GUI: ${BUILD_GUI}")
message(STATUS "To build GUI, use: cmake -DBUILD_GUI=ON ..")
