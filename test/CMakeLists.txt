file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
    )

add_executable(${FSM_TEST} ${SOURCES})

target_link_libraries(${FSM_TEST}
    PRIVATE ${FSM}
    )
