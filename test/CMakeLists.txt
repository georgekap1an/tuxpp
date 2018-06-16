file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/test)

# env config file
site_name(SITE)
configure_file(env.bat.template ${CMAKE_CURRENT_BINARY_DIR}/env.bat)
configure_file(env.sh.template ${CMAKE_CURRENT_BINARY_DIR}/env.sh)

if(WIN32)
    set(ENV_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/env.bat)
else()
    set(ENV_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/env.sh)
endif()


install(FILES ${ENV_SCRIPT} DESTINATION test)

include_directories(include)

# binary config files
if(${TUXEDO_VERSION} GREATER 1213)
    set(UBB_SECONDARY_QUEUE_LINE "SECONDARYRQ=Y")
endif()
configure_file(test_ubb.template ${CMAKE_CURRENT_BINARY_DIR}/test_ubb)
configure_file(tmadmin_input.template ${CMAKE_CURRENT_BINARY_DIR}/tmadmin_input)
configure_file(qmadmin_input.template ${CMAKE_CURRENT_BINARY_DIR}/qmadmin_input)

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/test/tuxconfig
                   COMMAND ${CMAKE_COMMAND} -E env
                   "TUXCONFIG=${CMAKE_INSTALL_PREFIX}/test/tuxconfig"
                   tmloadcf -y ${CMAKE_CURRENT_BINARY_DIR}/test_ubb
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/test_ubb)

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/test/tlog
                   COMMAND ${CMAKE_COMMAND} -E env
                   "TUXCONFIG=${CMAKE_INSTALL_PREFIX}/test/tuxconfig"
                   tmadmin -c < ${CMAKE_CURRENT_BINARY_DIR}/tmadmin_input
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tmadmin_input)

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/test/qdevice
                   COMMAND ${CMAKE_COMMAND} -E env
                   "TUXCONFIG=${CMAKE_INSTALL_PREFIX}/test/tuxconfig"
                   qmadmin ${CMAKE_INSTALL_PREFIX}/test/qdevice < ${CMAKE_CURRENT_BINARY_DIR}/qmadmin_input
                   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/qmadmin_input)

add_custom_command(OUTPUT ${CMAKE_INSTALL_PREFIX}/test/repository
                   COMMAND ${CMAKE_COMMAND} -E env
                   tmloadrepos -i ${CMAKE_CURRENT_SOURCE_DIR}/repository.template  ${CMAKE_INSTALL_PREFIX}/test/repository
                   DEPENDS repository.template)

add_custom_target(binary_configs ALL DEPENDS ${CMAKE_INSTALL_PREFIX}/test/tuxconfig ${CMAKE_INSTALL_PREFIX}/test/tlog ${CMAKE_INSTALL_PREFIX}/test/qdevice ${CMAKE_INSTALL_PREFIX}/test/repository)                   

if(${TUXEDO_VERSION} GREATER 1213)
    # record files
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/customer.R
                   COMMAND cpy2record -o ${CMAKE_CURRENT_BINARY_DIR}/customer.R ${CMAKE_CURRENT_SOURCE_DIR}/customer.cpy
                   DEPENDS customer.cpy)
                   
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/transaction.R
                   COMMAND cpy2record -o ${CMAKE_CURRENT_BINARY_DIR}/transaction.R ${CMAKE_CURRENT_SOURCE_DIR}/transaction.cpy
                   DEPENDS transaction.cpy)
                   
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/string_info.R
                   COMMAND cpy2record -o ${CMAKE_CURRENT_BINARY_DIR}/string_info.R ${CMAKE_CURRENT_SOURCE_DIR}/string_info.cpy
                   DEPENDS string_info.cpy)

    add_custom_target(records ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/customer.R ${CMAKE_CURRENT_BINARY_DIR}/transaction.R ${CMAKE_CURRENT_BINARY_DIR}/string_info.R)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/customer.R ${CMAKE_CURRENT_BINARY_DIR}/transaction.R ${CMAKE_CURRENT_BINARY_DIR}/string_info.R
        DESTINATION  test)
endif()
        
# field tables
install(FILES fields16 fields32 DESTINATION test)

# views
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/views16.V
                   COMMAND ${CMAKE_COMMAND} -E env
                   "FIELDTBLS=fields16"
                   "FLDTBLDIR=${CMAKE_SOURCE_DIR}/test"
                   viewc -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/views16
                   DEPENDS views16)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/views32.V
                   COMMAND ${CMAKE_COMMAND} -E env
                   "FIELDTBLS32=fields32"
                   "FLDTBLDIR32=${CMAKE_SOURCE_DIR}/test"
                   viewc32 -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/views32
                   DEPENDS views32)

add_custom_target(views ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/views16.V ${CMAKE_CURRENT_BINARY_DIR}/views32.V)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/views16.V ${CMAKE_CURRENT_BINARY_DIR}/views32.V DESTINATION test)

# servers
add_executable(test_server src/test_server.cpp src/test_server_main_${TUXEDO_VERSION}.cpp)
target_link_libraries(test_server tux buft fml fml32 engine  ${CMAKE_DL_LIBS} Threads::Threads tuxpp tmib trep ${SUNPRO_LINK_FLAGS})

add_executable(mssq_server src/mssq_server.cpp src/mssq_server_main_${TUXEDO_VERSION}.cpp)
target_link_libraries(mssq_server tux buft fml fml32 engine  ${CMAKE_DL_LIBS} Threads::Threads tuxpp tmib trep ${SUNPRO_LINK_FLAGS})

add_executable(posting_server src/posting_server.cpp src/posting_server_main_${TUXEDO_VERSION}.cpp)
target_link_libraries(posting_server tux buft fml fml32 engine  ${CMAKE_DL_LIBS} Threads::Threads tuxpp tmib trep ${SUNPRO_LINK_FLAGS})

# client
add_executable(test_runner src/buffer_test.cpp src/carray_test.cpp
            src/cstring_test.cpp src/decimal_number_test.cpp src/fml32_test.cpp
            src/mbstring_test.cpp src/test_runner.cpp src/util_test.cpp
            src/view16_test.cpp src/view32_test.cpp src/xml_test.cpp
            src/record_test.cpp src/fml16_test.cpp src/convert_test.cpp
            src/service_error_test.cpp src/context_test.cpp src/request_response_test.cpp
            src/conversation_test.cpp src/unsolicited_notification_test.cpp
            src/message_queuing_test.cpp src/transaction_test.cpp src/pub_sub_test.cpp
            src/admin_test.cpp src/service_test.cpp)
            
target_link_libraries(test_runner tux buft fml fml32 engine  ${CMAKE_DL_LIBS} Threads::Threads tuxpp tmib trep)

install(TARGETS test_server mssq_server posting_server test_runner DESTINATION test)

