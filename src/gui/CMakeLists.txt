cmake_minimum_required(VERSION 3.5)

project(gui LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets VirtualKeyboard PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets VirtualKeyboard PrintSupport REQUIRED)
find_package(InfluxDB)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        qcustomplot.cpp
        qcustomplot.h
        qjsontree.cpp
        qjsontree.h
        connectionform.cpp
        connectionform.h
        connectionform.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(gui
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(gui SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(gui
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(gui PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::VirtualKeyboard
    Qt${QT_VERSION_MAJOR}::PrintSupport
    InfluxData::InfluxDB
)
