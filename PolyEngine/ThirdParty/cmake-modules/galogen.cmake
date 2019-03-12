set(GALOGEN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/galogen)
set(GALOGEN_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/galogen)
set(GALOGEN_SRCS "${GALOGEN_SRC_DIR}/galogen.cpp" "${GALOGEN_SRC_DIR}/third_party/tinyxml2.cpp")
add_executable(Galogen ${GALOGEN_SRCS})
#set_target_properties(Galogen PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${GALOGEN_BIN_DIR}) # This does not work for some reason
set_target_properties (Galogen PROPERTIES FOLDER "ThirdParty" )