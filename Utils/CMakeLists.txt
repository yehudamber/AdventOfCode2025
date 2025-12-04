add_library(Utils STATIC)
target_sources(Utils PUBLIC FILE_SET CXX_MODULES FILES utils.cppm)
target_compile_features(Utils PUBLIC cxx_std_23)
enable_cxx_warnings(Utils)

add_executable(testUtils test.cpp)
target_link_libraries(testUtils PRIVATE Utils)
enable_cxx_warnings(testUtils)

add_test(NAME UtilsTest COMMAND testUtils)
