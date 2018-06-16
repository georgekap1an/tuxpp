
add_executable(fmlhpp16 src/fmlhpp16.cpp)
target_link_libraries(fmlhpp16 ${CMAKE_DL_LIBS} fml ${SUNPRO_LINK_FLAGS} Threads::Threads)

add_executable(fmlhpp32 src/fmlhpp32.cpp)
target_link_libraries(fmlhpp32 ${CMAKE_DL_LIBS} fml32 ${SUNPRO_LINK_FLAGS} Threads::Threads)

install(TARGETS fmlhpp16 fmlhpp32 DESTINATION bin)