cmake_minimum_required(VERSION 3.20)

project(trade_aggregation VERSION 1.0.0 LANGUAGES CXX)

option(TRADE_AGGREGATION_BUILD_TESTS "Build the C++ test suite" ON)
option(TRADE_AGGREGATION_BUILD_EXAMPLES "Build the usage examples" ON)

add_library(trade_aggregation src/utils.cpp)
add_library(trade_aggregation::trade_aggregation ALIAS trade_aggregation)
target_compile_features(trade_aggregation PUBLIC cxx_std_20)
target_include_directories(trade_aggregation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
set_target_properties(trade_aggregation PROPERTIES
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

function(trade_aggregation_configure_compile_policy target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target} PRIVATE -Wno-unqualified-std-cast-call)
    endif()
endfunction()

trade_aggregation_configure_compile_policy(trade_aggregation)

if(TRADE_AGGREGATION_BUILD_TESTS)
    enable_testing()
    add_executable(trade_aggregation_tests tests/trade_aggregation_tests.cpp)
    target_link_libraries(trade_aggregation_tests PRIVATE trade_aggregation::trade_aggregation)
    target_compile_definitions(trade_aggregation_tests PRIVATE
        TRADE_AGGREGATION_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    )
    trade_aggregation_configure_compile_policy(trade_aggregation_tests)
    add_test(NAME trade_aggregation_tests COMMAND trade_aggregation_tests)
endif()

if(TRADE_AGGREGATION_BUILD_EXAMPLES)
    add_executable(streaming_aggregate_ohlc examples/streaming_aggregate_ohlc.cpp)
    target_link_libraries(streaming_aggregate_ohlc PRIVATE trade_aggregation::trade_aggregation)
    trade_aggregation_configure_compile_policy(streaming_aggregate_ohlc)

    add_executable(aggregate_all_ohlc examples/aggregate_all_ohlc.cpp)
    target_link_libraries(aggregate_all_ohlc PRIVATE trade_aggregation::trade_aggregation)
    trade_aggregation_configure_compile_policy(aggregate_all_ohlc)

    add_executable(user_trade_type examples/user_trade_type.cpp)
    target_link_libraries(user_trade_type PRIVATE trade_aggregation::trade_aggregation)
    trade_aggregation_configure_compile_policy(user_trade_type)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
install(TARGETS trade_aggregation EXPORT trade_aggregationTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
configure_package_config_file(
    cmake/trade_aggregationConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/trade_aggregationConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/trade_aggregation
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/trade_aggregationConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(EXPORT trade_aggregationTargets
    NAMESPACE trade_aggregation::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/trade_aggregation
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/trade_aggregationConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/trade_aggregationConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/trade_aggregation
)
