find_package(Doxygen)
if (DOXYGEN_FOUND)

    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT})
    message("Doxygen build started")

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
               COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
               OUTPUT_QUIET)
               
    add_custom_target(docs ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION doc)
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)