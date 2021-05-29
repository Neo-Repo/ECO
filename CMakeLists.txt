cmake_minimum_required(VERSION 3.14)

project(ECO LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick Xml REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Xml REQUIRED)

add_executable(ECO
    main.cpp
    src/virt.cpp
    src/virt.h
    src/service.cpp
    src/service.h
    src/startup.cpp
    src/startup.h
    src/power.cpp
    src/power.h
    src/CursorPosProvider.h
    qml.qrc
)

target_compile_definitions(ECO
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(ECO
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Xml virt pthread)
