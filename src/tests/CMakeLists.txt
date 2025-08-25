# Tests CMakeLists.txt for ssmtp-mailer

# Add test executable
add_executable(ssmtp-mailer-tests-${SYSTEM_ARCH}
    test_main.cpp
    # Add more test source files here when they exist
    # test_smtp.cpp
    # test_config.cpp
)

# Link test executable with main library
target_link_libraries(ssmtp-mailer-tests-${SYSTEM_ARCH}
    ssmtp-mailer-lib-${SYSTEM_ARCH}
    ${OPENSSL_LIBRARIES}
    ${PLATFORM_LIBRARIES}
)

# Add tests to CTest
add_test(NAME ssmtp-mailer-tests-${SYSTEM_ARCH} COMMAND ssmtp-mailer-tests-${SYSTEM_ARCH})

# Set test properties
set_tests_properties(ssmtp-mailer-tests-${SYSTEM_ARCH} PROPERTIES
    TIMEOUT 300
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
