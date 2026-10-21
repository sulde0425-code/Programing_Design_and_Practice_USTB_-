cmake_minimum_required(VERSION 3.16)
project(BankSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
# 如果使用 Qt5，将上面一行改为：
# find_package(Qt5 REQUIRED COMPONENTS Widgets Core)

set(SOURCES
    date.cpp
    account.cpp
    mainwindow.cpp
    main.cpp
)

set(HEADERS
    date.h
    account.h
    mainwindow.h
)

# WIN32: 隐藏控制台黑框，纯 GUI 应用程序
add_executable(bank WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(bank PRIVATE
    Qt6::Widgets
    Qt6::Core
)
# Qt5 用户改为：
# target_link_libraries(bank PRIVATE Qt5::Widgets Qt5::Core)

# ===== 生成 Release 版本的属性 =====
# 设置默认构建类型为 Release（可不加，在 Qt Creator 中手动切换）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set_target_properties(bank PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# ===== 安装规则（可选，配合 windeployqt 使用）=====
install(TARGETS bank DESTINATION .)
