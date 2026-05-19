find_package(Qt6 REQUIRED COMPONENTS Test)

function(add_pinentry_test name source)
    if("${name}" STREQUAL "tst_bridge" OR "${name}" STREQUAL "tst_protocol_flow" OR "${name}" STREQUAL "tst_thread_safety")
        add_executable(${name} ${source} ../pinentry_bridge.cpp ../secure_memory.cpp)
    elseif("${name}" STREQUAL "tst_secure_memory")
        add_executable(${name} ${source} ../secure_memory.cpp)
    else()
        add_executable(${name} ${source})
    endif()
    target_link_libraries(${name} PRIVATE
        Qt6::Test
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::Qml
        ${HYPRUTILS_LIBRARIES}
        ${LIBSECRET_LIBRARIES}
    )
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${HYPRUTILS_INCLUDE_DIRS}
        ${LIBSECRET_INCLUDE_DIRS}
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_pinentry_test(tst_assuan tst_assuan.cpp)
add_pinentry_test(tst_bridge tst_bridge.cpp)
add_pinentry_test(tst_secure_memory tst_secure_memory.cpp)
add_pinentry_test(tst_protocol_flow tst_protocol_flow.cpp)
add_pinentry_test(tst_thread_safety tst_thread_safety.cpp)

add_executable(tst_main_qml tst_main_qml.cpp ../pinentry_bridge.cpp ../secure_memory.cpp)
target_link_libraries(tst_main_qml PRIVATE
    Qt6::Test Qt6::Core Qt6::Gui Qt6::Quick Qt6::Qml
    ${HYPRUTILS_LIBRARIES} ${LIBSECRET_LIBRARIES}
)
target_include_directories(tst_main_qml PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${HYPRUTILS_INCLUDE_DIRS}
    ${LIBSECRET_INCLUDE_DIRS}
)
target_compile_definitions(tst_main_qml PRIVATE
    SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
add_test(NAME tst_main_qml COMMAND tst_main_qml)
set_tests_properties(tst_main_qml PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
