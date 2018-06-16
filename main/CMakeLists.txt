add_library(tuxpp SHARED src/util.cpp src/decimal_number.cpp
          src/buffer.cpp src/cstring.cpp src/carray.cpp src/xml.cpp src/mbstring.cpp
          src/fml16.cpp src/fml32.cpp src/record.cpp src/init_request.cpp src/convert.cpp
          src/context.cpp src/transaction.cpp src/service_error.cpp
          src/conversation.cpp src/message_queuing.cpp src/pub_sub.cpp
          src/request_response.cpp src/unsolicited_notification.cpp
          src/admin.cpp src/service.cpp)
          
set_target_properties(tuxpp PROPERTIES
                    VERSION ${PROJECT_VERSION}
                    SOVERSION ${PROJECT_VERSION_MAJOR})
                    
target_link_libraries(tuxpp ${SUNPRO_LINK_FLAGS})

install(TARGETS tuxpp DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
