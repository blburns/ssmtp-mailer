cmake_minimum_required(VERSION 3.16)
project(ssmtp-mailer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_LOGGING "Enable logging" ON)
option(ENABLE_SSL "Enable SSL/TLS support" ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find OpenSSL
if(ENABLE_SSL)
    find_package(OpenSSL REQUIRED)
endif()

# Find system libraries
find_library(SOCKET_LIBRARY socket)
find_library(RT_LIBRARY rt)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/core/*.cpp"
    "src/utils/*.cpp"
)

# Header files
file(GLOB_RECURSE HEADERS 
    "include/*.hpp"
    "src/*.hpp"
)

# Create main executable
add_executable(ssmtp-mailer ${SOURCES})

# Create shared library
add_library(ssmtp-mailer-lib SHARED ${SOURCES})
set_target_properties(ssmtp-mailer-lib PROPERTIES
    OUTPUT_NAME "ssmtp-mailer"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Link libraries
target_link_libraries(ssmtp-mailer
    ssmtp-mailer-lib
    ${OPENSSL_LIBRARIES}
    ${SOCKET_LIBRARY}
    ${RT_LIBRARY}
    pthread
)

target_link_libraries(ssmtp-mailer-lib
    ${OPENSSL_LIBRARIES}
    ${SOCKET_LIBRARY}
    ${RT_LIBRARY}
    pthread
)

# Compiler flags
target_compile_options(ssmtp-mailer PRIVATE
    -Wall -Wextra -Wpedantic
    -O2 -DNDEBUG
)

target_compile_options(ssmtp-mailer-lib PRIVATE
    -Wall -Wextra -Wpedantic
    -O2 -DNDEBUG
    -fPIC
)

# Install targets
install(TARGETS ssmtp-mailer ssmtp-mailer-lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

# Install configuration files
install(DIRECTORY config/ DESTINATION etc/ssmtp-mailer)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ssmtp-mailer-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-config.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-config-version.cmake"
    DESTINATION lib/cmake/ssmtp-mailer
)
