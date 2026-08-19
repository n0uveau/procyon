add_executable(procyon main.cpp)
target_compile_features(procyon PRIVATE cxx_std_23)
target_link_libraries(procyon PRIVATE SFML::Graphics procyon_warnings)