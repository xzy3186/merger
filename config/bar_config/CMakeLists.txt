configure_file( config_gen.sh.in ${CMAKE_CURRENT_BINARY_DIR}/generated/config_gen.sh @ONLY)

install(FILES VANDLEBars.txt DESTINATION config/bar_config)
install(FILES config_vandle_corrector.yaml DESTINATION config/bar_config)
install(FILES config_vandle_corrector_source.yaml DESTINATION config/bar_config)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/config_gen.sh DESTINATION config/bar_config)
