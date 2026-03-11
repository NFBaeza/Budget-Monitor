find_package(Qt6 REQUIRED COMPONENTS Core Sql)

add_executable(test_report_service
    monthlyreportservice_test.cpp
)

target_include_directories(test_report_service PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)
target_link_libraries(test_report_service
    GTest::gtest_main
    GTest::gmock
    Qt6::Core     
    Qt6::Sql     
)

add_test(NAME SensorServiceTest COMMAND test_report_service)
