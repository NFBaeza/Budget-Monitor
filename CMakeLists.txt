cmake_minimum_required(VERSION 3.16)
project(budget_monitor VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTOUIC ON) 
set(CMAKE_AUTORCC ON) 


set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)


find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Charts Network)

# Fetch Google Test (Same as before)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Link the tests directory
add_subdirectory(tests)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/widgets)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/views)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/dialogs)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/database)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/reportservices)


set(SOURCES
    src/main.cpp
    src/views/mainwindow.cpp
    src/widgets/dashboardwidget.cpp
    src/widgets/savingwidget.cpp
    src/widgets/monthreportwidget.cpp
    src/dialogs/formdialog.cpp
    src/dialogs/categorydialog.cpp
    src/dialogs/addingfiledialog.cpp
    src/dialogs/monthselectordialog.cpp
    src/dialogs/usersettingsdialog.cpp
    src/database/databasemanager.cpp 
    src/database/databaseworker.cpp 
    src/reportservices/monthlyreportservice.cpp 
)

# Header files
set(HEADERS
    include/views/mainwindow.h
    include/widgets/dashboardwidget.h
    include/widgets/savingwidget.h
    include/widgets/monthreportwidget.h
    include/dialogs/formdialog.h
    include/dialogs/categorydialog.h
    include/dialogs/addingfiledialog.h
    include/dialogs/monthselectordialog.h
    include/dialogs/usersettingsdialog.h
    include/database/databasemanager.h
    include/database/databaseworker.h
    include/reportservices/monthlyreportservice.h
)
    

# UI files
set(UI_FILES
    ui/mainwindow.ui
    ui/dashboardwidget.ui
    ui/savingwidget.ui
    ui/formdialog.ui
    ui/categorydialog.ui
    ui/monthselectordialog.ui
    ui/addingfiledialog.ui
    ui/monthreportwidget.ui
    ui/usersettingsdialog.ui
)

add_subdirectory(libs/XLSXReader)

qt6_add_resources(RESOURCES resources.qrc)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCES}
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
    ${CMAKE_CURRENT_SOURCE_DIR}/include/reportservices
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
    Qt6::Network
    XLSXREADER
    gtest
)

# Copy .env to build directory if it exists
if(EXISTS "${CMAKE_SOURCE_DIR}/.env")
    configure_file("${CMAKE_SOURCE_DIR}/.env" "${CMAKE_BINARY_DIR}/.env" COPYONLY)
endif()