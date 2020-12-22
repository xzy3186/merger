configure_file( gen_mergerLists.py.in ${CMAKE_CURRENT_BINARY_DIR}/generated/gen_mergerLists.py @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/gen_mergerLists.py DESTINATION mergerLists)
