add_executable(spsc_example example.cpp)
target_link_libraries(spsc_example PRIVATE SPSC::spsc)
spsc_target_compile_options(spsc_example)
