cmake_minimum_required(VERSION 3.16)
project(StreamUFSC VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Necessário para o Qt: gera automaticamente os arquivos moc_*, ui_* e qrc_*
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(StreamUFSC
    main.cpp
    movie.h movie.cpp
    theme.h theme.cpp
    startwindow.h startwindow.cpp startwindow.ui
    mainwindow.h mainwindow.cpp mainwindow.ui
    addmoviewindow.h addmoviewindow.cpp addmoviewindow.ui
    aboutwindow.h aboutwindow.cpp aboutwindow.ui
    resources.qrc
)

target_link_libraries(StreamUFSC PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
