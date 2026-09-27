cmake_minimum_required(VERSION 3.16)

project(AIDoctor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

# ---------------------------------------------------------------------------
# Qt 6
# ---------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ---------------------------------------------------------------------------
# llama.cpp (local, offline inference)
# ---------------------------------------------------------------------------
set(LLAMA_BUILD_COMMON   ON  CACHE BOOL "" FORCE)
set(LLAMA_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_TOOLS    OFF CACHE BOOL "" FORCE)
set(LLAMA_BUILD_SERVER   OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS    OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/llama.cpp)

# ---------------------------------------------------------------------------
# Application sources
# ---------------------------------------------------------------------------
set(AI_DOCTOR_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ChatWidget.cpp
    src/LLMEngine.cpp
    src/Worker.cpp
    src/Settings.cpp
    ui/MainWindow.ui
    ui/Settings.ui
    resources/resources.qrc
)

qt_add_executable(AIDoctor ${AI_DOCTOR_SOURCES})

target_include_directories(AIDoctor PRIVATE
    src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/llama.cpp/include
)

target_link_libraries(AIDoctor PRIVATE
    Qt6::Widgets
    llama
)

if(WIN32)
    target_link_libraries(AIDoctor PRIVATE ws2_32)
endif()

# Copy prompts and deploy Qt runtime DLLs next to the executable (Windows)
if(WIN32)
    add_custom_command(TARGET AIDoctor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:AIDoctor>/prompts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/prompts/system_prompt.txt
            $<TARGET_FILE_DIR:AIDoctor>/prompts/system_prompt.txt
        COMMAND ${WINDEPLOYQT_EXECUTABLE}
            --release --no-translations $<TARGET_FILE:AIDoctor>
        COMMENT "Copying prompt and deploying Qt DLLs"
    )
else()
    add_custom_command(TARGET AIDoctor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:AIDoctor>/prompts
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/prompts/system_prompt.txt
            $<TARGET_FILE_DIR:AIDoctor>/prompts/system_prompt.txt
        COMMENT "Copying system prompt to output directory"
    )
endif()

install(TARGETS AIDoctor RUNTIME DESTINATION bin)
