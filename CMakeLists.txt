cmake_minimum_required(VERSION 3.16)

project(ULGL-Embed VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(PYTHON_EXECUTABLE python python3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "" FORCE)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glfw)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glm)

set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/vendor/glad/include)
set(GLAD_SRC_DIR ${CMAKE_SOURCE_DIR}/vendor/glad/src)

add_library(glad STATIC
    ${GLAD_SRC_DIR}/glad.c
)

target_include_directories(glad PUBLIC
    ${GLAD_INCLUDE_DIR}
)

set(ULTRALIGHT_SDK_PATH "${CMAKE_SOURCE_DIR}/vendor/Ultralight" CACHE PATH "Path to Ultralight SDK")

if(EXISTS "${ULTRALIGHT_SDK_PATH}/include")
    message(STATUS "Found Ultralight SDK at: ${ULTRALIGHT_SDK_PATH}")
    message(STATUS "  - Include directory: ${ULTRALIGHT_SDK_PATH}/include")
    message(STATUS "  - Binary directory: ${ULTRALIGHT_SDK_PATH}/bin")
    message(STATUS "  - Library directory: ${ULTRALIGHT_SDK_PATH}/lib")
    
    set(ULTRALIGHT_INCLUDE_DIR "${ULTRALIGHT_SDK_PATH}/include")
    set(ULTRALIGHT_BINARY_DIR "${ULTRALIGHT_SDK_PATH}/bin")
    set(ULTRALIGHT_RESOURCES_DIR "${ULTRALIGHT_SDK_PATH}/resources")
    
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(ULTRALIGHT_LIBRARY_DIR "${ULTRALIGHT_SDK_PATH}/lib")
    else()
        set(ULTRALIGHT_LIBRARY_DIR "${ULTRALIGHT_SDK_PATH}/bin")
    endif()
    
    include_directories("${ULTRALIGHT_INCLUDE_DIR}")
    link_directories("${ULTRALIGHT_LIBRARY_DIR}")
    
    set(ULTRALIGHT_FOUND TRUE)
    add_definitions(-DULTRALIGHT_FOUND)
else()
    message(WARNING "Ultralight SDK not found at ${ULTRALIGHT_SDK_PATH}")
    message(STATUS "Please download the Ultralight SDK from https://ultralig.ht")
    message(STATUS "Extract it to: ${ULTRALIGHT_SDK_PATH}")
    set(ULTRALIGHT_FOUND FALSE)
endif()

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
    src/VertexArray.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Framebuffer.cpp
    src/Component.cpp
)

if(ULTRALIGHT_FOUND)
    target_sources(${PROJECT_NAME} PRIVATE
        src/UltralightRenderer.cpp
        src/InputEvent.cpp
        src/JSBridge.cpp
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
)

if(ULTRALIGHT_FOUND)
    if(CMAKE_SYSTEM_NAME MATCHES "Windows")
        target_link_libraries(${PROJECT_NAME} PRIVATE
            UltralightCore
            Ultralight
            WebCore
            AppCore
        )
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE
            UltralightCore
            Ultralight
            WebCore
            AppCore
        )
    endif()
    
    if(PYTHON_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying Ultralight binaries from ${ULTRALIGHT_BINARY_DIR} to $<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/smart_copy.py"
                "full"
                "${ULTRALIGHT_BINARY_DIR}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E echo "Ultralight binaries copied"
        )
        
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/smart_copy.py"
                "full"
                "${ULTRALIGHT_RESOURCES_DIR}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
            COMMENT "Copying Ultralight resources (incremental)"
        )
    else()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying Ultralight binaries from ${ULTRALIGHT_BINARY_DIR} to $<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ULTRALIGHT_BINARY_DIR}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E echo "Ultralight binaries copied"
        )
        
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ULTRALIGHT_RESOURCES_DIR}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
        )
    endif()
endif()

if(PYTHON_EXECUTABLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/app"
        COMMENT "Creating app directory"
    )
    
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/smart_copy.py"
            "build"
            "${CMAKE_SOURCE_DIR}/app"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/app"
        COMMENT "Copying React app build output (incremental)"
    )
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/app"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/app/build"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/app/build"
        COMMENT "Copying React app build output"
    )
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    if(PYTHON_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/smart_copy.py"
                "full"
                "${CMAKE_SOURCE_DIR}/assets"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
            COMMENT "Copying assets (incremental)"
        )
    else()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        )
    endif()
endif()

if(PYTHON_EXECUTABLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/copy_html_if_exists.py"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/app/build/index.html"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/app/index.html"
    )
endif()

if(UNIX AND NOT APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND chmod +x "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMENT "Setting execute permissions on ${PROJECT_NAME}"
    )
elseif(APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND chmod +x "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMENT "Setting execute permissions on ${PROJECT_NAME}"
    )
endif()

if(ULTRALIGHT_FOUND)
    message(STATUS "")
    message(STATUS "=== Build Output ===")
    message(STATUS "Executable and dependencies are in: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message(STATUS "  This directory contains:")
    message(STATUS "    - ${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
    message(STATUS "    - All Ultralight DLLs")
    message(STATUS "    - resources/ folder")
    if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
        message(STATUS "    - assets/ folder")
    endif()
    message(STATUS "    - app/ folder (React app)")
    message(STATUS "")
    message(STATUS "To run: Use 'cmake --build . --target run'")
    message(STATUS "")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(EXECUTABLE_NAME "${PROJECT_NAME}.exe")
    add_custom_target(run
        COMMAND ${CMAKE_COMMAND} -E echo "Running ${PROJECT_NAME}..."
        COMMAND ${CMAKE_COMMAND} -E chdir "$<TARGET_FILE_DIR:${PROJECT_NAME}>" "${EXECUTABLE_NAME}"
        DEPENDS ${PROJECT_NAME}
        USES_TERMINAL
        COMMAND_EXPAND_LISTS
    )
else()
    set(EXECUTABLE_NAME "${PROJECT_NAME}")
    add_custom_target(run
        COMMAND ${CMAKE_COMMAND} -E echo "Running ${PROJECT_NAME}..."
        COMMAND ${CMAKE_COMMAND} -E chdir "$<TARGET_FILE_DIR:${PROJECT_NAME}>" ./${EXECUTABLE_NAME}
        DEPENDS ${PROJECT_NAME}
        USES_TERMINAL
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${GLAD_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_SOURCE_DIR}/vendor/glm
)

if(ULTRALIGHT_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${ULTRALIGHT_INCLUDE_DIR}
    )
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
elseif(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(IOKIT_FRAMEWORK IOKit REQUIRED)
    find_library(COREVIDEO_FRAMEWORK CoreVideo REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
    )
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        GL
        X11
        pthread
        dl
    )
endif()

if(CMAKE_EXPORT_COMPILE_COMMANDS AND EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
        COMMENT "Copying compile_commands.json to source directory"
    )
endif()
