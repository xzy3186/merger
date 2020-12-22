configure_file( gen_anamergerLists.py.in ${CMAKE_CURRENT_BINARY_DIR}/generated/gen_anamergerLists.py @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/gen_anamergerLists.py DESTINATION anamergerLists)
