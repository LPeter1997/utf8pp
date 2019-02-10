set(UTF8PP_TEST_SOURCES
    catch.hpp
    main.cpp
    parse.cpp
    write.cpp
)

add_executable(utf8pp_test ${UTF8PP_TEST_SOURCES})
target_include_directories(utf8pp_test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src")
