cmake_minimum_required(VERSION 3.16)
project(budget_monitor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "/home/miosa/Qt/6.8.3/gcc_64")
set(Qt6_DIR "/home/miosa/Qt/6.8.3/gcc_64/lib/cmake/Qt6")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON) # Procesa macros como Q_OBJECT en los .h
set(CMAKE_AUTOUIC ON) # Convierte tus .ui en código C++
set(CMAKE_AUTORCC ON) # Maneja archivos de recursos (imágenes, iconos)

# Tell AutoUIC where to find .ui files
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)


find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/widgets)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/views)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/dialogs)

# Source files organized by type
set(SOURCES
    src/main.cpp
    src/views/mainwindow.cpp
    src/widgets/dashboardwidget.cpp
    src/dialogs/formdialog.cpp
    src/dialogs/categorydialog.cpp
)

# Header files
set(HEADERS
    include/views/mainwindow.h
    include/widgets/dashboardwidget.h
    include/dialogs/formdialog.h
    include/dialogs/categorydialog.h
)

# UI files
set(UI_FILES
    ui/mainwindow.ui
    ui/dashboardwidget.ui
    ui/formdialog.ui
    ui/categorydialog.ui
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/include/views
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dialogs
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
)