cmake_minimum_required(VERSION ${CMAKE_VERSION})
project(TelegramMessageIndex CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Detail output" FORCE)

# scan all TDLib types directories 
set(TDLib_TYPES
        "Debug"
        "Release"
        "RelWithDebInfo"
)

foreach (TDLib_TYPE ${TDLib_TYPES})
    if (NOT ${CMAKE_BUILD_TYPE} STREQUAL ${TDLib_TYPE})
        continue()
    endif ()

    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/TDLib-${TDLib_TYPE})
        message(FATAL_ERROR "Directory ${CMAKE_CURRENT_SOURCE_DIR}/TDLib-${TDLib_TYPE} does not exist")
    endif ()

    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/TDLib-${TDLib_TYPE}/bin
            OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/TDLib-${TDLib_TYPE}/include
            OR NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/TDLib-${TDLib_TYPE}/lib)
        message(FATAL_ERROR "Directory ${CMAKE_CURRENT_SOURCE_DIR}/TDLib-${TDLib_TYPE} is incomplete")
    endif ()

    set(TDLib_Bin_Path ${CMAKE_CURRENT_SOURCE_DIR}/TDLib-${TDLib_TYPE}/bin)
    set(TDLib_Include_Path ${CMAKE_CURRENT_SOURCE_DIR}/TDLib-${TDLib_TYPE}/include)
    set(TDLib_Lib_Path ${CMAKE_CURRENT_SOURCE_DIR}/TDLib-${TDLib_TYPE}/lib)

    #        enum all files
    file(GLOB TDLib_Bin_Files ${TDLib_Bin_Path}/*.*)
    file(GLOB TDLib_Lib_Files ${TDLib_Lib_Path}/*.*)
    #        enum all files

endforeach ()

# copy bin files to build directory
foreach (TDLib_BIN_FILE ${TDLib_Bin_Files})
    file(COPY ${TDLib_BIN_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach ()

message(STATUS "TDLib_Include_Path: ${TDLib_Include_Path}")
message(STATUS "TDLib_Bin_Files: ${TDLib_Bin_Files}")
message(STATUS "TDLib_Lib_Files: ${TDLib_Lib_Files}")

include_directories(${TDLib_Include_Path})
link_libraries(${TDLib_Lib_Files})


# import 3-rd party library
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(concurrencpp CONFIG REQUIRED)
find_package(nameof CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

link_libraries(nlohmann_json::nlohmann_json)
link_libraries(nameof::nameof)
link_libraries(concurrencpp::concurrencpp)
link_libraries(ZLIB::ZLIB)
link_libraries(OpenSSL::Crypto OpenSSL::SSL)
link_libraries(ws2_32 CRYPT32 Normaliz Psapi)


# set utf-8
add_compile_options(/utf-8)
set(COMPILE_FILES
        main.cpp
        Core/TelegramCore.cpp
        Core/ObjectManager.cpp
        Core/Utils/TdUtils.hpp
        Core/Utils/Logger.hpp
        Core/Utils/ConsoleCtrlCapturer.cpp
)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${COMPILE_FILES})
target_precompile_headers(${PROJECT_NAME} PRIVATE Core/PCH.h)

# add test subdirectory
#add_subdirectory(Catch2_Test)


set(ENABLE_ASAN ON)
if (ENABLE_ASAN AND MSVC)
    # https://learn.microsoft.com/en-us/cpp/sanitizers/error-container-overflow?view=msvc-170
    # 如果出现以下错误则启用以下定义
    # my_static.lib(my_code.obj) : error LNK2038: mismatch detected for 'annotate_vector': value '0' doesn't match value '1' in main.obj
    target_compile_definitions(${PROJECT_NAME} PRIVATE _DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)

    target_compile_options(${PROJECT_NAME} PRIVATE /fsanitize=address /Zi)
    target_link_options(${PROJECT_NAME} PRIVATE /INCREMENTAL:NO)

    cmake_path(GET CMAKE_CXX_COMPILER PARENT_PATH CMAKE_CXX_COMPILER_DIR)
    message(AUTHOR_WARNING "ASAN PATH >>> PATH=${CMAKE_CXX_COMPILER_DIR} <<<, Please add this directory to RuntimeEnv to use ASAN.")

    file(COPY ${CMAKE_CXX_COMPILER_DIR}/clang_rt.asan_dynamic-x86_64.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    message(AUTHOR_WARNING "Auto Copy x64-windows.dll to ${CMAKE_CURRENT_BINARY_DIR}...")
endif ()
