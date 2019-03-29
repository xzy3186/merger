configure_file("modulefiles/merger" merger @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/merger DESTINATION share/modulefiles/)