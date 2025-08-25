cmake_minimum_required(VERSION 3.16)
# Get system architecture for package naming
execute_process(COMMAND uname -m OUTPUT_VARIABLE SYSTEM_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "System architecture: ${SYSTEM_ARCH}")

project(ssmtp-mailer-${SYSTEM_ARCH} VERSION 0.2.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum macOS deployment version" FORCE)
    # Allow command line override of architectures
    if(NOT DEFINED CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for macOS" FORCE)
    endif()
    set(PLATFORM_MACOS TRUE)
    message(STATUS "Building for macOS (Monterey 12.0+) with architectures: ${CMAKE_OSX_ARCHITECTURES}")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    message(STATUS "Building for Linux")
elseif(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    message(STATUS "Building for Windows")
    
    # Windows-specific settings
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    
    # Set Windows SDK version if available
    if(DEFINED ENV{WINDOWS_SDK_VERSION})
        set(CMAKE_SYSTEM_VERSION $ENV{WINDOWS_SDK_VERSION})
    endif()
endif()

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)
option(ENABLE_LOGGING "Enable logging" ON)
option(ENABLE_SSL "Enable SSL/TLS support" ON)
option(USE_SYSTEM_LIBS "Use system libraries instead of Homebrew" OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(PkgConfig REQUIRED)

# Find OpenSSL
if(ENABLE_SSL)
    if(USE_SYSTEM_LIBS)
        # Try to find system OpenSSL first
        find_package(OpenSSL QUIET)
        if(NOT OpenSSL_FOUND)
            # Fall back to Homebrew OpenSSL
            find_package(OpenSSL REQUIRED)
        endif()
    else()
        find_package(OpenSSL REQUIRED)
    endif()
endif()

# Find jsoncpp for JSON parsing
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
if(NOT JSONCPP_FOUND)
    # Try to find jsoncpp without pkg-config
    find_library(JSONCPP_LIBRARIES jsoncpp)
    find_path(JSONCPP_INCLUDE_DIRS json/json.h)
    if(JSONCPP_LIBRARIES AND JSONCPP_INCLUDE_DIRS)
        set(JSONCPP_FOUND TRUE)
        set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARIES})
        set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIRS})
    endif()
endif()

if(NOT JSONCPP_FOUND)
    message(WARNING "jsoncpp not found. Service Account authentication will not be available.")
    add_definitions(-DDISABLE_SERVICE_ACCOUNT_AUTH)
else()
    message(STATUS "Found jsoncpp: ${JSONCPP_LIBRARIES}")
    add_definitions(-DENABLE_SERVICE_ACCOUNT_AUTH)
endif()

# Find libcurl for HTTP requests
find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
    # Try to find libcurl without find_package
    find_library(CURL_LIBRARIES curl)
    find_path(CURL_INCLUDE_DIRS curl/curl.h)
    if(CURL_LIBRARIES AND CURL_INCLUDE_DIRS)
        set(CURL_FOUND TRUE)
        set(CURL_LIBRARIES ${CURL_LIBRARIES})
        set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
    endif()
endif()

if(NOT CURL_FOUND)
    message(WARNING "libcurl not found. Service Account authentication will not be available.")
    add_definitions(-DDISABLE_SERVICE_ACCOUNT_AUTH)
else()
    message(STATUS "Found libcurl: ${CURL_LIBRARIES}")
    add_definitions(-DENABLE_SERVICE_ACCOUNT_AUTH)
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
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target arm64-apple-macos12.0")
    endif()
    if(CMAKE_OSX_ARCHITECTURES MATCHES ".*x86_64.*")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -target x86_64-apple-macos12.0")
    endif()
    
elseif(PLATFORM_LINUX)
    # Linux specific libraries
    # Socket functions are part of libc, no need to find_library
    # RT library for real-time functions
    find_library(RT_LIBRARY rt)
    
    set(PLATFORM_LIBRARIES 
        ${RT_LIBRARY}
        pthread
    )
    
    # Linux compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    
elseif(PLATFORM_WINDOWS)
    # Windows specific libraries
    set(PLATFORM_LIBRARIES 
        ws2_32
        iphlpapi
        crypt32
        advapi32
        userenv
        shell32
    )
    
    # Windows compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
    
    # Windows-specific definitions
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DNOMINMAX)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add jsoncpp include directories if found
if(JSONCPP_FOUND)
    include_directories(${JSONCPP_INCLUDE_DIRS})
endif()

# Add libcurl include directories if found
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
endif()

# Source files (exclude tests)
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/core/*.cpp"
    "src/utils/*.cpp"
)

# Filter out test files from main sources
list(FILTER SOURCES EXCLUDE REGEX ".*test.*\\.cpp$")

# Header files
file(GLOB_RECURSE HEADERS 
    "include/*.hpp"
    "src/*.hpp"
)

# Create main executable
add_executable(ssmtp-mailer-${SYSTEM_ARCH} ${SOURCES})

# Create shared library
add_library(ssmtp-mailer-lib-${SYSTEM_ARCH} SHARED ${SOURCES})
set_target_properties(ssmtp-mailer-lib-${SYSTEM_ARCH} PROPERTIES
    OUTPUT_NAME "ssmtp-mailer"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Platform-specific target properties
if(PLATFORM_MACOS)
    set_target_properties(ssmtp-mailer-${SYSTEM_ARCH} PROPERTIES
        MACOSX_BUNDLE FALSE
        MACOSX_RPATH "@executable_path/../lib"
    )
    set_target_properties(ssmtp-mailer-lib-${SYSTEM_ARCH} PROPERTIES
        MACOSX_RPATH "@loader_path"
    )
elseif(PLATFORM_WINDOWS)
    set_target_properties(ssmtp-mailer-${SYSTEM_ARCH} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    set_target_properties(ssmtp-mailer-lib-${SYSTEM_ARCH} PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
    )
endif()

# Link libraries
target_link_libraries(ssmtp-mailer-${SYSTEM_ARCH}
    ssmtp-mailer-lib-${SYSTEM_ARCH}
    ${OPENSSL_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${CURL_LIBRARIES}
    ${PLATFORM_LIBRARIES}
)

# Add linker flags to suppress dylib warnings on macOS
if(PLATFORM_MACOS)
    set_target_properties(ssmtp-mailer-${SYSTEM_ARCH} PROPERTIES
        LINK_FLAGS "-Wl,-w"
    )
endif()

target_link_libraries(ssmtp-mailer-lib-${SYSTEM_ARCH}
    ${OPENSSL_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${CURL_LIBRARIES}
    ${PLATFORM_LIBRARIES}
)

# Add linker flags to suppress dylib warnings on macOS
if(PLATFORM_MACOS)
    set_target_properties(ssmtp-mailer-lib-${SYSTEM_ARCH} PROPERTIES
        LINK_FLAGS "-Wl,-w"
    )
endif()

# Compiler flags
target_compile_options(ssmtp-mailer-${SYSTEM_ARCH} PRIVATE
    -Wall -Wextra -Wpedantic
    -O2 -DNDEBUG
)

target_compile_options(ssmtp-mailer-lib-${SYSTEM_ARCH} PRIVATE
    -Wall -Wextra -Wpedantic
    -O2 -DNDEBUG
    -fPIC
)

# Platform-specific compile definitions
if(PLATFORM_MACOS)
    target_compile_definitions(ssmtp-mailer-${SYSTEM_ARCH} PRIVATE PLATFORM_MACOS=1)
    target_compile_definitions(ssmtp-mailer-lib-${SYSTEM_ARCH} PRIVATE PLATFORM_MACOS=1)
elseif(PLATFORM_LINUX)
    target_compile_definitions(ssmtp-mailer-${SYSTEM_ARCH} PRIVATE PLATFORM_LINUX=1)
    target_compile_definitions(ssmtp-mailer-lib-${SYSTEM_ARCH} PRIVATE PLATFORM_LINUX=1)
elseif(PLATFORM_WINDOWS)
    target_compile_definitions(ssmtp-mailer-${SYSTEM_ARCH} PRIVATE PLATFORM_WINDOWS=1)
    target_compile_definitions(ssmtp-mailer-lib-${SYSTEM_ARCH} PRIVATE PLATFORM_WINDOWS=1)
endif()

# Install targets with platform-specific destinations
if(PLATFORM_MACOS)
    # macOS install paths
    install(TARGETS ssmtp-mailer-${SYSTEM_ARCH} ssmtp-mailer-lib-${SYSTEM_ARCH}
        RUNTIME DESTINATION /usr/local/bin
        LIBRARY DESTINATION /usr/local/lib
        ARCHIVE DESTINATION /usr/local/lib
    )
    install(DIRECTORY include/ DESTINATION /usr/local/include)
    install(DIRECTORY config/ DESTINATION /usr/local/etc/ssmtp-mailer)
    install(DIRECTORY tools/oauth2-helper/ DESTINATION /usr/local/share/ssmtp-mailer/oauth2-helper)
    install(DIRECTORY docs/ DESTINATION /usr/local/share/ssmtp-mailer/docs)
    install(FILES LICENSE README.md DESTINATION /usr/local/share/ssmtp-mailer)
    
    # Create additional directories for macOS
    install(CODE "
        # Create additional directories for OAuth2 tools and logs
        execute_process(COMMAND \${CMAKE_COMMAND} -E make_directory /usr/local/var/log/ssmtp-mailer
            WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}
            ERROR_QUIET
        )
        execute_process(COMMAND \${CMAKE_COMMAND} -E make_directory /usr/local/etc/ssmtp-mailer/oauth2
            WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}
            ERROR_QUIET
        )
    ")
    
elseif(PLATFORM_WINDOWS)
    # Windows install paths
    install(TARGETS ssmtp-mailer-${SYSTEM_ARCH} ssmtp-mailer-lib-${SYSTEM_ARCH}
        RUNTIME DESTINATION "C:/Program Files/ssmtp-mailer/bin"
        LIBRARY DESTINATION "C:/Program Files/ssmtp-mailer/lib"
        ARCHIVE DESTINATION "C:/Program Files/ssmtp-mailer/lib"
    )
    install(DIRECTORY include/ DESTINATION "C:/Program Files/ssmtp-mailer/include")
    install(DIRECTORY config/ DESTINATION "C:/Program Files/ssmtp-mailer/config")
    install(DIRECTORY tools/oauth2-helper/ DESTINATION "C:/Program Files/ssmtp-mailer/oauth2-helper")
    install(DIRECTORY docs/ DESTINATION "C:/Program Files/ssmtp-mailer/docs")
    install(FILES LICENSE README.md DESTINATION "C:/Program Files/ssmtp-mailer")
    
    # Create additional directories for Windows
    install(CODE "
        # Create additional directories for OAuth2 tools and logs
        execute_process(COMMAND \${CMAKE_COMMAND} -E make_directory \"C:/Program Files/ssmtp-mailer/logs\"
            WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}
            ERROR_QUIET
        )
        execute_process(COMMAND \${CMAKE_COMMAND} -E make_directory \"C:/Program Files/ssmtp-mailer/oauth2\"
            WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}
            ERROR_QUIET
        )
        execute_process(COMMAND \${CMAKE_COMMAND} -E make_directory \"C:/Program Files/ssmtp-mailer/temp\"
            WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}
            ERROR_QUIET
        )
    ")
    
else()
    # Linux install paths - Follow FHS standards
    install(TARGETS ssmtp-mailer-${SYSTEM_ARCH} ssmtp-mailer-lib-${SYSTEM_ARCH}
        RUNTIME DESTINATION usr/bin
        LIBRARY DESTINATION usr/lib
        ARCHIVE DESTINATION usr/lib
    )
    install(DIRECTORY include/ DESTINATION usr/include)
    install(DIRECTORY config/ DESTINATION etc/ssmtp-mailer)
    install(DIRECTORY tools/oauth2-helper/ DESTINATION usr/share/ssmtp-mailer/oauth2-helper)
    install(DIRECTORY docs/ DESTINATION usr/share/ssmtp-mailer/docs)
    install(FILES LICENSE README.md DESTINATION usr/share/ssmtp-mailer)
    
    # Create symlinks for convenience (similar to alternatives)
    install(CODE "
        # Create symlink for ssmtp-mailer binary
        execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink 
            /usr/bin/ssmtp-mailer-${SYSTEM_ARCH} /usr/local/bin/ssmtp-mailer
            WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}
            ERROR_QUIET
        )
        # Create symlink for ssmtp-mailer-lib
        execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink 
            /usr/lib/libssmtp-mailer-lib-${SYSTEM_ARCH}.so /usr/local/lib/libssmtp-mailer-lib.so
            WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}
            ERROR_QUIET
        )
        
        # Create additional directories for OAuth2 tools and logs
        execute_process(COMMAND \${CMAKE_COMMAND} -E make_directory /var/log/ssmtp-mailer
            WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}
            ERROR_QUIET
        )
        execute_process(COMMAND \${CMAKE_COMMAND} -E make_directory /etc/ssmtp-mailer/oauth2
            WORKING_DIRECTORY \${CMAKE_INSTALL_PREFIX}
            ERROR_QUIET
        )
    ")
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
    "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-${SYSTEM_ARCH}-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ssmtp-mailer-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-${SYSTEM_ARCH}-config.cmake"
    @ONLY
)

# Install CMake package configuration files
if(PLATFORM_MACOS)
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-${SYSTEM_ARCH}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-${SYSTEM_ARCH}-config-version.cmake"
        DESTINATION /usr/local/lib/cmake/ssmtp-mailer-${SYSTEM_ARCH}
    )
elseif(PLATFORM_WINDOWS)
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-${SYSTEM_ARCH}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-${SYSTEM_ARCH}-config-version.cmake"
        DESTINATION "C:/Program Files/ssmtp-mailer/lib/cmake/ssmtp-mailer-${SYSTEM_ARCH}"
    )
else()
    # Linux - standard FHS location
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-${SYSTEM_ARCH}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ssmtp-mailer-${SYSTEM_ARCH}-config-version.cmake"
        DESTINATION usr/lib/cmake/ssmtp-mailer-${SYSTEM_ARCH}
    )
endif()

# macOS package generation
if(PLATFORM_MACOS AND BUILD_SHARED_LIBS)
    # Create macOS packages (DMG and PKG)
    set(CPACK_GENERATOR "DragNDrop;productbuild")
    set(CPACK_PACKAGE_NAME "ssmtp-mailer-${SYSTEM_ARCH}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple SMTP Mailer for macOS")
    set(CPACK_PACKAGE_VENDOR "ssmtp-mailer")
    set(CPACK_PACKAGE_CONTACT "support@blburns.com")
    
    # DMG specific settings
    set(CPACK_DMG_VOLUME_NAME "ssmtp-mailer-${PROJECT_VERSION}-${SYSTEM_ARCH}")
    set(CPACK_DMG_FORMAT "UDZO")
    # Note: Background image and DS_Store setup script removed - create macos/ directory if needed
    
    # PKG specific settings (productbuild)
    set(CPACK_PRODUCTBUILD_PACKAGE_NAME "ssmtp-mailer-${PROJECT_VERSION}-${SYSTEM_ARCH}")
    set(CPACK_PRODUCTBUILD_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PRODUCTBUILD_PACKAGE_DESCRIPTION "Simple SMTP Mailer for macOS - ${SYSTEM_ARCH}")
    set(CPACK_PRODUCTBUILD_PACKAGE_VENDOR "ssmtp-mailer")
    set(CPACK_PRODUCTBUILD_PACKAGE_CONTACT "support@blburns.com")
    
    # Ensure macOS packages follow standard Unix FHS
    # Note: Package scripts removed - create macos/ directory if needed
    
    include(CPack)
    
# Linux package generation
elseif(PLATFORM_LINUX AND BUILD_SHARED_LIBS)
    # Create Linux packages (DEB, RPM, TGZ)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_PACKAGE_NAME "ssmtp-mailer-${SYSTEM_ARCH}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple SMTP Mailer for Linux")
    set(CPACK_PACKAGE_VENDOR "ssmtp-mailer")
    set(CPACK_PACKAGE_CONTACT "support@blburns.com")
    
    # DEB specific settings
    set(CPACK_DEBIAN_PACKAGE_NAME "ssmtp-mailer-${SYSTEM_ARCH}")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libssl3, libcurl4, libjsoncpp25")
    set(CPACK_DEBIAN_PACKAGE_SECTION "mail")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/ssmtp-mailer/ssmtp-mailer")
    
    # Ensure DEB package follows FHS
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/debian/postrm")
    
    # RPM specific settings
    set(CPACK_RPM_PACKAGE_NAME "ssmtp-mailer-${SYSTEM_ARCH}")
    set(CPACK_RPM_PACKAGE_REQUIRES "openssl, libcurl, jsoncpp")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Internet")
    set(CPACK_RPM_PACKAGE_URL "https://github.com/ssmtp-mailer/ssmtp-mailer")
    
    # Ensure RPM package follows FHS
    set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm/postinst")
    set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm/postun")
    
    # TGZ specific settings
    set(CPACK_TGZ_PACKAGE_NAME "ssmtp-mailer-${PROJECT_VERSION}-${SYSTEM_ARCH}")
    
    include(CPack)

# Windows package generation
elseif(PLATFORM_WINDOWS AND BUILD_SHARED_LIBS)
    # Create Windows installer
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "ssmtp-mailer-${SYSTEM_ARCH}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple SMTP Mailer for Windows")
    set(CPACK_PACKAGE_VENDOR "ssmtp-mailer")
    set(CPACK_PACKAGE_CONTACT "support@blburns.com")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "ssmtp-mailer")
    
    # NSIS specific settings
    set(CPACK_NSIS_DISPLAY_NAME "ssmtp-mailer ${PROJECT_VERSION} (${SYSTEM_ARCH})")
    set(CPACK_NSIS_PACKAGE_NAME "ssmtp-mailer-${SYSTEM_ARCH}")
    set(CPACK_NSIS_CONTACT "support@blburns.com")
    set(CPACK_NSIS_URL "https://github.com/ssmtp-mailer/ssmtp-mailer")
    set(CPACK_NSIS_ENABLE_DIRECTORY_PAGE ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_MENU_LINKS "https://github.com/ssmtp-mailer/ssmtp-mailer" "Documentation")
    
    # Windows specific settings - follow Windows conventions
    set(CPACK_NSIS_INSTALL_ROOT "C:\\Program Files")
    set(CPACK_NSIS_PROGRAM_MENU_DIR "ssmtp-mailer")
    set(CPACK_NSIS_DESKTOP_SHORTCUT ON)
    set(CPACK_NSIS_START_MENU_SHORTCUT ON)
    
    # Ensure Windows package follows Windows conventions
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        # Create Start Menu shortcuts
        CreateDirectory \\\"$SMPROGRAMS\\\\ssmtp-mailer\\\"
        CreateShortCut \\\"$SMPROGRAMS\\\\ssmtp-mailer\\\\ssmtp-mailer.lnk\\\" \\\"$INSTDIR\\\\bin\\\\ssmtp-mailer.exe\\\"
        CreateShortCut \\\"$SMPROGRAMS\\\\ssmtp-mailer\\\\Documentation.lnk\\\" \\\"$INSTDIR\\\\docs\\\\README.md\\\"
        
        # Create Desktop shortcut
        CreateShortCut \\\"$DESKTOP\\\\ssmtp-mailer.lnk\\\" \\\"$INSTDIR\\\\bin\\\\ssmtp-mailer.exe\\\"
        
        # Add to PATH
        EnVar::SetHKLM
        EnVar::AddValue \\\"Path\\\" \\\"$INSTDIR\\\\bin\\\"
    ")
    
    include(CPack)
endif()
