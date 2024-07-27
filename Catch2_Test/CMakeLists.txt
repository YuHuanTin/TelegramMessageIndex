
# not required
find_package(Catch2 CONFIG)

# 遍历 CMAKE_SOURCE_DIR 目录下的所有文件
#file(GLOB_RECURSE CATCH2_TEST_FILES ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.h)
#message(STATUS "CATCH2_TEST_FILES: ${CATCH2_TEST_FILES}")

set(CATCH2_TEST_FILES
        ../Core/Config/ProgramConfig.cpp
        ../Core/Config/ProgramConfig.h
)

set(OPEN_ASAN ON)
if (OPEN_ASAN AND MSVC)
    add_compile_definitions(_DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)

    add_compile_options(/fsanitize=address /Zi)
    add_link_options(/INCREMENTAL:NO)

    cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH CMAKE_CXX_COMPILER_DIR)
    message(AUTHOR_WARNING "ASAN PATH >>> PATH=${CMAKE_CXX_COMPILER_DIR} <<<, Please add this directory to RuntimeEnv to use ASAN.")
endif ()

add_executable(Catch2_Test ProgramConfigTest.cpp ${CATCH2_TEST_FILES})
target_link_libraries(Catch2_Test PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)