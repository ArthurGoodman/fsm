file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    )

add_library(${FSM} ${SOURCES})

target_include_directories(${FSM}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    )
