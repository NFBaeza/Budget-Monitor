cmake_minimum_required(VERSION 3.16)
project(budget_monitor VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTOUIC ON) 
set(CMAKE_AUTORCC ON) 


set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)


find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/widgets)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/views)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/dialogs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/database)


set(SOURCES
    src/main.cpp
    src/views/mainwindow.cpp
    src/widgets/dashboardwidget.cpp
    src/widgets/savingwidget.cpp
    src/dialogs/formdialog.cpp
    src/dialogs/categorydialog.cpp
    src/dialogs/addingfiledialog.cpp
    src/database/databasemanager.cpp  
)

# Header files
set(HEADERS
    include/views/mainwindow.h
    include/widgets/dashboardwidget.h
    include/widgets/savingwidget.h
    include/dialogs/formdialog.h
    include/dialogs/categorydialog.h
    include/dialogs/addingfiledialog.h
    include/database/databasemanager.h
)
    

# UI files
set(UI_FILES
    ui/mainwindow.ui
    ui/dashboardwidget.ui
    ui/savingwidget.ui
    ui/formdialog.ui
    ui/categorydialog.ui
    ui/addingfiledialog.ui
)

add_subdirectory(libs/XLSXReader)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/database
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
    XLSXREADER
)