add_library(procyon_core STATIC word_wrap.cpp markup.cpp)
target_compile_features(procyon_core PUBLIC cxx_std_23)
target_link_libraries(procyon_core PRIVATE procyon_warnings)
target_include_directories(procyon_core PUBLIC ${CMAKE_SOURCE_DIR}/src)