cmake_minimum_required(VERSION 3.16)
project(ssmtp-mailer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment version" FORCE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
    set(PLATFORM_MACOS TRUE)
    message(STATUS "Building for macOS (Big Sur 11.0+)")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    message(STATUS "Building for Linux")
elseif(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Building for Windows")
endif()

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

# Platform-specific library handling
if(PLATFORM_MACOS)
    # macOS specific libraries
    find_library(SECURITY_FRAMEWORK Security)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    find_library(SYSTEM_CONFIGURATION_FRAMEWORK SystemConfiguration)
    
    # macOS doesn't need separate socket or rt libraries
    set(PLATFORM_LIBRARIES 
        ${SECURITY_FRAMEWORK}
        ${CORE_FOUNDATION_FRAMEWORK}
        ${SYSTEM_CONFIGURATION_FRAMEWORK}
    )
    
    # Set macOS-specific compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    
    # Enable universal binary support for Intel + Apple Silicon
    if(CMAKE_OSX_ARCHITECTURES MATCHES ".*arm64.*")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target arm64-apple-macos11.0")
    endif()
    if(CMAKE_OSX_ARCHITECTURES MATCHES ".*x86_64.*")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target x86_64-apple-macos11.0")
    endif()
    
elseif(PLATFORM_LINUX)
    # Linux specific libraries
    find_library(SOCKET_LIBRARY socket)
    find_library(RT_LIBRARY rt)
    
    set(PLATFORM_LIBRARIES 
        ${SOCKET_LIBRARY}
        ${RT_LIBRARY}
        pthread
    )
    
    # Linux compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

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

# Platform-specific target properties
if(PLATFORM_MACOS)
    set_target_properties(ssmtp-mailer PROPERTIES
        MACOSX_BUNDLE FALSE
        MACOSX_RPATH "@executable_path/../lib"
    )
    set_target_properties(ssmtp-mailer-lib PROPERTIES
        MACOSX_RPATH "@loader_path"
    )
endif()

# Link libraries
target_link_libraries(ssmtp-mailer
    ssmtp-mailer-lib
    ${OPENSSL_LIBRARIES}
    ${PLATFORM_LIBRARIES}
)

target_link_libraries(ssmtp-mailer-lib
    ${OPENSSL_LIBRARIES}
    ${PLATFORM_LIBRARIES}
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

# Platform-specific compile definitions
if(PLATFORM_MACOS)
    target_compile_definitions(ssmtp-mailer PRIVATE PLATFORM_MACOS=1)
    target_compile_definitions(ssmtp-mailer-lib PRIVATE PLATFORM_MACOS=1)
elseif(PLATFORM_LINUX)
    target_compile_definitions(ssmtp-mailer PRIVATE PLATFORM_LINUX=1)
    target_compile_definitions(ssmtp-mailer-lib PRIVATE PLATFORM_LINUX=1)
endif()

# Install targets
install(TARGETS ssmtp-mailer ssmtp-mailer-lib
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

# Platform-specific install paths
if(PLATFORM_MACOS)
    # macOS install paths
    install(DIRECTORY config/ DESTINATION /usr/local/etc/ssmtp-mailer)
else()
    # Linux install paths
    install(DIRECTORY config/ DESTINATION etc/ssmtp-mailer)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/tests)
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

# macOS package generation
if(PLATFORM_MACOS AND BUILD_SHARED_LIBS)
    # Create macOS package
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "ssmtp-mailer-${PROJECT_VERSION}")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_PACKAGE_NAME "ssmtp-mailer")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple SMTP Mailer")
    set(CPACK_PACKAGE_VENDOR "ssmtp-mailer")
    set(CPACK_PACKAGE_CONTACT "support@example.com")
    
    include(CPack)
endif()
