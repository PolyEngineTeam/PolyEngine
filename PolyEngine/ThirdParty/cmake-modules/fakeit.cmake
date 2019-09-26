add_library(FakeIt INTERFACE)
target_include_directories(FakeIt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/FakeIt/single_header/catch)