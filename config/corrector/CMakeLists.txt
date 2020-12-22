configure_file( config_gen.sh.in ${CMAKE_CURRENT_BINARY_DIR}/generated/config_gen.sh @ONLY)
configure_file( correction_config.yaml.in ${CMAKE_CURRENT_BINARY_DIR}/generated/correction_config.yaml @ONLY)

install(FILES configallnuclides.sh DESTINATION config/corrector)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/config_gen.sh DESTINATION config/corrector)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/correction_config.yaml DESTINATION config/corrector)
