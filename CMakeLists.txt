cmake_minimum_required(VERSION 2.8)

#[[ In-source build check ]]
if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed.")
    message(WARNING "Remove CMakeFiles/ and CMakeCache.txt in the\
            source root before building again.")
else()
    #[[ Set project version ]]
    project(mission-control)
    set(MISSION_CONTROL_MAJOR_VERSION 0)
    set(MISSION_CONTROL_MINOR_VERSION 1)
    set(MISSION_CONTROL_PATCH_VERSION 0)
    set(MISSION_CONTROL_VERSION
        ${MISSION_CONTROL_MAJOR_VERSION}.
        ${MISSION_CONTROL_MINOR_VERSION}.
        ${MISSION_CONTROL_PATCH_VERSION})


    #[[ CONSTANTS ]]
    # Compiler options
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clangd lsp
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread")
    # File paths
    set(SOURCE_MAIN ${CMAKE_SOURCE_DIR}/src/main.cpp)
    set(SOURCE_INTERNAL_LIBS
        ${CMAKE_SOURCE_DIR}/src/mission_control.cpp
        ${CMAKE_SOURCE_DIR}/src/logger.cpp)
    #[[ CONSTANTS END ]]

    #[[ Define the output binary ]]
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    add_executable(${PROJECT_NAME} ${SOURCE_MAIN})

    #[[ link with threads ]]
    find_package (Threads)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

    #[[ Link internal libraries ]]
    add_library(internal_libs ${SOURCE_INTERNAL_LIBS})
    target_include_directories(internal_libs
        PUBLIC ${CMAKE_SOURCE_DIR}/includes
        PUBLIC ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${PROJECT_NAME} internal_libs)

    #[[ Link external libraries ]]
    add_subdirectory(libs)
    target_link_libraries(${PROJECT_NAME} external_libs)
endif("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
