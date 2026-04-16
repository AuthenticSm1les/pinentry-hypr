cmake_minimum_required(VERSION 3.16)
project(pinentry-hypr VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml Widgets QuickControls2)
find_package(PkgConfig REQUIRED)
pkg_check_modules(HYPRUTILS REQUIRED hyprutils)
pkg_check_modules(LIBSECRET REQUIRED libsecret-1)

qt_standard_project_setup()

qt_add_executable(pinentry-hypr
    main.cpp
)

qt_add_qml_module(pinentry-hypr
    URI PinentryHypr
    VERSION 1.0
    QML_FILES Main.qml
)

target_link_libraries(pinentry-hypr PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::Widgets
    Qt6::QuickControls2
    ${HYPRUTILS_LIBRARIES}
    ${LIBSECRET_LIBRARIES}
)

target_include_directories(pinentry-hypr PRIVATE 
    ${HYPRUTILS_INCLUDE_DIRS}
    ${LIBSECRET_INCLUDE_DIRS}
)

install(TARGETS pinentry-hypr DESTINATION bin)
