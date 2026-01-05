cmake_minimum_required(VERSION 3.16)
project(budget_monitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "/home/miosa/Qt/6.8.3/gcc_64")
set(Qt6_DIR "/home/miosa/Qt/6.8.3/gcc_64/lib/cmake/Qt6")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON) # Procesa macros como Q_OBJECT en los .h
set(CMAKE_AUTOUIC ON) # Convierte tus .ui en código C++
set(CMAKE_AUTORCC ON) # Maneja archivos de recursos (imágenes, iconos)


find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts)

set(SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    dashboardwidget.cpp
    dashboardwidget.h
    dashboardwidget.ui
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets 
    Qt6::Sql
    Qt6::Charts
)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/budget_monitor.db"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")