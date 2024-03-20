cmake_minimum_required(VERSION 3.26)
project(TelegramMessageIndex)

set(CMAKE_CXX_STANDARD 23)


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

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Catch2 CONFIG REQUIRED)


link_libraries(Catch2::Catch2 Catch2::Catch2WithMain)
link_libraries(ZLIB::ZLIB)
link_libraries(OpenSSL::Crypto OpenSSL::SSL)
link_libraries(ws2_32 CRYPT32 Normaliz Psapi)

add_compile_options("/utf-8")


set(COMPILE_FILES
        Functions.cpp
        Functions.h
        Core/TelegramClientCore.cpp
        Core/TelegramClientCore.h
        Parser/MessageParser.cpp
        Parser/MessageParser.h
        Config/ProgramConfig.cpp
        Config/ProgramConfig.h
        ConsoleCtrlCapturer.cpp
        ConsoleCtrlCapturer.h
)

add_executable(TelegramMessageIndex
        main.cpp
        ${COMPILE_FILES}
)


add_executable(Catch_Test
        Catch2_Test/ProgramConfigTest.cpp
        ${COMPILE_FILES}
)