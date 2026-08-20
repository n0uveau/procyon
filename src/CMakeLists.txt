add_subdirectory(core)

add_executable(procyon main.cpp)
target_link_libraries(procyon PRIVATE procyon_core SFML::Graphics procyon_warnings)