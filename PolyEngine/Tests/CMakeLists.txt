add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/APITests)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/CoreTests)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/EngineTests)

# For calling "make tests"
add_custom_target(tests COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS 
    ${CORE_TESTS_TARGET} ${API_TESTS_TARGET} ${ENGINE_TESTS_TARGET})
set_target_properties(tests PROPERTIES FOLDER "Engine/Misc" )